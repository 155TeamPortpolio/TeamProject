#pragma once
#include "Level.h"

NS_BEGIN(Engine)
class CGameInstance;
class CAudioSource;
NS_END

NS_BEGIN(Client)

class CTutorial_Level :
    public CLevel
{
private:
	CTutorial_Level(const string& LevelKey);
	virtual ~CTutorial_Level() DEFAULT;

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT Awake()      override;
	virtual void    Update()     override;
	virtual HRESULT Render()     override;

private:
	void Ready_Map(const string& LevelTag, const string& AreaTag);
	void Ready_UI();

private:
	CGameInstance* m_pGameInstance = { nullptr };
	class CPlayer* m_pPlayer = { nullptr };
	CAudioSource* m_pBGM = {};

public:
	static CTutorial_Level* Create(const string& LevelKey);
	virtual void Free() override;
};

NS_END