#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)
class CTexture;
NS_END

NS_BEGIN(Client)

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
    _float3 m_vSkyColor = _float3(0.47f, 0.78f, 0.94f);    
    _float3 m_vCloudColor = _float3(1.0f, 1.0f, 1.0f);
    _float m_fAccTime = 0.f;

    //============ShaderTest================
    //vector<CTexture*> NoiseTextures;

public:
    static CTestCloud* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free();
};

NS_END