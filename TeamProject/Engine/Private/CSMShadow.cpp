#include "Engine_Defines.h"
#include "CSMShadow.h"
#include "GameInstance.h"

CCSMShadow::CCSMShadow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :m_pDevice(pDevice), m_pContext(pContext)
{
    Safe_AddRef(pDevice);
    Safe_AddRef(pContext);
}

HRESULT CCSMShadow::Initialize(_uint shadowSize)
{
    m_iShadowMapSize = shadowSize;

    D3D11_TEXTURE2D_DESC TextureDesc = {};
    ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

    TextureDesc.Width = m_iShadowMapSize;
    TextureDesc.Height = m_iShadowMapSize;
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = m_iNumCascades;
    TextureDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    TextureDesc.SampleDesc.Count = 1;
    TextureDesc.Usage = D3D11_USAGE_DEFAULT;
    TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

    if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pShadowMapArray)))
        return E_FAIL;

    D3D11_DEPTH_STENCIL_VIEW_DESC pDepthStencilTexture = {};
    ZeroMemory(&TextureDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

    pDepthStencilTexture.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    pDepthStencilTexture.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
    pDepthStencilTexture.Texture2DArray.MipSlice = 0;
    pDepthStencilTexture.Texture2DArray.ArraySize = 1;

    for (UINT i = 0; i < m_iNumCascades; ++i)
    {
        pDepthStencilTexture.Texture2DArray.FirstArraySlice = i;
        if (FAILED(m_pDevice->CreateDepthStencilView(m_pShadowMapArray, &pDepthStencilTexture, &m_pDSV[i])))
            return E_FAIL;
    }

    // Shader Resource View 생성
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

    srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    srvDesc.Texture2DArray.MipLevels = 1;
    srvDesc.Texture2DArray.ArraySize = m_iNumCascades;

    if (FAILED(m_pDevice->CreateShaderResourceView(m_pShadowMapArray, &srvDesc, &m_pShadowMapSRV)))
        return E_FAIL;

    m_viewPort.TopLeftX = 0.0f;
    m_viewPort.TopLeftY = 0.0f;
    m_viewPort.Width = static_cast<_float>(m_iShadowMapSize);
    m_viewPort.Height = static_cast<_float>(m_iShadowMapSize);
    m_viewPort.MinDepth = 0.0f;
    m_viewPort.MaxDepth = 1.0f;

    return S_OK;
}

void CCSMShadow::Update(const _vector& lightDir)
{
    auto CamMgr = CGameInstance::GetInstance()->Get_CameraMgr();
    _matrix view = *CamMgr->Get_ViewMatrix();
    _matrix proj = *CamMgr->Get_ProjMatrix();

    const _matrix shadowview = *CamMgr->Get_ShadowViewMatrix();

    _float nearPlane = CamMgr->Get_Near();
    _float farPlane = CamMgr->Get_Far();

     CalculateCascadeSplits(nearPlane, farPlane);

    for (_uint i = 0; i < m_iNumCascades; ++i)
    {
        _matrix cascadeProj = proj;
        cascadeProj.r[2] = XMVectorSet(0, 0,
            m_fCascadeSplits[i + 1] / (m_fCascadeSplits[i + 1] - m_fCascadeSplits[i]),
            1.0f);
        cascadeProj.r[3] = XMVectorSet(0, 0,
            -m_fCascadeSplits[i] * m_fCascadeSplits[i + 1] /
            (m_fCascadeSplits[i + 1] - m_fCascadeSplits[i]),
            0);

        // 절두체 코너 계산
        _vector frustumCorners[8];
        CalculateFrustumCorners(view * cascadeProj,
            m_fCascadeSplits[i],
            m_fCascadeSplits[i + 1],
            frustumCorners);

        // Light View-Projection 생성
        m_lightViewProj[i] = CreateLightViewProj(frustumCorners, lightDir);
    }
}

void CCSMShadow::Begin_ShadowRender(UINT cascadeIndex)
{
    m_pContext->ClearDepthStencilView(m_pDSV[cascadeIndex], D3D11_CLEAR_DEPTH, 1.0f, 0);

    m_pContext->OMSetRenderTargets(0, nullptr, m_pDSV[cascadeIndex]);

    m_pContext->RSSetViewports(1, &m_viewPort);
}

void CCSMShadow::End_ShadowRender()
{
    ID3D11RenderTargetView* nullRTV = nullptr;
    m_pContext->OMSetRenderTargets(1, &nullRTV, nullptr);
}

const _matrix& CCSMShadow::GetLightViewProj(UINT cascadeIndex) const
{
    return m_lightViewProj[cascadeIndex];
}

void CCSMShadow::CalculateCascadeSplits(_float fNear, _float fFar)
{
    _float lambda = 0.75f; // 조절 가능 (0=균등, 1=로그)

    m_fCascadeSplits[0] = fNear;
    m_fCascadeSplits[m_iNumCascades] = fFar;

    for (_uint i = 1; i < m_iNumCascades; ++i)
    {
        _float ratio = static_cast<_float>(i) / m_iNumCascades;

        // 로그 분할
        _float logSplit = fNear * pow(fFar / fNear, ratio);
        // 균등 분할
        _float uniformSplit = fNear + (fFar - fNear) * ratio;
        // 혼합
        m_fCascadeSplits[i] = lambda * logSplit + (1.0f - lambda) * uniformSplit;
    }
}

void CCSMShadow::CalculateFrustumCorners(const _matrix& viewProj, _float nearPlane, _float farPlane, _vector* corners)
{
    _vector frustumCorners[8] = {
        // Near plane
        XMVectorSet(-1.0f, -1.0f, 0.0f, 1.0f),
        XMVectorSet(1.0f, -1.0f, 0.0f, 1.0f),
        XMVectorSet(1.0f,  1.0f, 0.0f, 1.0f),
        XMVectorSet(-1.0f,  1.0f, 0.0f, 1.0f),
        // Far plane
        XMVectorSet(-1.0f, -1.0f, 1.0f, 1.0f),
        XMVectorSet(1.0f, -1.0f, 1.0f, 1.0f),
        XMVectorSet(1.0f,  1.0f, 1.0f, 1.0f),
        XMVectorSet(-1.0f,  1.0f, 1.0f, 1.0f)
    };

    _matrix invViewProj = XMMatrixInverse(nullptr, viewProj);

    for (_int i = 0; i < 8; ++i)
    {
        _vector worldPos = XMVector4Transform(frustumCorners[i], invViewProj);
        corners[i] = worldPos / XMVectorGetW(worldPos);
    }
}

_matrix CCSMShadow::CreateLightViewProj(const _vector* frustumCorners, const _vector& lightDir)
{
    _vector center = XMVectorZero();
    for (_int i = 0; i < 8; ++i)
        center += frustumCorners[i];
    center /= 8.0f;

    _vector lightPos = center - XMVector3Normalize(lightDir) * 100.0f;
    _matrix lightView = XMMatrixLookAtLH(lightPos, center, XMVectorSet(0, 1, 0, 0));

    _vector minExtents = XMVectorSet(FLT_MAX, FLT_MAX, FLT_MAX, 1.0f);
    _vector maxExtents = XMVectorSet(-FLT_MAX, -FLT_MAX, -FLT_MAX, 1.0f);

    for (_int i = 0; i < 8; ++i)
    {
        _vector lightSpacePos = XMVector3Transform(frustumCorners[i], lightView);
        minExtents = XMVectorMin(minExtents, lightSpacePos);
        maxExtents = XMVectorMax(maxExtents, lightSpacePos);
    }

    // Ortho Projection 생성
    _float l = XMVectorGetX(minExtents);
    _float r = XMVectorGetX(maxExtents);
    _float b = XMVectorGetY(minExtents);
    _float t = XMVectorGetY(maxExtents);
    _float n = XMVectorGetZ(minExtents) - 50.0f; // 확장
    _float f = XMVectorGetZ(maxExtents) + 50.0f;

    _matrix lightProj = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);

    return lightView * lightProj;
}

CCSMShadow* CCSMShadow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint shadowSize)
{
	CCSMShadow* Instance = new CCSMShadow(pDevice, pContext);
	if (FAILED(Instance->Initialize(shadowSize))) {
		Safe_Release(Instance);
	}
	return Instance;
}

void CCSMShadow::Free()
{
	__super::Free();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);

    if (m_pShadowMapSRV) m_pShadowMapSRV->Release();
    if (m_pShadowMapArray) m_pShadowMapArray->Release();

    for (UINT i = 0; i < MAX_CASCADES; ++i)
    {
        if (m_pDSV[i]) m_pDSV[i]->Release();
    }
}
