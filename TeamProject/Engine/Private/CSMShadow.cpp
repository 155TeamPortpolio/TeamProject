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

    // 그림자 맵 텍스처 배열 생성
    D3D11_TEXTURE2D_DESC TextureDesc = {};
    ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

    TextureDesc.Width = m_iShadowMapSize;
    TextureDesc.Height = m_iShadowMapSize;
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = m_iNumCascades;
    TextureDesc.Format = DXGI_FORMAT_R32_TYPELESS;  // Depth용 typeless
    TextureDesc.SampleDesc.Count = 1;
    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.Usage = D3D11_USAGE_DEFAULT;
    TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

    if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pShadowMapArray)))
        return E_FAIL;

    // 각 cascade별 Depth Stencil View 생성
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
    dsvDesc.Texture2DArray.MipSlice = 0;
    dsvDesc.Texture2DArray.ArraySize = 1;  // 각 DSV는 하나의 배열 슬라이스만

    for (UINT i = 0; i < m_iNumCascades; ++i)
    {
        dsvDesc.Texture2DArray.FirstArraySlice = i;
        if (FAILED(m_pDevice->CreateDepthStencilView(m_pShadowMapArray, &dsvDesc, &m_pDSV[i])))
            return E_FAIL;
    }

    // Shader Resource View 생성 (전체 배열)
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

    srvDesc.Format = DXGI_FORMAT_R32_FLOAT;  // Depth 읽기용
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    srvDesc.Texture2DArray.MipLevels = 1;
    srvDesc.Texture2DArray.MostDetailedMip = 0;
    srvDesc.Texture2DArray.FirstArraySlice = 0;
    srvDesc.Texture2DArray.ArraySize = m_iNumCascades;

    if (FAILED(m_pDevice->CreateShaderResourceView(m_pShadowMapArray, &srvDesc, &m_pShadowMapSRV)))
        return E_FAIL;

    // Shadow용 Comparison Sampler 생성
    D3D11_SAMPLER_DESC samplerDesc = {};
    ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

    samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
    samplerDesc.BorderColor[0] = 1.0f;  // Border 밖은 빛이 닿는 것으로
    samplerDesc.BorderColor[1] = 1.0f;
    samplerDesc.BorderColor[2] = 1.0f;
    samplerDesc.BorderColor[3] = 1.0f;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    if (FAILED(m_pDevice->CreateSamplerState(&samplerDesc, &m_pShadowSampler)))
        return E_FAIL;

    // Viewport 설정
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

    // 메인 카메라의 View/Proj 행렬 가져오기
    _matrix mainCameraView = *CamMgr->Get_ViewMatrix();
    _matrix mainCameraProj = *CamMgr->Get_ProjMatrix();

    // 카메라 렌즈 정보 가져오기
    Lens camLens = CamMgr->Get_Lens();

    // Cascade 분할 계산
    CalculateCascadeSplits(camLens.zNear, 200.f);

    // 라이트 방향 가져오기
    auto shadowTransform = CamMgr->Get_ShadowCam()->Get_Owner()->Get_Component<CTransform>();
    _vector lightDir = XMVector3Normalize(shadowTransform->Dir(STATE::LOOK));

    // 각 Cascade별로 Light View-Projection 행렬 계산
    for (_uint i = 0; i < m_iNumCascades; ++i)
    {
        _vector frustumCorners[8];

        // 1. 메인 카메라의 View Space에서 Frustum Corner 계산
        CalculateFrustumCornersInViewSpace(
            XMConvertToRadians(camLens.fov),
            camLens.aspect,
            m_fCascadeSplits[i],
            m_fCascadeSplits[i + 1],
            frustumCorners
        );

        // 2. World Space로 변환
        _matrix invView = XMMatrixInverse(nullptr, mainCameraView);
        for (int j = 0; j < 8; ++j)
        {
            frustumCorners[j] = XMVector3TransformCoord(frustumCorners[j], invView);
        }

        // 3. Light View-Projection 행렬 생성
        m_lightViewProj[i] = CreateLightViewProj(frustumCorners, lightDir);

        // 디버그 출력
#ifdef _DEBUG
        XMFLOAT4X4 mat;
        XMStoreFloat4x4(&mat, m_lightViewProj[i]);

        char buffer[512];
        sprintf_s(buffer, "=== Cascade %d (Near: %.2f, Far: %.2f) ===\n",
            i, m_fCascadeSplits[i], m_fCascadeSplits[i + 1]);
        OutputDebugStringA(buffer);

        sprintf_s(buffer, "  [%.4f, %.4f, %.4f, %.4f]\n", mat._11, mat._12, mat._13, mat._14);
        OutputDebugStringA(buffer);
        sprintf_s(buffer, "  [%.4f, %.4f, %.4f, %.4f]\n", mat._21, mat._22, mat._23, mat._24);
        OutputDebugStringA(buffer);
        sprintf_s(buffer, "  [%.4f, %.4f, %.4f, %.4f]\n", mat._31, mat._32, mat._33, mat._34);
        OutputDebugStringA(buffer);
        sprintf_s(buffer, "  [%.4f, %.4f, %.4f, %.4f]\n\n", mat._41, mat._42, mat._43, mat._44);
        OutputDebugStringA(buffer);
#endif
    }
}

void CCSMShadow::Begin_ShadowRender(_uint cascadeIndex)
{
    // 이전 렌더 타겟과 Depth Stencil 저장
    _uint numRTVs = 1;
    m_pContext->OMGetRenderTargets(numRTVs, &m_pPrevRTV, &m_pPrevDSV);

    // Depth 버퍼 클리어
    m_pContext->ClearDepthStencilView(m_pDSV[cascadeIndex], D3D11_CLEAR_DEPTH, 1.0f, 0);

    // 그림자 맵 렌더링 (색상 출력 없음, Depth만)
    ID3D11RenderTargetView* nullRTV = nullptr;
    m_pContext->OMSetRenderTargets(1, &nullRTV, m_pDSV[cascadeIndex]);
    m_pContext->RSSetViewports(1, &m_viewPort);
}

void CCSMShadow::End_ShadowRender()
{
    // 이전 렌더 타겟 복구
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
    // Practical Split Scheme (Practical CSM 논문)
    // lambda: 0에 가까울수록 균등 분할, 1에 가까울수록 로그 분할
    _float lambda = 0.75f;

    m_fCascadeSplits[0] = fNear;
    m_fCascadeSplits[m_iNumCascades] = fFar;

    for (_uint i = 1; i < m_iNumCascades; ++i)
    {
        _float ratio = static_cast<_float>(i) / static_cast<_float>(m_iNumCascades);

        // 로그 분할: 가까운 곳에 더 많은 디테일
        _float logSplit = fNear * pow(fFar / fNear, ratio);

        // 균등 분할: 동일한 간격
        _float uniformSplit = fNear + (fFar - fNear) * ratio;

        // 두 방식을 lambda로 혼합
        m_fCascadeSplits[i] = lambda * logSplit + (1.0f - lambda) * uniformSplit;
    }

#ifdef _DEBUG
    char buffer[256];
    sprintf_s(buffer, "Cascade Splits: ");
    OutputDebugStringA(buffer);
    for (_uint i = 0; i <= m_iNumCascades; ++i)
    {
        sprintf_s(buffer, "%.2f ", m_fCascadeSplits[i]);
        OutputDebugStringA(buffer);
    }
    OutputDebugStringA("\n");
#endif
}

void CCSMShadow::CalculateFrustumCornersInViewSpace(
    float fovY,
    float aspect,
    float nearPlane,
    float farPlane,
    _vector* corners)
{
    // FOV와 Aspect Ratio를 사용해 Frustum의 너비/높이 계산
    float tanHalfFovY = tan(fovY * 0.5f);

    // Near Plane의 크기
    float nearHeight = nearPlane * tanHalfFovY;
    float nearWidth = nearHeight * aspect;

    // Far Plane의 크기
    float farHeight = farPlane * tanHalfFovY;
    float farWidth = farHeight * aspect;

    // View Space에서 Frustum Corner 계산
    // View Space: +X = 오른쪽, +Y = 위, -Z = 앞(카메라가 보는 방향)

    // Near Plane의 4개 코너
    corners[0] = XMVectorSet(-nearWidth, -nearHeight, nearPlane, 1.0f);  // 좌하
    corners[1] = XMVectorSet(nearWidth, -nearHeight, nearPlane, 1.0f);  // 우하
    corners[2] = XMVectorSet(nearWidth, nearHeight, nearPlane, 1.0f);  // 우상
    corners[3] = XMVectorSet(-nearWidth, nearHeight, nearPlane, 1.0f);  // 좌상

    // Far Plane의 4개 코너
    corners[4] = XMVectorSet(-farWidth, -farHeight, farPlane, 1.0f);  // 좌하
    corners[5] = XMVectorSet(farWidth, -farHeight, farPlane, 1.0f);  // 우하
    corners[6] = XMVectorSet(farWidth, farHeight, farPlane, 1.0f);  // 우상
    corners[7] = XMVectorSet(-farWidth, farHeight, farPlane, 1.0f);  // 좌상
}
_matrix CCSMShadow::CreateLightViewProj(const _vector* frustumCorners, const _vector& lightDir)
{
    auto CamMgr = CGameInstance::GetInstance()->Get_CameraMgr();
    _matrix lightView = *CamMgr->Get_ShadowViewMatrix();

    // 1. 프러스텀 코너들의 중점 계산 (월드 스페이스)
    _vector frustumCenter = XMVectorZero();
    for (_int i = 0; i < 8; ++i)
    {
        frustumCenter = XMVectorAdd(frustumCenter, frustumCorners[i]);
    }
    frustumCenter = XMVectorScale(frustumCenter, 1.0f / 8.0f);

    // 2. 바운딩 스피어의 반지름 계산
    _float sphereRadius = 0.0f;
    for (_int i = 0; i < 8; ++i)
    {
        _vector diff = XMVectorSubtract(frustumCorners[i], frustumCenter);
        _float dist = XMVectorGetX(XMVector3Length(diff));
        sphereRadius = max(sphereRadius, dist);
    }

    // 3. 반지름을 텍셀 단위로 스냅 (안정화)
    sphereRadius = ceil(sphereRadius * 16.0f) / 16.0f;

    // 4. 중점을 라이트 스페이스로 변환
    _vector centerLightSpace = XMVector3TransformCoord(frustumCenter, lightView);

    // 5. 라이트 스페이스에서 sphere 기준으로 AABB 생성
    _float minX = XMVectorGetX(centerLightSpace) - sphereRadius;
    _float maxX = XMVectorGetX(centerLightSpace) + sphereRadius;
    _float minY = XMVectorGetY(centerLightSpace) - sphereRadius;
    _float maxY = XMVectorGetY(centerLightSpace) + sphereRadius;

    // 6. Z 범위는 모든 코너를 체크해서 구함
    _float minZ = FLT_MAX;
    _float maxZ = -FLT_MAX;
    for (_int i = 0; i < 8; ++i)
    {
        _vector lightSpacePos = XMVector3TransformCoord(frustumCorners[i], lightView);
        _float z = XMVectorGetZ(lightSpacePos);
        minZ = min(minZ, z);
        maxZ = max(maxZ, z);
    }

#ifdef _DEBUG
    char buffer[512];
    sprintf_s(buffer, "Before Snap: X[%.4f, %.4f], Y[%.4f, %.4f], Radius: %.4f\n",
        minX, maxX, minY, maxY, sphereRadius);
    OutputDebugStringA(buffer);
#endif

    // 7. Z 범위 확장 (그림자 잘림 방지)
    _float zRange = maxZ - minZ;
    _float zExtension = max(zRange * 1.0f, 50.0f);
    minZ = minZ - zExtension;
    maxZ = maxZ + zExtension;

    // 8. 텍셀 스내핑 (shimmer 방지)
    _float worldUnitsPerTexel = (maxX - minX) / static_cast<_float>(m_iShadowMapSize);

#ifdef _DEBUG
    _float originalMinX = minX;
    _float originalMaxX = maxX;
#endif

    minX = floor(minX / worldUnitsPerTexel) * worldUnitsPerTexel;
    maxX = ceil(maxX / worldUnitsPerTexel) * worldUnitsPerTexel;
    minY = floor(minY / worldUnitsPerTexel) * worldUnitsPerTexel;
    maxY = ceil(maxY / worldUnitsPerTexel) * worldUnitsPerTexel;

#ifdef _DEBUG
    sprintf_s(buffer, "After Snap:  X[%.4f, %.4f], Y[%.4f, %.4f] (texel: %.4f)\n",
        minX, maxX, minY, maxY, worldUnitsPerTexel);
    OutputDebugStringA(buffer);

    sprintf_s(buffer, "Delta: dX=%.4f, dY=%.4f\n\n",
        (maxX - minX) - (originalMaxX - originalMinX),
        (maxY - minY) - (originalMaxX - originalMinX));
    OutputDebugStringA(buffer);
#endif

    _matrix lightProj = XMMatrixOrthographicOffCenterLH(minX, maxX, minY, maxY, minZ, maxZ);
    return lightView * lightProj;
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