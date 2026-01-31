#pragma once
#include "BasePanel.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)
class CBattleSystem;
class CMonsterSpawnConsole final : public CBasePanel
{
private:
	CMonsterSpawnConsole(GUI_CONTEXT* pContext);
	virtual ~CMonsterSpawnConsole() = default;

public:
	HRESULT			Initialize();
	virtual void	Update_Panel(_float dt) override;
	virtual void	Render_GUI() override;

private:
	HRESULT			LoadMonsterTableData(const string& csvpath);
	void			CheckCoolTime(const _float dt);
	void			Render_GUI_Contents();
	void			RenderGuiMonsterSelect();
	void			RenderGuiSetValue();
	void			RenderGuiSpawnMonster();

private:
	/* Gui Animation */
	_bool m_isOpen = false;
	_float m_fOpentime = {};

	/* Refernce */
	CGameInstance* m_pGameInstance = { nullptr };
	CBattleSystem* m_pBattleSystem = { nullptr };

	/* Data */
	unordered_map<string, string>	m_MonsterTags;
	vector<string>					m_MonsterKeys;
	string							m_tagSelectedKey = "";
	string							m_tagSelectedProtoTag = "";

	/* Value */
	_float3		m_vSpawnPos = {};

	/* Alarm*/
	_bool		m_isSpawnFailed = { false };
	_float2		m_vSpawnFailedTime = { 3.f, 0.f };

public:
	static CMonsterSpawnConsole* Create(GUI_CONTEXT* pContext);
	virtual void Free() override;
};

NS_END