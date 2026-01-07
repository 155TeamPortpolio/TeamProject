#include "Engine_Defines.h"
#include "HiZ_Culling.h"
#include "GameInstance.h"
#include "ComputeShader.h"

CHiZ_Culling::CHiZ_Culling()
{

}

HRESULT CHiZ_Culling::Initialize() {

	/*Ŭ�� ������*/
	_float2 clientSize = CGameInstance::GetInstance()->Get_ClientSize();
	m_viewport = clientSize;
	m_texSize = { (_uint)clientSize.x, (_uint)clientSize.y }; /*1�� �� �ؽ�ó ������*/
	m_threadSize = { 8, 8, 1 }; /*������ 8x8��*/

	/*�����尡 �ȼ� �׷��� ���� �� ���°� �����Ϸ��� */
	m_groupCount = {
		(m_texSize.x + m_threadSize.x - 1) / m_threadSize.x,
		(m_texSize.y + m_threadSize.y - 1) / m_threadSize.y,
		1
	};/*��ü�� + ���� ������ ���� -> �� ���带 8���� ��� ó���� �׷��� ��� �Ǵ°� �ø��� ���� 8��-1�� �����ذ���*/

	//1by1�� ������ 2,2�� �ؽ�ó�� ����
	m_mipCount = CalcMipCount(m_texSize.x, m_texSize.y);

	auto pDevice = CGameInstance::GetInstance()->Get_Device();

	/*�� ü�� ���� uav�� mip���� ����, srv�� �д� �뵵*/
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

	/*�Ӻ� uav, srv�� ����� ����*/
	m_HiZUav.resize(m_mipCount, nullptr);
	for (_uint mip = 0; mip < m_mipCount; ++mip)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_R32_FLOAT;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = mip;//HiZTex�� mip �� �常 ����
		HRESULT hr = pDevice->CreateUnorderedAccessView(m_pHiZTex, &uavDesc, &m_HiZUav[mip]);
		if (FAILED(hr)) return hr;
	}

	m_HiZSrvMip.resize(m_mipCount, nullptr);
	for (_uint mipIndex = 0; mipIndex < m_mipCount; ++mipIndex)
	{ /*��� �� ����� �뵵*/
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDescMip = {};
		srvDescMip.Format = DXGI_FORMAT_R32_FLOAT;
		srvDescMip.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDescMip.Texture2D.MostDetailedMip = mipIndex;
		srvDescMip.Texture2D.MipLevels = 1;//HiZTex�� mip �� �常 ����

		hr = pDevice->CreateShaderResourceView(m_pHiZTex, &srvDescMip, &m_HiZSrvMip[mipIndex]);
		if (FAILED(hr)) return hr;
	}
	/*��ü ��ü�ο� �ش��ϴ� SRV-> */
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = m_mipCount;
		hr = pDevice->CreateShaderResourceView(m_pHiZTex, &srvDesc, &m_pHiZSrv);
		if (FAILED(hr)) return hr;
	}

	/*������ ��� ���� ����*/
	m_pCopyBuffer = CreateDynamicCB(pDevice, sizeof(CB_CopyData));
	m_pReduceBuffer = CreateDynamicCB(pDevice, sizeof(CB_ReduceData));
	m_pOcculsionBuffer = CreateDynamicCB(pDevice, sizeof(CB_OcclusionData));

	/*3�� ������ ���ۿ� ���� �б⸦ ���ư��鼭 �� ����*/
	/*�׷��� ������� ������?��  CPU�� ��� �� �ִ� �̺�Ʈ*/
	D3D11_QUERY_DESC queryDesc = {};
	queryDesc.Query = D3D11_QUERY_EVENT;
	queryDesc.MiscFlags = 0;

	for (_uint frameIndex = 0; frameIndex < kFrameBuffered; ++frameIndex)
	{
		HRESULT queryResult = pDevice->CreateQuery(&queryDesc, &m_readbackFrames[frameIndex].copyDoneQuery);
		if (FAILED(queryResult)) return queryResult;
	}
	for (_uint i = 0; i < kFrameBuffered; ++i)
	{
		m_readbackFrames[i].keys.clear();
	}
	m_cachedKeys.clear();
	m_cachedVisibleFlags.clear();
	m_frameCursor = 0; 
	return S_OK;
}

void CHiZ_Culling::Update_HiZ(ID3D11DeviceContext* pContext)
{
	Check_Resource();
	if (!m_isReady) return;

	{ /*1�� ���� �ؽ�ó �׳� ����*/
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

	/*�Ӻ��� �ܰ躰�� ���ư���*/
	for (_uint srcMipIndex = 0; srcMipIndex < m_mipCount - 1; ++srcMipIndex)
	{
		/*2,2 �� ���������� ����*/
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
	/*SRV�ϰ� ���̴� ���� Ÿ�̹��� �޶�*/
	if (m_isReady)
		return;

	if (!m_pCopyShader) {
		m_pCopyShader = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_ComputeShader(G_GlobalLevelKey, "CS_HiZ_Copy.hlsl");
		if (m_pCopyShader) Safe_AddRef(m_pCopyShader);
	}
	if (!m_pReduceShader) {
		m_pReduceShader = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_ComputeShader(G_GlobalLevelKey, "CS_HiZ_Reduce.hlsl");
		if (m_pReduceShader) Safe_AddRef(m_pReduceShader);
	}
	if (!m_pOcclusionShader) {
		m_pOcclusionShader = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_ComputeShader(G_GlobalLevelKey, "CS_OcclusionCull.hlsl");
		if (m_pOcclusionShader) Safe_AddRef(m_pOcclusionShader);
	}
	if (!m_pDepthSrv) {
		m_pDepthSrv = CGameInstance::GetInstance()->Get_RenderSystem()->Get_EngineTargetSRV("Target_Static_Depth");
		if (m_pDepthSrv)      Safe_AddRef(m_pDepthSrv);
	}

	if (m_pDepthSrv && m_pCopyShader && m_pReduceShader && m_pOcclusionShader)
		m_isReady = true;

}
#ifdef _USING_GUI
void CHiZ_Culling::Render_GUI()
{
	if (ImGui::Begin("HiZ Occlusion"))
	{
		ImGui::Text("Frustum In      : %u", m_stats.frustumIn);
		ImGui::Text("Tested (inputs) : %u", m_stats.tested);
		ImGui::Text("Not Tested      : %u", m_stats.notTested);

	if (isTabOpen) {
		ImGui::SetNextWindowPos(ImVec2(1200, 5));
		if (ImGui::Begin("HiZ Occlusion", &isTabOpen))
		{
			float testedRatio = (float)m_stats.tested / (float)m_stats.frustumIn * 100.0f;
			float culledRatio = (m_stats.tested > 0) ? (float)m_stats.culledByOcc / (float)m_stats.tested * 100.0f : 0.0f;
			ImGui::Text("Tested Ratio     : %.1f%%", testedRatio);
			ImGui::Text("Culled Ratio     : %.1f%% (of tested)", culledRatio);
		}
	}
	else {
		ImGui::SetNextWindowPos(ImVec2(1200, 5));
		ImGui::Begin("##HiZ Occlusion_btn", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
		if (ImGui::Button("HiZ result")) { isTabOpen = !isTabOpen; }
		ImGui::End();
	}

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

/*���� �ø��� ����*/
vector<OPAQUE_PACKET> CHiZ_Culling::OcculsionCulling(const vector<OPAQUE_PACKET>& frustums)
{
	if (!m_isReady)
		return frustums;

	vector<OPAQUE_PACKET> result;
	if (frustums.empty())
		return result;

#ifdef _USING_GUI
	m_stats = {};
	m_stats.frustumIn = (uint32_t)frustums.size();
#endif

	vector<OcclusionInput> inputs;
	inputs.reserve(frustums.size());

	vector<OcclusionKey> writeKeys;
	writeKeys.reserve(frustums.size());

	auto cameraMgr = CGameInstance::GetInstance()->Get_CameraMgr();
	_matrix viewMatrix = *cameraMgr->Get_ViewMatrix();

	_float zFar = cameraMgr->Get_Far();
	_uint viewportW = (_uint)m_viewport.x;
	_uint viewportH = (_uint)m_viewport.y;

	unordered_map<OcclusionKey, const OPAQUE_PACKET*, OcclusionKeyHash, OcclusionKeyEq> currentCandidateMap;
	currentCandidateMap.reserve(frustums.size() * 2);

	for (_uint packetIndex = 0; packetIndex < frustums.size(); ++packetIndex)
	{
		const OPAQUE_PACKET& packet = frustums[packetIndex];

		OcclusionInput inputData = {};
		_uint compactIndex = (_uint)inputs.size();

		_bool ok = BuildOcclusionInput(
			packet.pModel->Get_MeshBoundingBox(packet.DrawIndex),
			_smatrix(*packet.pWorldMatrix),
			viewMatrix, viewportW, viewportH, zFar,
			compactIndex, inputData);

		if (!ok)
		{
			// �˻� ���ܴ� �ϴ� ���̰� + �����׸��ý� ����(����)
			result.push_back(packet);

			OcclusionKey key{};
			key.ObjID = packet.ObjID;
			key.drawIndex = packet.DrawIndex;

			auto it = m_hysteresis.find(key);
			if (it != m_hysteresis.end())
				it->second.hiddenStreak = 0;

			continue;
		}

		inputs.push_back(inputData);

		OcclusionKey key{};
		key.ObjID = packet.ObjID;
		key.drawIndex = packet.DrawIndex;

		writeKeys.push_back(key);
		currentCandidateMap.emplace(key, &packet);
	}

	if (inputs.empty())
		return result;

	ID3D11DeviceContext* context = CGameInstance::GetInstance()->Get_Context();
	ID3D11Device* device = CGameInstance::GetInstance()->Get_Device();

	EnsureOcclusionResources(device, (_uint)inputs.size());

	// input buffer update
	{
		D3D11_BOX box = {};
		box.left = 0;
		box.right = (UINT)(inputs.size() * sizeof(OcclusionInput));
		box.top = 0;
		box.bottom = 1;
		box.front = 0;
		box.back = 1;

		context->UpdateSubresource(m_inputBuffer, 0, &box, inputs.data(), 0, 0);
	}

	// ---- write slot ----
	const _uint writeFrameIndex = (_uint)(m_frameCounter % kFrameBuffered);
	OcclusionReadbackFrame& writeSlot = m_readbackFrames[writeFrameIndex];

	writeSlot.keys = writeKeys;

	{
		_uint clearValue[4] = { 1,1,1,1 };
		context->ClearUnorderedAccessViewUint(writeSlot.visibleUav, clearValue);
	}

#ifdef _USING_GUI
	m_stats.tested = (_uint)inputs.size();
	m_stats.notTested = m_stats.frustumIn - m_stats.tested;
#endif

	// dispatch
	m_pOcclusionShader->Bind(context);
	m_pOcclusionShader->SetSRV(context, 0, m_pHiZSrv);
	m_pOcclusionShader->SetSRV(context, 1, m_inputSrv);
	m_pOcclusionShader->SetUAV(context, 0, writeSlot.visibleUav);

	CB_OcclusionData cbData = {};
	cbData.viewportSize = { viewportW, viewportH };
	cbData.mipCount = m_mipCount;
	cbData.epsilon = 3e-3;
	cbData.inputCount = (_uint)inputs.size();

	Update_CBuffer(context, m_pOcculsionBuffer, &cbData, sizeof(cbData));
	m_pOcclusionShader->SetCB(context, 0, m_pOcculsionBuffer);

	_uint threadCount = (_uint)inputs.size();
	_uint groupCountX = (threadCount + 64 - 1) / 64;
	m_pOcclusionShader->Dispatch(context, groupCountX, 1, 1);

	{
		ID3D11ShaderResourceView* nullSrvs[2] = { nullptr, nullptr };
		ID3D11UnorderedAccessView* nullUavs[1] = { nullptr };
		context->CSSetShaderResources(0, 2, nullSrvs);
		context->CSSetUnorderedAccessViews(0, 1, nullUavs, nullptr);
	}

	// readback issue
	context->CopyResource(writeSlot.visibleStaging, writeSlot.visibleBuffer);
	context->End(writeSlot.copyDoneQuery);

	writeSlot.hasIssued = true;
	writeSlot.issuedFrame = m_frameCounter;

	// ---- IMPORTANT: �Ϸ�� ������ ���� ��ĵ�ؼ� ĳ�� ���� ----
	_bool anyRead = false;
	for (_uint slotIndex = 0; slotIndex < kFrameBuffered; ++slotIndex)
	{
		if (TryReadbackOne(context, m_readbackFrames[slotIndex]))
			anyRead = true;
	}

#ifdef _USING_GUI
	m_stats.canRead = anyRead ? 1u : 0u;
#endif

	// ĳ�ð� ������ ���������� ���� ���
	if (m_cachedKeys.empty())
	{
		for (auto& kv : currentCandidateMap)
			result.push_back(*kv.second);

		++m_frameCounter;

#ifdef _USING_GUI
		m_stats.visibleByOcc = (uint32_t)currentCandidateMap.size();
		m_stats.culledByOcc = 0;
		m_stats.outResult = (uint32_t)result.size();
#endif
		return result;
	}

	// ---- apply cached results (hysteresis) ----
	unordered_set<OcclusionKey, OcclusionKeyHash, OcclusionKeyEq> touched;
	touched.reserve(m_cachedKeys.size() * 2);

	_uint visibleCount = 0;
	_uint occludedCount = 0;

	const _uint cachedCount = (_uint)m_cachedKeys.size();
	for (_uint cachedIndex = 0; cachedIndex < cachedCount; ++cachedIndex)
	{
		const OcclusionKey& key = m_cachedKeys[cachedIndex];

		auto packetIter = currentCandidateMap.find(key);
		if (packetIter == currentCandidateMap.end())
			continue;

		const _uint flag = m_cachedVisibleFlags[cachedIndex];
		const _bool gpuVisible = (flag == 1);

		auto stateIter = m_hysteresis.find(key);
		if (stateIter == m_hysteresis.end())
			stateIter = m_hysteresis.emplace(key, OcclusionHysteresisState{}).first;

		OcclusionHysteresisState& state = stateIter->second;

		touched.emplace(key);

		if (gpuVisible)
		{
			state.hiddenStreak = 0;
			if (state.visibleStreak < 255) ++state.visibleStreak;

			if (state.isHidden && state.visibleStreak >= showAfter)
				state.isHidden = false;

			++visibleCount;
		}
		else
		{
			state.visibleStreak = 0;
			if (state.hiddenStreak < 255) ++state.hiddenStreak;

			if (!state.isHidden && state.hiddenStreak >= hideAfter)
				state.isHidden = true;

			++occludedCount;
		}

		if (!state.isHidden)
			result.push_back(*packetIter->second);
	}

	// �̹� �����ӿ� ���� ������(ĳ�ÿ� ����) �ֵ��� ������ ���̰� �ֱ�
	for (const OcclusionKey& key : writeKeys)
	{
		if (touched.find(key) != touched.end())
			continue;

		auto packetIter = currentCandidateMap.find(key);
		if (packetIter != currentCandidateMap.end())
			result.push_back(*packetIter->second);
	}

	// �ĺ����� ����� Ű�� �����׸��ý� ����
	for (auto it = m_hysteresis.begin(); it != m_hysteresis.end(); )
	{
		if (currentCandidateMap.find(it->first) == currentCandidateMap.end())
			it = m_hysteresis.erase(it);
		else
			++it;
	}

	++m_frameCounter;

#ifdef _USING_GUI
	m_stats.visibleByOcc = visibleCount;
	m_stats.culledByOcc = occludedCount;
	m_stats.outResult = (_uint)result.size();
#endif

	return result;
}



/*��Ŷ �ϳ��� ��Ŭ�������� ��ȯ �����ִ� �Լ�*/
_bool CHiZ_Culling::BuildOcclusionInput(
	const MINMAX_BOX& localAabbMinMax,
	_fmatrix worldMatrix,
	_fmatrix viewMatrix,
	_uint viewportW,
	_uint viewportH,
	_float zFar,
	_uint indexInList,
	OcclusionInput& outInput)
{
	XMFLOAT3 size{
		localAabbMinMax.vMax.x - localAabbMinMax.vMin.x,
		localAabbMinMax.vMax.y - localAabbMinMax.vMin.y,
		localAabbMinMax.vMax.z - localAabbMinMax.vMin.z
	};
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

	_float maxAxis = max(size.x, max(size.y, size.z));
	_float minAxis = min(size.x, min(size.y, size.z));

	// --- flags�� �����ϰ� "�ٷ� Ż��"���� �ʱ� ---
	_uint flags = 0;
	{
		const _bool flatRisk = (maxAxis > 0.02f) && ((minAxis / maxAxis) < 0.05f);
		const _bool hugeRisk = (maxAxis > 225.0f);
		const _bool thinRisk = (maxAxis > 0.02f) && ((minAxis / maxAxis) < 0.03f);

		if (flatRisk || hugeRisk || thinRisk)
			flags |= OCCL_FLAG_RISK_FLAT_OR_HUGE;

		// ���� ���� ����
		if (extents.y < 0.6f && maxAxis > 0.5f)
			flags |= OCCL_FLAG_RISK_GROUNDCONTACT;
	}

	BoundingBox localAabb(center, extents);
	BoundingBox worldAabb;
	localAabb.Transform(worldAabb, worldMatrix);

	XMFLOAT3 corners[8];
	worldAabb.GetCorners(corners);

	const XMMATRIX projMatrix =
		XMLoadFloat4x4(CGameInstance::GetInstance()->Get_CameraMgr()->Get_ProjMatrix());
	const XMMATRIX viewProjMatrix = XMMatrixMultiply(viewMatrix, projMatrix);

	_float minX = (_float)viewportW;
	_float minY = (_float)viewportH;
	_float maxX = 0.0f;
	_float maxY = 0.0f;

	_float objMinDepth01 = 1.0f;  // �ʱⰪ�� 1�� (�ָ�)
	_bool anyValid = false;
	_uint validCount = 0;

	const _float nearMargin = 1e-5f;
	const _float clipMargin = 1e-6f;

	for (int cornerIndex = 0; cornerIndex < 8; ++cornerIndex)
	{
		const _vector worldPos = XMLoadFloat3(&corners[cornerIndex]);

		// viewZ üũ (��/����� �ڴ� ����)
		const _vector viewPos = XMVector3TransformCoord(worldPos, viewMatrix);
		const _float viewZ = XMVectorGetZ(viewPos);
		if (viewZ <= nearMargin)
			continue; // << ������ return false ���µ�, ������ "�ڳ� �ϳ�"�� ����

		const _float depth01 = Clamp01(viewZ / zFar);
		objMinDepth01 = min(objMinDepth01, depth01);

		// clip -> ndc
		const _vector clip = XMVector4Transform(XMVectorSetW(worldPos, 1.0f), viewProjMatrix);
		const _float clipW = XMVectorGetW(clip);
		if (clipW <= clipMargin)
			continue;

		const _vector ndc = XMVectorScale(clip, 1.0f / clipW);
		const _float ndcX = XMVectorGetX(ndc);
		const _float ndcY = XMVectorGetY(ndc);

		const _float screenX = (ndcX * 0.5f + 0.5f) * (_float)viewportW;
		const _float screenY = (1.0f - (ndcY * 0.5f + 0.5f)) * (_float)viewportH;

		minX = min(minX, screenX);
		minY = min(minY, screenY);
		maxX = max(maxX, screenX);
		maxY = max(maxY, screenY);

		anyValid = true;
		++validCount;
	}

	// ��ȿ �ڳʰ� �ʹ� ������(ī�޶� ��/Ŭ�� ����) �׳� ������Ѷ�
	// => �ø� �Է��� ������ ����
	if (!anyValid || validCount < 2)
		return false;

	// ����/�ø�
	minX = floorf(minX);
	minY = floorf(minY);
	maxX = ceilf(maxX);
	maxY = ceilf(maxY);

	_float rectW = maxX - minX;
	_float rectH = maxY - minY;
	if (rectW < 1.0f || rectH < 1.0f)
		return false;

	const float areaPx = rectW * rectH;

	if (areaPx <= 4.0f)
		return false;

	float inflate = 2.0f;

	const float thinPx = min(rectW, rectH);
	if (thinPx <= 4.0f)
		inflate = 4.0f;

	const float screenCover = areaPx / (float(viewportW) * float(viewportH));
	if (screenCover >= 0.50f)
		return false;
	if (screenCover >= 0.25f)
		flags |= OCCL_FLAG_RISK_FLAT_OR_HUGE; // "ū ȭ�� ����"�� ���豺 ���

	// inflate ����
	minX -= inflate; minY -= inflate;
	maxX += inflate; maxY += inflate;

	if (minX < 0.0f) minX = 0.0f;
	if (minY < 0.0f) minY = 0.0f;
	if (maxX > (_float)viewportW) maxX = (_float)viewportW;
	if (maxY > (_float)viewportH) maxY = (_float)viewportH;

	if (maxX <= minX || maxY <= minY)
		return false;

	outInput.minX = (_uint)minX;
	outInput.minY = (_uint)minY;
	outInput.maxX = (_uint)maxX;
	outInput.maxY = (_uint)maxY;

	// objMinDepth01�� 1.0���� �ʱ�ȭ������ �״�� OK
	outInput.objMinDepth01 = objMinDepth01;

	outInput.indexInList = indexInList;
	outInput.padding = flags;

	return true;
}


/*�� �����ӿ� �ʿ��� ������Ʈ ������ �޾Ƽ�, �׸�ŭ ó�� ������ ���۸� ���Ҵ�*/
void CHiZ_Culling::EnsureOcclusionResources(ID3D11Device* pDevice, _uint requiredCount)
{
	if (requiredCount == 0) return;

	const _bool hasInputResources = (m_inputBuffer && m_inputSrv);
	const _bool hasAllFrameResources =
		(m_readbackFrames[0].visibleBuffer && m_readbackFrames[0].visibleUav && m_readbackFrames[0].visibleStaging);

	if (hasAllFrameResources && hasInputResources && m_capacity >= requiredCount)
		return;

	_uint newCapacity = (m_capacity > 0) ? m_capacity : 256;
	while (newCapacity < requiredCount) newCapacity *= 2;

	Safe_Release(m_inputSrv);
	Safe_Release(m_inputBuffer);

	for (_uint frameIndex = 0; frameIndex < kFrameBuffered; ++frameIndex)
	{
		Safe_Release(m_readbackFrames[frameIndex].visibleUav);
		Safe_Release(m_readbackFrames[frameIndex].visibleBuffer);
		Safe_Release(m_readbackFrames[frameIndex].visibleStaging);

		m_readbackFrames[frameIndex].hasIssued = false;
		m_readbackFrames[frameIndex].issuedFrame = 0;
		m_readbackFrames[frameIndex].keys.clear();
	}

	// input structured buffer
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = sizeof(OcclusionInput) * newCapacity;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		bufferDesc.StructureByteStride = sizeof(OcclusionInput);

		HRESULT result = pDevice->CreateBuffer(&bufferDesc, nullptr, &m_inputBuffer);
		if (FAILED(result)) return;
	}

	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = newCapacity;

		HRESULT result = pDevice->CreateShaderResourceView(m_inputBuffer, &srvDesc, &m_inputSrv);
		if (FAILED(result)) return;
	}

	// per-frame visible buffers
	for (_uint frameIndex = 0; frameIndex < kFrameBuffered; ++frameIndex)
	{
		{
			D3D11_BUFFER_DESC visibleDesc = {};
			visibleDesc.ByteWidth = sizeof(_uint) * newCapacity;
			visibleDesc.Usage = D3D11_USAGE_DEFAULT;
			visibleDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
			visibleDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			visibleDesc.StructureByteStride = sizeof(_uint);

			HRESULT visibleResult = pDevice->CreateBuffer(&visibleDesc, nullptr, &m_readbackFrames[frameIndex].visibleBuffer);
			if (FAILED(visibleResult)) return;
		}

		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC visibleUavDesc = {};
			visibleUavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			visibleUavDesc.Format = DXGI_FORMAT_UNKNOWN;
			visibleUavDesc.Buffer.FirstElement = 0;
			visibleUavDesc.Buffer.NumElements = newCapacity;

			HRESULT visibleUavResult = pDevice->CreateUnorderedAccessView(
				m_readbackFrames[frameIndex].visibleBuffer, &visibleUavDesc, &m_readbackFrames[frameIndex].visibleUav);
			if (FAILED(visibleUavResult)) return;
		}

		{
			D3D11_BUFFER_DESC stagingDesc = {};
			stagingDesc.ByteWidth = sizeof(_uint) * newCapacity;
			stagingDesc.Usage = D3D11_USAGE_STAGING;
			stagingDesc.BindFlags = 0;
			stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			stagingDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			stagingDesc.StructureByteStride = sizeof(_uint);

			HRESULT stagingResult = pDevice->CreateBuffer(&stagingDesc, nullptr, &m_readbackFrames[frameIndex].visibleStaging);
			if (FAILED(stagingResult)) return;
		}
	}

	m_cachedVisibleFlags.assign(newCapacity, 1);
	m_cachedKeys.clear();
	m_cachedFrame = 0;

	m_capacity = newCapacity;
}


_bool CHiZ_Culling::isQueryComplete(ID3D11DeviceContext* pContext, ID3D11Query* pQuery)
{
	if (!pQuery) return false;

	BOOL done = FALSE;
	HRESULT hr = pContext->GetData(pQuery, &done, sizeof(done), D3D11_ASYNC_GETDATA_DONOTFLUSH);
	return (hr == S_OK) && (done == TRUE);
}

_bool CHiZ_Culling::TryReadbackOne(ID3D11DeviceContext* pContext, OcclusionReadbackFrame& slot)
{
	if (!slot.hasIssued)
		return false;

	BOOL done = FALSE;
	HRESULT hr = pContext->GetData(slot.copyDoneQuery, &done, sizeof(done), D3D11_ASYNC_GETDATA_DONOTFLUSH);
	if (!(hr == S_OK && done == TRUE))
		return false;

	D3D11_MAPPED_SUBRESOURCE mapped = {};
	HRESULT mapHr = pContext->Map(slot.visibleStaging, 0, D3D11_MAP_READ, 0, &mapped);
	if (FAILED(mapHr))
		return false;

	const _uint* flags = (const _uint*)mapped.pData;
	const _uint count = (_uint)slot.keys.size();

	if (m_cachedVisibleFlags.size() < m_capacity)
		m_cachedVisibleFlags.resize(m_capacity, 1);

	// ĳ�� ���� (slot.issuedFrame�� �� �ֽ��� ����)
	if (slot.issuedFrame >= m_cachedFrame)
	{
		memcpy(m_cachedVisibleFlags.data(), flags, sizeof(_uint) * count);
		m_cachedKeys = slot.keys;
		m_cachedFrame = slot.issuedFrame;
	}

	pContext->Unmap(slot.visibleStaging, 0);

	// ���� �Һ� �Ϸ�
	slot.hasIssued = false;
	slot.keys.clear();
	slot.issuedFrame = 0;

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
	Safe_Release(m_pCopyBuffer);
	Safe_Release(m_pReduceShader);
	Safe_Release(m_pReduceBuffer);

	Safe_Release(m_pOcclusionShader);
	Safe_Release(m_pOcculsionBuffer);

	Safe_Release(m_inputBuffer);
	Safe_Release(m_inputSrv);

	for (_uint frameIndex = 0; frameIndex < kFrameBuffered; ++frameIndex)
	{
		Safe_Release(m_readbackFrames[frameIndex].visibleUav);
		Safe_Release(m_readbackFrames[frameIndex].visibleBuffer);
		Safe_Release(m_readbackFrames[frameIndex].visibleStaging);
		Safe_Release(m_readbackFrames[frameIndex].copyDoneQuery);
	}

	Safe_Release(m_pHiZSrv);
}
