#pragma once
#include "UI_Object.h"

NS_BEGIN(Engine)
class CTextSlot;
NS_END

NS_BEGIN(Client)

class CUI_Switch final : public CUI_Object
{
public:
	typedef struct tagSwitchDesc {
		CHARACTER left;
		CHARACTER right;
	}SWITCH_DESC;

private:
	enum class STATE { ACTIVE, DEACTIVATING, END };
	enum class ROLE { LEFT, RIGHT, END };
	
private:
	CUI_Switch() {}
	CUI_Switch(const CUI_Switch& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_Switch() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }
	virtual void	UI_Active(void* pArg = nullptr)  override;
	virtual void	UI_DeActive(void* pArg = nullptr) override;

private:
	class CUI_SwitchGauge* m_pGauge = { nullptr };
	class CUI_SwitchRole* m_pRoles[ENUM(ROLE::END)] = { nullptr };
	class CTextSlot* m_pTimerText = {};
	
	STATE m_eState = { STATE::END };

	CHARACTER m_characters[ENUM(ROLE::END)] = {};

	_float m_fTimer = {};
	const float m_fDuration = { 3.f };

private:
	void Cache();
	void Create_Gauge();
	void Create_Roles();

	void Change_State(STATE eState);
	void Update_Timer(_float dt);

	void Update_TimerText();

	void Change_RoleIcon(ROLE eRole, CHARACTER eCharacter);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END