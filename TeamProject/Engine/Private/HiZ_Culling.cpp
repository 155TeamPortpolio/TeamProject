#include "Engine_Defines.h"
#include "HiZ_Culling.h"
#include "GameInstance.h"
#include "ComputeShader.h"

CHiZ_Culling::CHiZ_Culling()
{
	//	Copy pass : Depth SRV → HiZ mip0 UAV(그냥 복사)
	//	Reduce loop : HiZ mip(src) SRV → HiZ mip(dst) UAV, dst = src + 1 (2×2 min)

}

HRESULT CHiZ_Culling::Initialize() {

	_float2 clientSize = CGameInstance::GetInstance()->Get_ClientSize();
	m_viewport= clientSize;
	m_texSize = { (_uint)clientSize.x, (_uint)clientSize.y };
	m_threadSize = { 8, 8, 1 };

	/*쓰레드가 픽셀 그룹을 지을 때 남는게 없게하려고 */
	m_groupCount = {
		(m_texSize.x + m_threadSize.x - 1) / m_threadSize.x,
		(m_texSize.y + m_threadSize.y - 1) / m_threadSize.y,
		1
	};

	//1by1될 때까지 2,2로 나눔
	m_mipCount = CalcMipCount(m_texSize.x, m_texSize.y);

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = m_texSize.x;
	desc.Height = m_texSize.y;

	auto pDevice = CGameInstance::GetInstance()->Get_Device();
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

	m_HiZUav.resize(m_mipCount, nullptr);
	for (_uint mip = 0; mip < m_mipCount; ++mip)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_R32_FLOAT;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = mip;

		HRESULT hr = pDevice->CreateUnorderedAccessView(m_pHiZTex, &uavDesc, &m_HiZUav[mip]);
		if (FAILED(hr)) return hr;
	}

	m_HiZSrvMip.resize(m_mipCount, nullptr);
	for (_uint mipIndex = 0; mipIndex < m_mipCount; ++mipIndex)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDescMip = {};
		srvDescMip.Format = DXGI_FORMAT_R32_FLOAT;
		srvDescMip.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDescMip.Texture2D.MostDetailedMip = mipIndex;
		srvDescMip.Texture2D.MipLevels = 1;

		hr = pDevice->CreateShaderResourceView(m_pHiZTex, &srvDescMip, &m_HiZSrvMip[mipIndex]);
		if (FAILED(hr)) return hr;
	}

	m_pCopyBuffer = CreateDynamicCB(pDevice, sizeof(CB_CopyData));
	m_pReduceBuffer = CreateDynamicCB(pDevice, sizeof(CB_ReduceData));


	return S_OK;
}

void CHiZ_Culling::Update_HiZ(ID3D11DeviceContext* pContext)
{
	Check_Resource();
	if (!m_isReady) return;

	{
		_uint dstWidth = m_texSize.x;
		_uint dstHeight = m_texSize.y;

		_uint3 groupCount =
		{
			(dstWidth + m_threadSize.x - 1) / m_threadSize.x,
			(dstHeight + m_threadSize.y - 1) / m_threadSize.y,
			1
		};

		m_pCopyShader->Bind(pContext);

		CB_CopyData cbCopy = {};
		cbCopy.dstSize = { dstWidth ,dstHeight };

		Update_CBuffer(pContext, m_pCopyBuffer, &cbCopy, sizeof(cbCopy));

		m_pCopyShader->SetCB(pContext, 0, m_pCopyBuffer);
		m_pCopyShader->SetSRV(pContext, 0, m_pDepthSrv);
		m_pCopyShader->SetUAV(pContext, 0, m_HiZUav[0]);

		m_pCopyShader->Dispatch(pContext, groupCount.x, groupCount.y, groupCount.z);

		ID3D11ShaderResourceView* nullSrv[1] = { nullptr };
		ID3D11UnorderedAccessView* nullUav[1] = { nullptr };
		pContext->CSSetShaderResources(0, 1, nullSrv);
		pContext->CSSetUnorderedAccessViews(0, 1, nullUav, nullptr);
	}

	for (_uint srcMipIndex = 0; srcMipIndex < m_mipCount - 1; ++srcMipIndex)
	{
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
		CB_ReduceData cbReduce = {};
		cbReduce.srcSize = { srcWidth ,srcHeight };
		cbReduce.dstSize = { dstWidth ,dstHeight };

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

	if (m_pDepthSrv)      Safe_AddRef(m_pDepthSrv);
	if (m_pCopyShader) Safe_AddRef(m_pCopyShader);
	if (m_pReduceShader) Safe_AddRef(m_pReduceShader);
	if (m_pOcclusionShader) Safe_AddRef(m_pOcclusionShader);

	if (m_pDepthSrv && m_pCopyShader && m_pReduceShader)
		m_isReady = true;
}

#ifdef _USING_GUI
void CHiZ_Culling::Render_GUI()
{
	if (ImGui::Begin("Hi_Z"))
	{
		if (m_mipCount > 0)
		{
			ImGui::SliderInt("Mip", &m_DebugMip, 0, (int)m_mipCount - 1);

			ID3D11ShaderResourceView* srv = nullptr;
			if (!m_HiZSrvMip.empty() && m_DebugMip >= 0 && m_DebugMip < (int)m_HiZSrvMip.size())
				srv = m_HiZSrvMip[m_DebugMip];

			if (srv)
			{
				float scale = 0.25f;
				ImGui::Text("Mip %d", m_DebugMip);
				ImGui::Image((ImTextureID)srv, ImVec2(1280.0f * scale, 720.0f * scale));
			}
			else
			{
				ImGui::Text("SRV is null.");
			}
		}
		else
		{
			ImGui::Text("No mip chain.");
		}
	}
	ImGui::End();
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
	ctx->Map(cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
	memcpy(ms.pData, data, size);
	ctx->Unmap(cb, 0);
}

_float CHiZ_Culling::Clamp01(_float value)
{
	if (value < 0.0f) return 0.0f;
	if (value > 1.0f) return 1.0f;
	return value;
}

vector<OPAQUE_PACKET> CHiZ_Culling::OcculsionCulling(const vector<OPAQUE_PACKET>& frustums)
{
	vector<OPAQUE_PACKET> result;

	if (frustums.empty())
		return result;

	vector<OcclusionInput> inputs;
	inputs.reserve(frustums.size());

	auto cameraMgr = CGameInstance::GetInstance()->Get_CameraMgr();
	_matrix viewMatrix =*cameraMgr->Get_ViewMatrix();
	_float zFar = cameraMgr->Get_Far(); // 너 엔진에 맞게
	_uint viewportW = (_uint)m_viewport.x; // 너가 가진 클라 크기
	_uint viewportH = (_uint)m_viewport.y;

	for (_uint packetIndex = 0; packetIndex <frustums.size(); ++packetIndex)
	{
		const OPAQUE_PACKET& packet = frustums[packetIndex];

		OcclusionInput inputData = {};
		_bool ok = BuildOcclusionInput(
			packet.pModel->Get_MeshBoundingBox(packet.DrawIndex),              
			_smatrix(*packet.pWorldMatrix),        
			viewMatrix,
			viewportW, viewportH,
			zFar,
			packetIndex,
			inputData);

		if (ok)
			inputs.push_back(inputData);
		else
		{
			// rect 계산 실패한 건 보수적으로 visible 처리
			// 입력에 안 넣고 그냥 결과에 포함
			result.push_back(packet);
		}
	}

	if (inputs.empty())
		return result;

	ID3D11DeviceContext* context = CGameInstance::GetInstance()->Get_Context();
	ID3D11Device* device = CGameInstance::GetInstance()->Get_Device();

	EnsureOcclusionResources(device, (uint)inputs.size()); // 너가 만들어야 하는 함수(리사이즈)

	context->UpdateSubresource(m_pOcclusionBuffer, 0, nullptr, inputs.data(), 0, 0);

	// 3) CS 바인딩
	m_pOcclusionShader->Bind(context);

	// t0 = HiZ SRV
	context->CSSetShaderResources(0, 1, &m_hizSrv);

	// t1 = inputs SRV
	context->CSSetShaderResources(1, 1, &m_occlusionInputSrv);

	// u0 = visible flags UAV
	context->CSSetUnorderedAccessViews(0, 1, &m_visibleFlagUav, nullptr);

	// b0 = constants
	struct CB_OcclusionData
	{
		uint32_t viewportW;
		uint32_t viewportH;
		uint32_t mipCount;
		float    epsilon;
		uint32_t inputCount;
		float    pad0, pad1, pad2;
	};

	CB_OcclusionData cbData = {};
	cbData.viewportW = viewportW;
	cbData.viewportH = viewportH;
	cbData.mipCount = m_mipCount;
	cbData.epsilon = 5e-4f;
	cbData.inputCount = (uint32_t)inputs.size();

	m_pOcclusionShader->SetCB(context, 0, &cbData); // 네 SetCB가 내부에서 상수버퍼 업데이트/바인딩한다고 가정

	// dispatch
	uint32_t threadCount = (uint32_t)inputs.size();
	uint32_t groupCountX = (threadCount + 64 - 1) / 64;
	m_pOcclusionShader->Dispatch(context, groupCountX, 1, 1);

	// 4) Unbind
	ID3D11ShaderResourceView* nullSrvs[2] = { nullptr, nullptr };
	ID3D11UnorderedAccessView* nullUavs[1] = { nullptr };
	context->CSSetShaderResources(0, 2, nullSrvs);
	context->CSSetUnorderedAccessViews(0, 1, nullUavs, nullptr);

	// 5) 결과 readback
	context->CopyResource(m_visibleFlagStaging, m_visibleFlagBuffer);

	D3D11_MAPPED_SUBRESOURCE mapped = {};
	HRESULT mapResult = context->Map(m_visibleFlagStaging, 0, D3D11_MAP_READ, 0, &mapped);
	if (FAILED(mapResult))
	{
		// readback 실패면 보수적으로 다 보이게
		for (const OPAQUE_PACKET& packet : frustums)
			result.push_back(packet);
		return result;
	}

	const uint32_t* flags = (const uint32_t*)mapped.pData;

	// 6) 필터링: inputs에 들어간 애들만 flags로 판단
	// inputs에 안 들어간 애들은 위에서 이미 result에 넣었음
	for (uint32_t inputIndex = 0; inputIndex < (uint32_t)inputs.size(); ++inputIndex)
	{
		if (flags[inputIndex] != 0)
		{
			uint32_t originalIndex = inputs[inputIndex].indexInList;
			result.push_back(frustums[originalIndex]);
		}
	}

	context->Unmap(m_visibleFlagStaging, 0);
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
	bool anyValid = false;

	XMMATRIX projMatrix = XMLoadFloat4x4(CGameInstance::GetInstance()->Get_CameraMgr()->Get_ProjMatrix());
	XMMATRIX viewProjMatrix = XMMatrixMultiply(viewMatrix, projMatrix);

	/*스크린 좌표로 코너를 보내는 중*/
	for (int cornerIndex = 0; cornerIndex < 8; ++cornerIndex)
	{
		_vector worldPos = XMLoadFloat3(&corners[cornerIndex]);

		// view space z
		_vector viewPos = XMVector3TransformCoord(worldPos, viewMatrix);
		float viewZ = XMVectorGetZ(viewPos);

		// 카메라 뒤(또는 0 이하)인 코너는 depth01 의미가 약함. 그래도 보수적으로 clamp.
		float depth01 = Clamp01(viewZ / zFar);
		if (depth01 < objMinDepth01) objMinDepth01 = depth01;

		// clip -> ndc
		_vector clip = XMVector4Transform(XMVectorSetW(worldPos, 1.0f), viewProjMatrix);
		float clipW = XMVectorGetW(clip);

		if (clipW <= 1e-6f)
			continue; /*Z나누기가 0이 나오지 않도록함*/

		/*Z나누기 진행*/
		_vector ndc = XMVectorScale(clip, 1.0f / clipW);
		float ndcX = XMVectorGetX(ndc);
		float ndcY = XMVectorGetY(ndc);

		float screenX = (ndcX * 0.5f + 0.5f) * (float)viewportW;
		float screenY = (1.0f - (ndcY * 0.5f + 0.5f)) * (float)viewportH;

		/*화면상 차지하는 영역의 민맥스 */
		if (screenX < minX) minX = screenX;
		if (screenY < minY) minY = screenY;
		if (screenX > maxX) maxX = screenX;
		if (screenY > maxY) maxY = screenY;

		anyValid = true;
	}

	if (!anyValid)
		return false;

	/*프러스텀에 걸린 것 같은 애들은 화면상 음수 좌표가 될 수 있으니까 클램핑*/
	if (minX < 0.0f) minX = 0.0f;
	if (minY < 0.0f) minY = 0.0f;
	if (maxX > (_float)viewportW) maxX	= (_float)viewportW;
	if (maxY > (_float)viewportH) maxY		= (_float)viewportH;

	if (maxX <= minX || maxY <= minY)
		return false;

	outInput.minX = (uint32_t)minX;
	outInput.minY = (uint32_t)minY;
	outInput.maxX = (uint32_t)maxX;
	outInput.maxY = (uint32_t)maxY;
	outInput.objMinDepth01 = objMinDepth01;
	outInput.indexInList = indexInList;
	outInput.padding = 0;
	return true;
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

	Safe_Release(m_pDepthSrv);
	Safe_Release(m_pCopyShader);
	Safe_Release(m_pReduceShader);
	Safe_Release(m_pCopyBuffer);
	Safe_Release(m_pReduceBuffer);
}
