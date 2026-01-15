#pragma once
#include "Level.h"

NS_BEGIN(Engine)
class CGameInstance;
class CUI_Object;
class IProtoService;
class IObjectService;
NS_END

NS_BEGIN(Client)
class CScott_Level : public CLevel
{
private:
	CScott_Level(const string& LevelKey);
	virtual ~CScott_Level() DEFAULT;

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT Awake()      override;
	virtual void    Update()     override;
	virtual HRESULT Render()     override;

public:
	static void PreLoad_Level();

private:
	void Ready_Map(const string& LevelTag, const string& AreaTag);
	void Rake_MapResources();
	void Ready_Camera();
	void Ready_ShadowCamera();

private:
	class CGameInstance* m_pGameInstance{};
	class CCamDirector* m_pCamDirector{};

public:
	static CScott_Level* Create(const string& LevelKey);
	virtual void Free() override;
};

NS_END