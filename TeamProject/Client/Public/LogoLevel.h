#pragma once
#include "Level.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)
class CCamDirector; class COrbitCam;

class CLogoLevel : public CLevel
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
    CGameInstance* m_pGameInstance{};

public:
    static CLogoLevel* Create(const string& LevelKey);
    virtual void Free() override;
};

NS_END