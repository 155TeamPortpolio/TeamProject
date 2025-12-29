#pragma once
#include "GameObject.h"
class CTestCloud :
    public CGameObject
{
private:
    CTestCloud();
    CTestCloud(const CTestCloud& rhs);
    virtual ~CTestCloud() DEFAULT;

public:
    void Set_CloudColor(_float3 newCloudColor);  //구름 색 변경
    void Set_SkyColor(_float3 newSkyColor);    //하늘 색 변경
    void Set_CloudInfo(_float3 newSkyColor, _float3 newCloudColor); //둘다 변경

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void Awake() override;
    void Priority_Update(_float dt) override;
    void Update(_float dt) override;
    void Late_Update(_float dt) override;

private:
    _float3 m_vCloudColor = _float3(0.95, 0.95, 0.98);
    _float3 m_vSkyColor =_float3(1.0,0.6,0.8);
    _float m_fAccTime = 0.f;

public:
    static CTestCloud* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free();
};

