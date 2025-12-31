#include "Engine_Defines.h"
#include "HiZ_Culling.h"
#include "GameInstance.h"
#include "ComputeShader.h"

CHiZ_Culling::CHiZ_Culling()
{

}

HRESULT CHiZ_Culling::Initialize() {

	/*클라 사이즈*/
	_float2 clientSize = CGameInstance::GetInstance()->Get_ClientSize();
	m_viewport= clientSize;
	m_texSize = { (_uint)clientSize.x, (_uint)clientSize.y }; /*1차 밉 텍스처 사이즈*/
	m_threadSize = { 8, 8, 1 }; /*쓰레드 8x8개*/

	/*쓰레드가 픽셀 그룹을 지을 때 남는게 없게하려고 */
	m_groupCount = {
		(m_texSize.x + m_threadSize.x - 1) / m_threadSize.x,
		(m_texSize.y + m_threadSize.y - 1) / m_threadSize.y,
		1
	};/*전체폭 + 방향 스레드 개수 -> 즉 위드를 8개씩 묶어서 처리할 그룹이 몇개나 되는가 올림을 위해 8개-1을 더해준거임*/

	//1by1될 때까지 2,2로 텍스처를 나눔
	m_mipCount = CalcMipCount(m_texSize.x, m_texSize.y);

	auto pDevice = CGameInstance::GetInstance()->Get_Device();
	
	/*밉 체인 생성 uav로 mip별로 쓰고, srv로 읽는 용도*/
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = m_texSize.x;
	desc.Height = m_texSize.y;
	desc.MipLevels = m_mipCount;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	HRESULT hr = pDevice->CreateTexture2D(&desc, nullptr, &m_pHiZTex);
	if (FAILED(hr))
		return hr;

	/*밉별 uav, srv를 만드는 과정*/
	m_HiZUav.resize(m_mipCount, nullptr);
	for (_uint mip = 0; mip < m_mipCount; ++mip)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_R32_FLOAT;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = mip;//HiZTex의 mip 한 장만 쓴다
		HRESULT hr = pDevice->CreateUnorderedAccessView(m_pHiZTex, &uavDesc, &m_HiZUav[mip]);
		if (FAILED(hr)) return hr;
	}

	m_HiZSrvMip.resize(m_mipCount, nullptr);
	for (_uint mipIndex = 0; mipIndex < m_mipCount; ++mipIndex)
	{ /*사실 상 디버깅 용도*/
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDescMip = {};
		srvDescMip.Format = DXGI_FORMAT_R32_FLOAT;
		srvDescMip.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDescMip.Texture2D.MostDetailedMip = mipIndex;
		srvDescMip.Texture2D.MipLevels = 1;//HiZTex의 mip 한 장만 본다

		hr = pDevice->CreateShaderResourceView(m_pHiZTex, &srvDescMip, &m_HiZSrvMip[mipIndex]);
		if (FAILED(hr)) return hr;
	}
	/*전체 밉체인에 해당하는 SRV-> */
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = m_mipCount;
		hr = pDevice->CreateShaderResourceView(m_pHiZTex, &srvDesc, &m_pHiZSrv);
		if (FAILED(hr)) return hr;
	}

	/*던져줄 상수 버퍼 생성*/
	m_pCopyBuffer = CreateDynamicCB(pDevice, sizeof(CB_CopyData));
	m_pReduceBuffer = CreateDynamicCB(pDevice, sizeof(CB_ReduceData));
	m_pOcculsionBuffer = CreateDynamicCB(pDevice, sizeof(CB_OcclusionData));

	
	return S_OK;
}

void CHiZ_Culling::Update_HiZ(ID3D11DeviceContext* pContext)
{
	Check_Resource();
	if (!m_isReady) return;

	{ /*1차 뎁스 텍스처 그냥 복사*/
		m_pCopyShader->Bind(pContext);
		CB_CopyData cbCopy = { m_texSize };
		Update_CBuffer(pContext, m_pCopyBuffer, &cbCopy, sizeof(cbCopy));
		m_pCopyShader->SetCB(pContext, 0, m_pCopyBuffer);
		m_pCopyShader->SetSRV(pContext, 0, m_pDepthSrv);
		m_pCopyShader->SetUAV(pContext, 0, m_HiZUav[0]);
		m_pCopyShader->Dispatch(pContext, m_groupCount.x, m_groupCount.y, m_groupCount.z);

		ID3D11ShaderResourceView* nullSrv[1] = { nullptr };
		ID3D11UnorderedAccessView* nullUav[1] = { nullptr };
		pContext->CSSetShaderResources(0, 1, nullSrv);
		pContext->CSSetUnorderedAccessViews(0, 1, nullUav, nullptr);
	}

	/*밉별로 단계별로 나아가기*/
	for (_uint srcMipIndex = 0; srcMipIndex < m_mipCount - 1; ++srcMipIndex)
	{
		/*2,2 로 나눠나가기 시작*/
		_uint srcWidth = max(1u, m_texSize.x >> srcMipIndex);
		_uint srcHeight = max(1u, m_texSize.y >> srcMipIndex);

		_uint dstWidth = max(1u, srcWidth >> 1);
		_uint dstHeight = max(1u, srcHeight >> 1);

		_uint dstMipIndex = srcMipIndex + 1;

		_uint3 groupCount =
		{
			(dstWidth + m_threadSize.x - 1) / m_threadSize.x,
			(dstHeight + m_threadSize.y - 1) / m_threadSize.y,
			1
		};

		m_pReduceShader->Bind(pContext);
		CB_ReduceData cbReduce = { { srcWidth ,srcHeight } ,{ dstWidth ,dstHeight } ,srcMipIndex };
		Update_CBuffer(pContext, m_pReduceBuffer, &cbReduce, sizeof(cbReduce));
		m_pReduceShader->SetCB(pContext, 0, m_pReduceBuffer);

		// SRV = HiZ src mip (slice SRV), UAV = HiZ dst mip
		m_pReduceShader->SetSRV(pContext, 0, m_HiZSrvMip[srcMipIndex]);
		m_pReduceShader->SetUAV(pContext, 0, m_HiZUav[dstMipIndex]);
		m_pReduceShader->Dispatch(pContext, groupCount.x, groupCount.y, groupCount.z);

		ID3D11ShaderResourceView* nullSrv[1] = { nullptr };
		ID3D11UnorderedAccessView* nullUav[1] = { nullptr };
		pContext->CSSetShaderResources(0, 1, nullSrv);
		pContext->CSSetUnorderedAccessViews(0, 1, nullUav, nullptr);
	}

}

void CHiZ_Culling::Check_Resource()
{
	/*SRV하고 쉐이더 생성 타이밍이 달라서*/
	if (m_isReady)
		return;

	if (!m_pCopyShader)
		m_pCopyShader = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_ComputeShader(G_GlobalLevelKey, "CS_HiZ_Copy.hlsl");
	if (!m_pReduceShader)
		m_pReduceShader = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_ComputeShader(G_GlobalLevelKey, "CS_HiZ_Reduce.hlsl");
	if (!m_pOcclusionShader)
		m_pOcclusionShader = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_ComputeShader(G_GlobalLevelKey, "CS_OcclusionCull.hlsl");
	if (!m_pDepthSrv)
		m_pDepthSrv = CGameInstance::GetInstance()->Get_RenderSystem()->Get_EngineTargetSRV("Target_Depth");

	//if (m_pDepthSrv)      Safe_AddRef(m_pDepthSrv);
	//if (m_pCopyShader) Safe_AddRef(m_pCopyShader);
	//if (m_pReduceShader) Safe_AddRef(m_pReduceShader);
	//if (m_pOcclusionShader) Safe_AddRef(m_pOcclusionShader);

	if (m_pDepthSrv && m_pCopyShader && m_pReduceShader)
		m_isReady = true;
}

#ifdef _USING_GUI
void CHiZ_Culling::Render_GUI()
{
	//if (ImGui::Begin("Hi_Z"))
	//{
	//	if (m_mipCount > 0)
	//	{
	//		ImGui::SliderInt("Mip", &m_DebugMip, 0, (int)m_mipCount - 1);
	//
	//		ID3D11ShaderResourceView* srv = nullptr;
	//		if (!m_HiZSrvMip.empty() && m_DebugMip >= 0 && m_DebugMip < (int)m_HiZSrvMip.size())
	//			srv = m_HiZSrvMip[m_DebugMip];
	//
	//		if (srv)
	//		{
	//			float scale = 0.25f;
	//			ImGui::Text("Mip %d", m_DebugMip);
	//			ImGui::Image((ImTextureID)srv, ImVec2(1280.0f * scale, 720.0f * scale));
	//		}
	//		else
	//		{
	//			ImGui::Text("SRV is null.");
	//		}
	//	}
	//	else
	//	{
	//		ImGui::Text("No mip chain.");
	//	}
	//}
	//ImGui::End();
}
#endif

_uint CHiZ_Culling::CalcMipCount(_uint width, _uint height)
{
	_uint mip = 1;
	while (width > 1 || height > 1)
	{
		width = (width > 1) ? (width >> 1) : 1;
		height = (height > 1) ? (height >> 1) : 1;
		++mip;
	}
	return mip;
}

ID3D11Buffer* CHiZ_Culling::CreateDynamicCB(ID3D11Device* device, _uint byteSize)
{
	D3D11_BUFFER_DESC bd = {};
	bd.ByteWidth = (byteSize + 15) & ~15u;
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	ID3D11Buffer* buffer = nullptr;
	HRESULT hr = device->CreateBuffer(&bd, nullptr, &buffer);
	return SUCCEEDED(hr) ? buffer : nullptr;
}

void CHiZ_Culling::Update_CBuffer(ID3D11DeviceContext* context, ID3D11Buffer* buffer, const void* data, _uint size)
{
	D3D11_MAPPED_SUBRESOURCE ms = {};
	context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
	memcpy(ms.pData, data, size);
	context->Unmap(buffer, 0);
}

_float CHiZ_Culling::Clamp01(_float value)
{
	if (value < 0.0f) return 0.0f;
	if (value > 1.0f) return 1.0f;
	return value;
}

/*실제 컬링을 시작*/
vector<OPAQUE_PACKET> CHiZ_Culling::OcculsionCulling(const vector<OPAQUE_PACKET>& frustums)
{
	if (!m_isReady)
		return frustums;

	vector<OPAQUE_PACKET> result;

	if (frustums.empty())
		return result;
	if (!m_isReady) 		
		return result;

	vector<OcclusionInput> inputs;
	inputs.reserve(frustums.size());

	auto cameraMgr = CGameInstance::GetInstance()->Get_CameraMgr();
	_matrix viewMatrix =*cameraMgr->Get_ViewMatrix();
	_float zFar = cameraMgr->Get_Far(); 
	_uint viewportW = (_uint)m_viewport.x; 
	_uint viewportH = (_uint)m_viewport.y;

	for (_uint packetIndex = 0; packetIndex <frustums.size(); ++packetIndex)
	{
		const OPAQUE_PACKET& packet = frustums[packetIndex];

		/*오브젝트의 오클루젼 데이터 생성*/
		OcclusionInput inputData = {};
		_bool ok = BuildOcclusionInput(packet.pModel->Get_MeshBoundingBox(packet.DrawIndex),_smatrix(*packet.pWorldMatrix),        
			viewMatrix,viewportW, viewportH,zFar,packetIndex,inputData);

		if (ok)
			inputs.push_back(inputData);
		else
		{
			result.push_back(packet); /*애매한거 -> ok 안떨어진건 그냥 보인다 처리 해버리기*/
		}
	}

	if (inputs.empty())
		return result;

	/*여기서 실제 오클루젼 진행*/
	ID3D11DeviceContext* context = CGameInstance::GetInstance()->Get_Context();
	ID3D11Device* device = CGameInstance::GetInstance()->Get_Device();

	/*버퍼 리사이즈*/
	EnsureOcclusionResources(device, inputs.size());

	D3D11_BOX box = {};
	box.left = 0;
	box.right = (UINT)(inputs.size() * sizeof(OcclusionInput));
	box.top = 0;
	box.bottom = 1;
	box.front = 0;
	box.back = 1;

	/*인풋 버퍼 삽입*/
	context->UpdateSubresource(m_inputBuffer, 0, &box, inputs.data(), 0, 0);

	m_pOcclusionShader->Bind(context);
	m_pOcclusionShader->SetSRV(context, 0, m_pHiZSrv);
	m_pOcclusionShader->SetSRV(context, 1, m_inputSrv);
	m_pOcclusionShader->SetUAV(context, 0, m_visibleUav);
	
	CB_OcclusionData cbData = {};
	cbData.viewportSize = { viewportW, viewportH };
	cbData.mipCount = m_mipCount;
	cbData.epsilon = 1e-4;
	cbData.inputCount = inputs.size();

	Update_CBuffer(context, m_pOcculsionBuffer, &cbData, sizeof(cbData));
	m_pOcclusionShader->SetCB(context, 0, m_pOcculsionBuffer);

	// dispatch
	_uint threadCount = inputs.size();
	_uint groupCountX = (threadCount + 64 - 1) / 64;
	m_pOcclusionShader->Dispatch(context, groupCountX, 1, 1);

	// Unbind
	ID3D11ShaderResourceView* nullSrvs[2] = { nullptr, nullptr };
	ID3D11UnorderedAccessView* nullUavs[1] = { nullptr };
	context->CSSetShaderResources(0, 2, nullSrvs);
	context->CSSetUnorderedAccessViews(0, 1, nullUavs, nullptr);

	// 결과 readback
	context->CopyResource(m_visibleStaging, m_visibleBuffer);

	D3D11_MAPPED_SUBRESOURCE mapped = {};
	HRESULT mapResult = context->Map(m_visibleStaging, 0, D3D11_MAP_READ, 0, &mapped);
	if (FAILED(mapResult))
	{
		for (const OPAQUE_PACKET& packet : frustums)
			result.push_back(packet);
		return result;
	}

	const _uint* flags = (const _uint*)mapped.pData;

	for (_uint inputIndex = 0; inputIndex < inputs.size(); ++inputIndex)
	{
		if (flags[inputIndex] != 0) /*이건 보인다고 적혔단느 뜻*/
		{
			_uint originalIndex = inputs[inputIndex].indexInList;
			result.push_back(frustums[originalIndex]);
		}
	}

	context->Unmap(m_visibleStaging, 0);
	return result;
}

/*패킷 하나를 오클루젼으로 변환 시켜주는 함수*/
_bool CHiZ_Culling::BuildOcclusionInput(const MINMAX_BOX& localAabbMinMax, _fmatrix worldMatrix, _fmatrix viewMatrix, _uint viewportW, _uint viewportH, _float zFar, _uint indexInList, OcclusionInput& outInput)
{
	XMFLOAT3 center{
		 (localAabbMinMax.vMin.x + localAabbMinMax.vMax.x) * 0.5f,
		 (localAabbMinMax.vMin.y + localAabbMinMax.vMax.y) * 0.5f,
		 (localAabbMinMax.vMin.z + localAabbMinMax.vMax.z) * 0.5f
	};
	XMFLOAT3 extents{
		(localAabbMinMax.vMax.x - localAabbMinMax.vMin.x) * 0.5f,
		(localAabbMinMax.vMax.y - localAabbMinMax.vMin.y) * 0.5f,
		(localAabbMinMax.vMax.z - localAabbMinMax.vMin.z) * 0.5f
	};

	BoundingBox localAabb(center, extents);

	BoundingOrientedBox localObb;
	BoundingOrientedBox::CreateFromBoundingBox(localObb, localAabb);

	BoundingOrientedBox worldObb;
	localObb.Transform(worldObb, worldMatrix);

	XMFLOAT3 corners[8];
	worldObb.GetCorners(corners);


	float minX = (_float)viewportW;
	float minY = (_float)viewportH;
	float maxX = 0.0f;
	float maxY = 0.0f;

	float objMinDepth01 = 1.0f;

	XMMATRIX projMatrix = XMLoadFloat4x4(CGameInstance::GetInstance()->Get_CameraMgr()->Get_ProjMatrix());
	XMMATRIX viewProjMatrix = XMMatrixMultiply(viewMatrix, projMatrix);
	bool anyValid = false;
	bool anyInvalid = false;

	for (int cornerIndex = 0; cornerIndex < 8; ++cornerIndex)
	{
		_vector worldPos = XMLoadFloat3(&corners[cornerIndex]);

		_vector clip = XMVector4Transform(XMVectorSetW(worldPos, 1.0f), viewProjMatrix);
		float clipW = XMVectorGetW(clip);

		if (clipW <= 1e-6f)
		{
			anyInvalid = true;
			continue;
		}

		// ndc
		_vector ndc = XMVectorScale(clip, 1.0f / clipW);
		float ndcX = XMVectorGetX(ndc);
		float ndcY = XMVectorGetY(ndc);

		float screenX = (ndcX * 0.5f + 0.5f) * (float)viewportW;
		float screenY = (1.0f - (ndcY * 0.5f + 0.5f)) * (float)viewportH;

		if (screenX < minX) minX = screenX;
		if (screenY < minY) minY = screenY;
		if (screenX > maxX) maxX = screenX;
		if (screenY > maxY) maxY = screenY;

		_vector viewPos = XMVector3TransformCoord(worldPos, viewMatrix);
		float viewZ = XMVectorGetZ(viewPos);

		if (viewZ > 0.0f)
		{
			float depth01 = Clamp01(viewZ / zFar);
			if (depth01 < objMinDepth01) objMinDepth01 = depth01;
		}

		anyValid = true;
	}

	if (!anyValid)
		return false;

	if (anyInvalid)
		return false;


	/*프러스텀에 걸린 것 같은 애들은 화면상 음수 좌표가 될 수 있으니까 클램핑*/
	if (minX < 0.0f) minX = 0.0f;
	if (minY < 0.0f) minY = 0.0f;
	if (maxX > (_float)viewportW) maxX	= (_float)viewportW;
	if (maxY > (_float)viewportH) maxY		= (_float)viewportH;

	if (maxX <= minX || maxY <= minY)
		return false;

	outInput.minX = (_uint)minX;
	outInput.minY = (_uint)minY;
	outInput.maxX = (_uint)maxX;
	outInput.maxY = (_uint)maxY;
	outInput.objMinDepth01 = objMinDepth01;
	outInput.indexInList = indexInList;
	outInput.padding = 0;
	return true;
}

/*번 프레임에 필요한 오브젝트 개수를 받아서, 그만큼 처리 가능한 버퍼를 재할당*/
void CHiZ_Culling::EnsureOcclusionResources(ID3D11Device* pDevice, _uint requiredCount)
{
	/*이번 오브젝트 요청*/
	/*요청 개수가 0이면 없음*/
	if (requiredCount == 0)
		return;

	const _bool hasAllResources =(m_inputBuffer) &&(m_inputSrv) &&(m_visibleBuffer) &&(m_visibleUav) &&(m_visibleStaging);
	if (hasAllResources && m_capacity >= requiredCount)/*현재 용량이 충분하면 갠춘*/
		return;

	/*부족하면 더 생성 시킬 것 -> 넉넉하게 2배수로*/
	_uint newCapacity = (m_capacity > 0) ? m_capacity : 256;
	while (newCapacity < requiredCount) newCapacity *= 2;

	/*기존 자원 해제*/
	Safe_Release(m_inputSrv);
	Safe_Release(m_inputBuffer);
	Safe_Release(m_visibleUav);
	Safe_Release(m_visibleBuffer);
	Safe_Release(m_visibleStaging);
	
	/*인풋(구조체를 넣으려는) 용도의 SRV를 만드는 과정 ->셰이더는 이걸을 읽고*/
	{	/*오브젝트의 인풋 데이터를 써서 셰이더로 던지는 용량을 만들어줌*/
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = sizeof(OcclusionInput) * newCapacity;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		bufferDesc.StructureByteStride = sizeof(OcclusionInput);
		HRESULT result = pDevice->CreateBuffer(&bufferDesc, nullptr, &m_inputBuffer);
		if (FAILED(result)) 
			return;
	}

	{	/*오브젝트의 버퍼 통해서 지금 srv만들어줌*/
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = newCapacity; /*용량 만큼 넣을거임*/
		HRESULT result = pDevice->CreateShaderResourceView(m_inputBuffer, &srvDesc, &m_inputSrv);
		if (FAILED(result))
			return;
	}

	/*VisibleFlags 를 아웃풋하는 용도의 UAV를 만드는 과정 -> 셰이더는 여기다 0혹은 1을 쓴다*/
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = sizeof(_uint) * newCapacity;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;        
		bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		bufferDesc.StructureByteStride = sizeof(_uint);
		HRESULT result = pDevice->CreateBuffer(&bufferDesc, nullptr, &m_visibleBuffer);
		if (FAILED(result)) return;
	}
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = newCapacity;
		HRESULT result = pDevice->CreateUnorderedAccessView(m_visibleBuffer, &uavDesc, &m_visibleUav);
		if (FAILED(result)) return;
	}

	/*읽어내는 용의 버퍼 - GPU 결과를 CPU에서 Map()으로 읽기 위한 staging 버퍼*/
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = sizeof(uint32_t) * newCapacity;
		bufferDesc.Usage = D3D11_USAGE_STAGING;
		bufferDesc.BindFlags = 0;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		HRESULT result = pDevice->CreateBuffer(&bufferDesc, nullptr, &m_visibleStaging);
		if (FAILED(result)) return;
	}

	m_capacity = newCapacity;
}

CHiZ_Culling* CHiZ_Culling::Create()
{
	CHiZ_Culling* instance = new CHiZ_Culling;
	if (FAILED(instance->Initialize())) {
		Safe_Release(instance);
	}
	return instance;
}


void CHiZ_Culling::Free()
{
	Safe_Release(m_pHiZTex);

	for (auto& srvMip : m_HiZSrvMip)
		Safe_Release(srvMip);
	for (auto& uav : m_HiZUav)
		Safe_Release(uav);

	//Safe_Release(m_pDepthSrv);
	//Safe_Release(m_pCopyShader);
	Safe_Release(m_pCopyBuffer);
	//Safe_Release(m_pReduceShader);
	Safe_Release(m_pReduceBuffer);

	//Safe_Release(m_pOcclusionShader);
	Safe_Release(m_pOcculsionBuffer);

	Safe_Release(m_inputBuffer);
	Safe_Release(m_inputSrv);
	Safe_Release(m_visibleBuffer);
	Safe_Release(m_visibleUav);
	Safe_Release(m_visibleStaging);

	Safe_Release(m_pHiZSrv);
}
