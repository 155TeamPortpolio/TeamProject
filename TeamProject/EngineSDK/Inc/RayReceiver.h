#pragma once
#include "Component.h"
NS_BEGIN(Engine)
class ENGINE_DLL CRayReceiver :
    public CComponent
{
private:
    CRayReceiver();
    CRayReceiver(const CRayReceiver& rhs);
    virtual ~CRayReceiver() DEFAULT;
public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(COMPONENT_DESC* pArg) override;
public:
    _bool OnRayHit(RAY* ray, RAY_HIT* HittedInfo);
    _float3 Get_RayHittedPos(_bool* isHit);
    void Set_ReturnType(_bool ReturnToManager) { m_ReturnToManager = ReturnToManager; }

private:
    void Check_VaildComponent();
private:
    _int m_ID = {-1};
    _bool m_bIsHit = {false};
    _bool m_ReturnToManager = {true};
    RAY_HIT m_tHitInfo = {};

    class CTransform* m_pTransform = { nullptr };
    class CModel* m_pModel = { nullptr };

public:
    static CRayReceiver* Create();
    virtual CComponent* Clone() override;
    virtual void Free() override;
};

NS_END