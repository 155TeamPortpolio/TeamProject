#pragma once
#include "Level.h"

NS_BEGIN(Engine)
class CGameInstance;
class CUI_Object;
class IProtoService;
class IObjectService;
NS_END

NS_BEGIN(Demo)
class CDemoLevel :
    public CLevel
{
private:
    CDemoLevel(const string& LevelKey);
    virtual ~CDemoLevel() DEFAULT;

public:
    virtual HRESULT Initialize() override;
    virtual HRESULT Awake() override;
    virtual void Update() override;
    virtual HRESULT Render()override;

public:
    static void PreLoad_Level();

private:
    CGameInstance* m_pGameInstance = { nullptr };
public:
    static CDemoLevel* Create(const string& LevelKey);
    virtual void Free() override;
};

NS_END