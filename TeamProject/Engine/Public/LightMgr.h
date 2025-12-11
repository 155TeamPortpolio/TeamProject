#pragma once
#include "ILightService.h"
NS_BEGIN(Engine)
class CLightMgr :
    public ILightService
{
private:
    CLightMgr();
    virtual ~CLightMgr();

public:
    HRESULT Initialize();
    virtual _int Register_Light(class CLight* Light) override;
    virtual void UnRegister_Light(_int ID) override;

public:
    virtual vector<class CLight*>& Get_VisibleLight() override  { return m_Lights; } ; /*나중에 컬링해서 보내기*/

private:
    vector<_bool> m_LightLifes;
    vector<class CLight*> m_Lights;

public:
    static CLightMgr* Create();
    virtual void Free() override;
};

NS_END