#pragma once
#include "Level.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)
class CLogoLevel : 
    public CLevel
{
private:
    CLogoLevel(const string& LevelKey);
    virtual ~CLogoLevel() DEFAULT;

public:
    virtual HRESULT Initialize() override;
    virtual HRESULT Awake()      override;
    virtual void    Update()     override;
    virtual HRESULT Render()     override;

private:
    HRESULT Create_LogoVideo();
    void PreLoadLeveleff(const string& levelKey);
    void PreLoadLevel(const string& levelKey);
    ResourceType CheckResourceType(const string& filePath, const string& fileName);
    _bool isSRGB(const string& filePath);
    _bool isEffect(const string& filePath);
    _bool isAnim(const string& filePath);
private:
    CGameInstance* m_pGameInstance{};
    string m_NextLevel = {};
    unordered_map<ResourceType, queue<PreloadKey>> m_LoadQue;
public:
    static CLogoLevel* Create(const string& LevelKey);
    virtual void Free() override;
};

NS_END