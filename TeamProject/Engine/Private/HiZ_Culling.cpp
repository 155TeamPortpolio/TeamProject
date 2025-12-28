#include "Engine_Defines.h"
#include "HiZ_Culling.h"
#include "GameInstance.h"
#include "ComputeShader.h"

CHiZ_Culling::CHiZ_Culling()
{
}

HRESULT CHiZ_Culling::Initialize() {

	_float2 size = CGameInstance::GetInstance()->Get_ClientSize();

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = (_uint)size.x;
	desc.Height = (_uint)size.y;

	auto pDevice = CGameInstance::GetInstance()->Get_Device();
	desc.MipLevels = 1;
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

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_R32_FLOAT;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;

	hr = pDevice->CreateUnorderedAccessView(m_pHiZTex, &uavDesc, &m_pHiZ0Uav);
	if (FAILED(hr))
		return hr;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	hr = pDevice->CreateShaderResourceView(m_pHiZTex, &srvDesc, &m_phiZ0Srv);
	if (FAILED(hr))
		return hr;

	m_pComputeShader = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_ComputeShader(G_GlobalLevelKey, "CS_Hi_Z.hlsl");
	m_pDepthSrv = CGameInstance::GetInstance()->Get_RenderSystem()->Get_EngineTargetSRV("Target_Depth");

	Safe_AddRef(m_pDepthSrv);
	Safe_AddRef(m_pComputeShader);

	return S_OK;
}

void CHiZ_Culling::Update_HiZ(ID3D11DeviceContext* pContext)
{
	ID3D11ShaderResourceView* nullSrv[1] = { nullptr };
	pContext->PSSetShaderResources(0, 1, nullSrv);
	pContext->OMSetRenderTargets(0, nullptr, nullptr);
	m_pComputeShader->Bind(pContext);
	m_pComputeShader->SetSRV(pContext, 0, m_pDepthSrv);
	m_pComputeShader->SetUAV(pContext, 0, m_pHiZ0Uav);

	UINT gx = (m_iGroup.x + 7) / 8;
	UINT gy = (m_iGroup.y + 7) / 8;
	m_pComputeShader->Dispatch(pContext, gx, gy, m_iGroup.z);

	ID3D11UnorderedAccessView* nullUav[1] = { nullptr };
	pContext->CSSetShaderResources(0, 1, nullSrv);
	pContext->CSSetUnorderedAccessViews(0, 1, nullUav, nullptr);
	pContext->CSSetShader(nullptr, nullptr, 0);
}

#ifdef _USING_GUI
void CHiZ_Culling::Render_GUI()
{
	if (ImGui::Begin("Hi_Z"))
	{
		ID3D11ShaderResourceView* pSRV = m_phiZ0Srv;
		if (pSRV)
		{
			ImGui::Image((ImTextureID)pSRV,
				ImVec2(1280 / 5, 720 / 5));
		}
	}
	ImGui::End();
}
#endif // _USING_GUI


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
	Safe_Release(m_pHiZ0Uav);
	Safe_Release(m_phiZ0Srv);
	Safe_Release(m_pDepthSrv);
	Safe_Release(m_pComputeShader);
}
