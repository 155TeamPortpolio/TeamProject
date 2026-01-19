#pragma once
#include "Level.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)
class CLoadingLevel : 
    public CLevel
{
private:
    CLoadingLevel(const string& LevelKey);
    virtual ~CLoadingLevel() DEFAULT;

public:
    virtual HRESULT Initialize() override;
    virtual HRESULT Awake()      override;
    virtual void    Update()     override;
    virtual HRESULT Render()     override;
    
private:
    CGameInstance* m_pGameInstance{}; 
    _float m_leastTime = { 0.f };
private:
    void PreLoadLevel();
    ResourceType CheckResourceType(const string& fileName);
    _bool isSRGB(const string& filePath);
private:
    string m_NextLevel = {};
    unordered_map<ResourceType, queue<PreloadKey>> m_LoadQue;

public:
    static CLoadingLevel* Create(const string& LevelKey);
    virtual void Free() override;
};

NS_END