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
		cbReduce.srcSize = { srcWidth ,srcHeight};
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
	if (!m_pDepthSrv)
		m_pDepthSrv = CGameInstance::GetInstance()->Get_RenderSystem()->Get_EngineTargetSRV("Target_Depth");

	if (m_pDepthSrv)      Safe_AddRef(m_pDepthSrv);
	if (m_pCopyShader) Safe_AddRef(m_pCopyShader);
	if (m_pReduceShader) Safe_AddRef(m_pReduceShader);

	if (m_pDepthSrv && m_pCopyShader&& m_pReduceShader)
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

void CHiZ_Culling::Update_CBuffer(ID3D11DeviceContext* ctx, ID3D11Buffer* cb, const void* data, UINT size)
{
	D3D11_MAPPED_SUBRESOURCE ms = {};
	ctx->Map(cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
	memcpy(ms.pData, data, size);
	ctx->Unmap(cb, 0);
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
