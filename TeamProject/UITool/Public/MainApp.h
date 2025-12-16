#pragma once
#include "Base.h"

namespace Engine 
{
	class CGameInstance;
}

class CMainApp final :public CBase
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
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };

public:
	static vector<string>			m_strLevelTags;
	static vector<const _char*>		m_szLevelTags;

public:
	static CMainApp* Create();
	virtual void Free() override;
};