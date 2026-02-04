#pragma once
#include "Level.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)
class CGacha_Level :
    public CLevel
{
private:
	CGacha_Level(const string& LevelKey);
	virtual ~CGacha_Level() DEFAULT;

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT Awake()      override;
	virtual void    Update()     override;
	virtual HRESULT Render()     override;

private:
	void Ready_Map(const string& LevelTag, const string& AreaTag);
	void Ready_GachaObjects();
	void Update_CamTime();

private:
	CGameInstance* m_pGameInstance = { nullptr };
	class CCamDirector* m_pCamDirector = { nullptr };

private:
	class CGachaProps* m_pGachaProps = nullptr;

private:
	OBJECT_HANDLE			m_GachaHandle{};
	vector<WEAPON_DESC>		m_ResultDesc;
	_int					m_iIndex = -1;
	_int					m_iMaxIndex = 10;

public:
	static CGacha_Level* Create(const string& LevelKey);
	virtual void Free() override;
};

NS_END