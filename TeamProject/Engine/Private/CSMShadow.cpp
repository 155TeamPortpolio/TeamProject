#include "Engine_Defines.h"
#include "CSMShadow.h"

#include "CameraMgr.h"
#include "Transform.h"
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
    TextureDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    TextureDesc.SampleDesc.Count = 1;
    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.Usage = D3D11_USAGE_DEFAULT;
    TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

    if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pShadowMapArray)))
        return E_FAIL;

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
    dsvDesc.Texture2DArray.MipSlice = 0;
    dsvDesc.Texture2DArray.ArraySize = 1;

    for (UINT i = 0; i < m_iNumCascades; ++i)
    {
        dsvDesc.Texture2DArray.FirstArraySlice = i;
        if (FAILED(m_pDevice->CreateDepthStencilView(m_pShadowMapArray, &dsvDesc, &m_pDSV[i])))
            return E_FAIL;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

    srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    srvDesc.Texture2DArray.MipLevels = 1;
    srvDesc.Texture2DArray.MostDetailedMip = 0;
    srvDesc.Texture2DArray.FirstArraySlice = 0;
    srvDesc.Texture2DArray.ArraySize = m_iNumCascades;

    if (FAILED(m_pDevice->CreateShaderResourceView(m_pShadowMapArray, &srvDesc, &m_pShadowMapSRV)))
        return E_FAIL;

    D3D11_SAMPLER_DESC samplerDesc = {};
    ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

    samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
    samplerDesc.BorderColor[0] = 1.0f;
    samplerDesc.BorderColor[1] = 1.0f;
    samplerDesc.BorderColor[2] = 1.0f;
    samplerDesc.BorderColor[3] = 1.0f;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    if (FAILED(m_pDevice->CreateSamplerState(&samplerDesc, &m_pShadowSampler)))
        return E_FAIL;

    m_viewPort.TopLeftX = 0.0f;
    m_viewPort.TopLeftY = 0.0f;
    m_viewPort.Width = static_cast<_float>(m_iShadowMapSize);
    m_viewPort.Height = static_cast<_float>(m_iShadowMapSize);
    m_viewPort.MinDepth = 0.0f;
    m_viewPort.MaxDepth = 1.0f;

    return S_OK;
}

void CCSMShadow::BindSampler(ID3D11DeviceContext* pContext, _uint slot)
{
    if (m_pShadowSampler)
    {
        pContext->PSSetSamplers(slot, 1, &m_pShadowSampler);
    }
}

void CCSMShadow::Update()
{
    auto CamMgr = CGameInstance::GetInstance()->Get_CameraMgr();
    if (!CamMgr->Get_ShadowCam())
        return;

    _matrix mainCameraView = *CamMgr->Get_ViewMatrix();
    _matrix mainCameraProj = *CamMgr->Get_ProjMatrix();

    Lens camLens = CamMgr->Get_Lens();

    CalculateCascadeSplits(camLens.zNear, camLens.zFar);

    auto shadowTransform = CamMgr->Get_ShadowCam()->Get_Owner()->Get_Component<CTransform>();
    _vector lightDir = XMVector3Normalize(shadowTransform->Dir(STATE::LOOK));

    for (_uint i = 0; i < m_iNumCascades; ++i)
    {
        _vector frustumCorners[8];

        CalculateFrustumCornersInViewSpace(
            XMConvertToRadians(camLens.fov),
            camLens.aspect,
            m_fCascadeSplits[i],
            m_fCascadeSplits[i + 1],
            frustumCorners
        );

        _matrix invView = XMMatrixInverse(nullptr, mainCameraView);
        for (int j = 0; j < 8; ++j)
        {
            frustumCorners[j] = XMVector3TransformCoord(frustumCorners[j], invView);
        }

        m_lightViewProj[i] = CreateLightViewProj(frustumCorners, lightDir, i);
    }
}

void CCSMShadow::Begin_ShadowRender(_uint cascadeIndex)
{
    _uint numRTVs = 1;
    m_pContext->OMGetRenderTargets(numRTVs, &m_pPrevRTV, &m_pPrevDSV);

    m_pContext->ClearDepthStencilView(m_pDSV[cascadeIndex], D3D11_CLEAR_DEPTH, 1.0f, 0);

    ID3D11RenderTargetView* nullRTV = nullptr;
    m_pContext->OMSetRenderTargets(1, &nullRTV, m_pDSV[cascadeIndex]);
    m_pContext->RSSetViewports(1, &m_viewPort);
}

void CCSMShadow::End_ShadowRender()
{
    m_pContext->OMSetRenderTargets(1, &m_pPrevRTV, m_pPrevDSV);

    Safe_Release(m_pPrevRTV);
    Safe_Release(m_pPrevDSV);
}

const _matrix& CCSMShadow::GetLightViewProj(UINT cascadeIndex) const
{
    return m_lightViewProj[cascadeIndex];
}

void CCSMShadow::CalculateCascadeSplits(_float fNear, _float fFar)
{
    _float lambda = 0.75f;

    m_fCascadeSplits[0] = fNear;
    m_fCascadeSplits[m_iNumCascades] = fFar;

    for (_uint i = 1; i < m_iNumCascades; ++i)
    {
        _float ratio = static_cast<_float>(i) / static_cast<_float>(m_iNumCascades);

        _float logSplit = fNear * pow(fFar / fNear, ratio);

        _float uniformSplit = fNear + (fFar - fNear) * ratio;

        m_fCascadeSplits[i] = lambda * logSplit + (1.0f - lambda) * uniformSplit;
    }
}

void CCSMShadow::CalculateFrustumCornersInViewSpace(_float fovY, _float aspect, _float nearPlane, _float farPlane, _vector* corners)
{
    float tanHalfFovY = tan(fovY * 0.5f);

    float nearHeight = nearPlane * tanHalfFovY;
    float nearWidth = nearHeight * aspect;

    float farHeight = farPlane * tanHalfFovY;
    float farWidth = farHeight * aspect;

    corners[0] = XMVectorSet(-nearWidth, -nearHeight, nearPlane, 1.0f);
    corners[1] = XMVectorSet(nearWidth, -nearHeight, nearPlane, 1.0f);
    corners[2] = XMVectorSet(nearWidth, nearHeight, nearPlane, 1.0f);
    corners[3] = XMVectorSet(-nearWidth, nearHeight, nearPlane, 1.0f);

    corners[4] = XMVectorSet(-farWidth, -farHeight, farPlane, 1.0f);
    corners[5] = XMVectorSet(farWidth, -farHeight, farPlane, 1.0f);
    corners[6] = XMVectorSet(farWidth, farHeight, farPlane, 1.0f);
    corners[7] = XMVectorSet(-farWidth, farHeight, farPlane, 1.0f);
}

_matrix CCSMShadow::CreateLightViewProj(const _vector* frustumCorners, const _vector& lightDir, _uint cascadeIndex)
{
    _vector center = XMVectorZero();
    for (_int i = 0; i < 8; ++i)
    {
        center = XMVectorAdd(center, frustumCorners[i]);
    }
    center = XMVectorScale(center, 1.0f / 8.0f);

    _vector up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    _float dotProduct = abs(XMVectorGetX(XMVector3Dot(lightDir, up)));
    if (dotProduct > 0.99f)
    {
        up = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    }

    _vector lightPos = XMVectorSubtract(center, XMVectorScale(lightDir, 100.0f));
    _matrix lightView = XMMatrixLookAtLH(lightPos, center, up);

    _float minX = FLT_MAX, maxX = -FLT_MAX;
    _float minY = FLT_MAX, maxY = -FLT_MAX;
    _float minZ = FLT_MAX, maxZ = -FLT_MAX;

    for (_int i = 0; i < 8; ++i)
    {
        _vector lightSpacePos = XMVector3TransformCoord(frustumCorners[i], lightView);

        _float x = XMVectorGetX(lightSpacePos);
        _float y = XMVectorGetY(lightSpacePos);
        _float z = XMVectorGetZ(lightSpacePos);

        minX = min(minX, x);
        maxX = max(maxX, x);
        minY = min(minY, y);
        maxY = max(maxY, y);
        minZ = min(minZ, z);
        maxZ = max(maxZ, z);
    }

    _float worldUnitsPerTexel = (maxX - minX) / static_cast<_float>(m_iShadowMapSize);

    minX = floor(minX / worldUnitsPerTexel) * worldUnitsPerTexel;
    maxX = ceil(maxX / worldUnitsPerTexel) * worldUnitsPerTexel;
    minY = floor(minY / worldUnitsPerTexel) * worldUnitsPerTexel;
    maxY = ceil(maxY / worldUnitsPerTexel) * worldUnitsPerTexel;

    _float zRange = maxZ - minZ;
    _float nearMargin = max(zRange * 2.f, 100.f);
    _float farMargin = zRange * 0.2f;   

    minZ = max(minZ - nearMargin, 0.1f); 
    maxZ = maxZ + farMargin;

    _matrix lightProj = XMMatrixOrthographicOffCenterLH(minX, maxX,  minY, maxY, minZ, maxZ);

    _matrix result = lightView * lightProj;

    return result;
}

CCSMShadow* CCSMShadow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint shadowSize)
{
    CCSMShadow* Instance = new CCSMShadow(pDevice, pContext);

    if (FAILED(Instance->Initialize(shadowSize)))
    {
        MSG_BOX("Failed to Create CSMShadow");
        Safe_Release(Instance);
        return nullptr;
    }

    return Instance;
}

void CCSMShadow::Free()
{
    __super::Free();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);

    Safe_Release(m_pShadowMapSRV);
    Safe_Release(m_pShadowMapArray);
    Safe_Release(m_pShadowSampler);

    for (_uint i = 0; i < MAX_CASCADES; ++i)
    {
        Safe_Release(m_pDSV[i]);
    }
}