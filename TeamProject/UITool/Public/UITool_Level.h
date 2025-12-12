#pragma once
#include "Level.h"

NS_BEGIN(Engine)
class CGameInstance;
class CUI_Object;
class IProtoService;
class IObjectService;
class IUI_Service;
NS_END

NS_BEGIN(UITool)
class CUITool_Level final :  public CLevel
{
private:
    CUITool_Level(const string& LevelKey);
    virtual ~CUITool_Level() DEFAULT;

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
    static CUITool_Level* Create(const string& LevelKey);
    virtual void Free() override;
};

NS_END