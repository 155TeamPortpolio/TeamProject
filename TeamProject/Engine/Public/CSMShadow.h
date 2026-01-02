#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CCSMShadow :
    public CBase
{
private:
    CCSMShadow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    ~CCSMShadow() DEFAULT;

public:
    HRESULT Initialize(_uint shadowSize);
    void Update();

    void Begin_ShadowRender(_uint cascadeIndex);
    void End_ShadowRender();
    
public:
    ID3D11ShaderResourceView* GetShadowMapSRV() const { return m_pShadowMapSRV; }
    const _matrix& GetLightViewProj(UINT cascadeIndex) const;
    const _float* GetCascadeSplits() const { return m_fCascadeSplits; }
    UINT GetCascadeCount() const { return m_iNumCascades; }

private:
    void CalculateCascadeSplits(_float fNear, _float fFar);
    void CalculateFrustumCorners(const _matrix& viewProj, _float nearPlane, _float farPlane, _vector* corners);
    _matrix CreateLightViewProj(const _vector* frustumCorners, const _vector& lightDir);


private:
    static const _uint MAX_CASCADES = 4;

    _uint m_iNumCascades = 4;
    _uint m_iShadowMapSize;
    _float m_fCascadeSplits[MAX_CASCADES + 1];

    ID3D11Texture2D* m_pShadowMapArray = nullptr;
    ID3D11DepthStencilView* m_pDSV[MAX_CASCADES] = { nullptr };
    ID3D11ShaderResourceView* m_pShadowMapSRV = nullptr;

    _matrix m_lightViewProj[MAX_CASCADES];

    ID3D11Device* m_pDevice;
    ID3D11DeviceContext* m_pContext;

    D3D11_VIEWPORT m_viewPort = {};

public:
    static CCSMShadow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint shadowSize);
    void Free() override;
};

NS_END