#include "Engine_Defines.h"
#include "HiZ_Culling.h"
#include "GameInstance.h"
#include "ComputeShader.h"

CHiZ_Culling::CHiZ_Culling()
{
}

HRESULT CHiZ_Culling::Initialize() {

	_float2 clientSize = CGameInstance::GetInstance()->Get_ClientSize();
	m_texSize = { (_uint)clientSize.x, (_uint)clientSize.y };

	m_threadSize = { 8, 8, 1 };

	/*쓰레드가 픽셀 그룹을 지을 때 남는게 없게하려고 */
	m_groupCount = {
		(m_texSize.x + m_threadSize.x - 1) / m_threadSize.x,
		(m_texSize.y + m_threadSize.y - 1) / m_threadSize.y,
		1
	};
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

	return S_OK;
}

void CHiZ_Culling::Update_HiZ(ID3D11DeviceContext* pContext)
{
	Check_Resource();
	if (!m_isReady)
		return;
	_uint mipCount = m_mipCount; // CalcMipCount 결과

	for (_uint mip = 0; mip < mipCount - 1; ++mip)
	{
		_uint srcW = max(1u, m_texSize.x >> mip);
		_uint srcH = max(1u, m_texSize.y >> mip);

		_uint dstW = max(1u, srcW >> 1);
		_uint dstH = max(1u, srcH >> 1);

		_uint3 groupCount =
		{
			(dstW + m_threadSize.x - 1) / m_threadSize.x,
			(dstH + m_threadSize.y - 1) / m_threadSize.y,
			1
		};

		m_pComputeShader->Bind(pContext);

		if (mip == 0)
			m_pComputeShader->SetSRV(pContext, 0, m_pDepthSrv); 
		else
			m_pComputeShader->SetSRV(pContext, 0, m_pHiZSrv);

		m_pComputeShader->SetUAV(pContext, 0, m_HiZUav[mip]);

		m_pComputeShader->Dispatch(
			pContext,
			groupCount.x,
			groupCount.y,
			groupCount.z
		);

		// 반드시 unbind (다음 패스 안전)
		ID3D11ShaderResourceView* nullSrv[1] = { nullptr };
		ID3D11UnorderedAccessView* nullUav[1] = { nullptr };
		pContext->CSSetShaderResources(0, 1, nullSrv);
		pContext->CSSetUnorderedAccessViews(0, 1, nullUav, nullptr);
	}
}

void CHiZ_Culling::Check_Resource()
{
	if (m_isReady)
		return;

	if (!m_pComputeShader)
		m_pComputeShader = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_ComputeShader(G_GlobalLevelKey, "CS_Hi_Z.hlsl");
	if (!m_pDepthSrv)
		m_pDepthSrv = CGameInstance::GetInstance()->Get_RenderSystem()->Get_EngineTargetSRV("Target_Depth");

	if (m_pDepthSrv)      Safe_AddRef(m_pDepthSrv);
	if (m_pComputeShader) Safe_AddRef(m_pComputeShader);

	if (m_pDepthSrv && m_pComputeShader)
		m_isReady = true;
}

#ifdef _USING_GUI
void CHiZ_Culling::Render_GUI()
{
	if (ImGui::Begin("Hi_Z"))
	{
		//D3D11ShaderResourceView* pSRV = m_pHiZSrv;
		//f (pSRV)
		//
		//	ImGui::Image((ImTextureID)pSRV,
		//		ImVec2(1280 / 5, 720 / 5));
		//
	}
	ImGui::End();
}
#endif // _USING_GUI


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

	Safe_Release(m_pDepthSrv);
	Safe_Release(m_pComputeShader);
	for (auto& uav : m_HiZUav)
	{
		Safe_Release(uav);
	}
}
