#pragma once
#include "Level.h"

NS_BEGIN(Engine)
class CGameInstance; 
NS_END

NS_BEGIN(Client)

class CZero_Level : public CLevel
{
private:
	CZero_Level(const string& LevelKey);
	virtual ~CZero_Level() DEFAULT;

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT Awake()      override;
	virtual void    Update()     override;
	virtual HRESULT Render()     override;

public:
	static void PreLoad_Level();

private:
	CGameInstance* m_pGameInstance{};

public:
	static CZero_Level* Create(const string& LevelKey);
	virtual void Free() override;
};

NS_END