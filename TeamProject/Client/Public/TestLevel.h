#pragma once
#include "Level.h"
NS_BEGIN(Engine)
class CGameInstance; class CUI_Object; class IProtoService; class IObjectService;
NS_END

NS_BEGIN(Client)
class CCamDirector; class COrbitCam;

class CTestLevel : public CLevel
{
private:
    CTestLevel(const string& LevelKey);
    virtual ~CTestLevel() DEFAULT;

public:
    virtual HRESULT Initialize() override;
    virtual HRESULT Awake()      override;
    virtual void    Update()     override;
    virtual HRESULT Render()     override;

private:
    void Ready_Camera();
    void Rake_MapResources();


public:
    static void PreLoad_Level();

private:
    CGameInstance* m_pGameInstance{};
    CCamDirector*  m_pCamDirector{};
    _uint          m_sequenceHandle = 0u;

    // 임시) 쓰레드풀 작업 후 개선 및 위치 이동예정 - 경인
    class CMapDataCloud* m_pMapDataCloud = { nullptr };

public:
    static CTestLevel* Create(const string& LevelKey);
    virtual void Free() override;
};

NS_END