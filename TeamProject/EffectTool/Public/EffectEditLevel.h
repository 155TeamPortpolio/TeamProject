#pragma once
#include "Level.h"

NS_BEGIN(Engine)
class CGameInstance;
class CUI_Object;
class IProtoService;
class IObjectService;
NS_END

NS_BEGIN(EffectTool)
class CEffectEditLevel :
    public CLevel
{
private:
    CEffectEditLevel(const string& LevelKey);
    virtual ~CEffectEditLevel() DEFAULT;

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
    static CEffectEditLevel* Create(const string& LevelKey);
    virtual void Free() override;
};

NS_END