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
    static const vector<const _char*> Get_TextureKeys();
    static _int Get_TextureKeysSize() { return static_cast<_int>(m_TextureKeys.size()); }

private:
    CGameInstance* m_pGameInstance = { nullptr };

private:
    static vector<string> m_TextureKeys;

private:
    HRESULT Ready_Textures();
    HRESULT Ready_Camera();
    HRESULT Ready_UIObjects();
    HRESULT Ready_GUIPanel();

private:
    HRESULT Add_Texture(const string& resourceKey, const string& resourcePath);

public:
    static CUITool_Level* Create(const string& LevelKey);
    virtual void Free() override;
};

NS_END