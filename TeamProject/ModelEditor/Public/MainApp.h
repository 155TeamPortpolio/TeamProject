#pragma once
#include "Base.h"


NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(ModelEdit)
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
	void ReadyBase();
private:
	CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = {nullptr};
	class CEditorSystem* m_pSystem = { nullptr };

public:
	static CMainApp* Create(); 
	virtual void Free() override;
};

NS_END