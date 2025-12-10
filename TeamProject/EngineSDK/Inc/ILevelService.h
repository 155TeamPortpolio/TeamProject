#pragma once
#include "IService.h"
NS_BEGIN(Engine)

class ENGINE_DLL ILevelService :
    public IService
{
protected:
    virtual ~ILevelService() DEFAULT;

public:
    virtual HRESULT Request_ChangeLevel(string levelKey, _bool Load = true) PURE;

    virtual void Update(_float dt)PURE;
    virtual HRESULT Render(ID3D11DeviceContext* pContext)PURE;
    virtual  class CLevel* Get_CurrentLevel()PURE;

    virtual void Register_Level(const string& levelID, LEVEL_CREATOR creator)PURE;
    virtual _uint Get_LevelCount() PURE;
    virtual const vector<string> Get_LevelList() PURE; //레벨 키 모음
    virtual _bool Check_ValidateLevel(const string& LevelTag) PURE;
    virtual const string& Get_NowLevelKey()PURE;
    virtual void Set_LoadingLevel(const string& LoadingKey)PURE;
    virtual const string& Get_NextLevel() PURE;
    virtual void Notify_LoadComplete() PURE;
};

NS_END