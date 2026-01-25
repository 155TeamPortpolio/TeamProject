#pragma once
#include "Base.h"

#include "CursorController.h"

namespace Engine {
	class CGameInstance;
}

class CMainApp :public CBase
{
private:
	explicit CMainApp();
	virtual ~CMainApp();

public:
	HRESULT Initialize();
	void Update(const float dt);
	HRESULT Render();

public:
	void Set_Levels();
	
private:
	CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = {nullptr};

	CCursorController m_cursorController;

public:
	static CMainApp* Create();
	virtual void Free() override;

private:
	void Initialize_GlobalPrototype();
	void Create_GlobalCamObjs();
	void Create_GlobalPlayer();
	void Create_GlobalEnviroment();
};

