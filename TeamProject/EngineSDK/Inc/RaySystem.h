#pragma once
#include "IRayService.h"
NS_BEGIN(Engine)
class CRaySystem :
    public IRayService
{
private:
    CRaySystem();
   virtual ~CRaySystem();

public:
    HRESULT Initialize();
    virtual _int Register_RayReceiver(class CRayReceiver* RayTarget) override;
    virtual void UnRegister_RayReceiver(_int ID) override;

public:
    virtual void Register_Ray(RAY* ray) override;
    virtual void Delete_Ray() override;
    virtual RAY_HIT* Get_FrontRayHit() override;
public:
    virtual void Update(_float dt) override;
    class CGameObject* Get_HittedObject();

private:
    RAY* m_pRay = {};
    vector<_bool> m_ReceiversLifes;
    vector<class CRayReceiver*> m_RayReceivers;
    vector<RAY_HIT> m_HittedTargets;

public:
    static CRaySystem* Create();
    virtual void Free() override;
};

NS_END