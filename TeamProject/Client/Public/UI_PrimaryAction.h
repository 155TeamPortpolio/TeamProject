#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_PrimaryAction final : public CUI_Object
{
private: 
	enum class CHILD { ATTACK, ATTACK_BG, ATTACK_ICON, ATTACK_MOUSE, INTERACT, INTERACT_GRADIENT, END };
	inline static const string INSTANCENAMES[ENUM(CHILD::END)] = { "attack", "attackBg", "attackIcon", "attackMouse", "interact", "interactGradient" };

	enum class MODE { ATTACK, INTERACT };
	enum class INTERACT_STATE { DISABLE, ENABLE, AVAILABLE };

private:
	CUI_PrimaryAction() {}
	CUI_PrimaryAction(const CUI_PrimaryAction& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_PrimaryAction() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }
	virtual void	UI_Active(void* pArg = nullptr)  override;
	virtual void	UI_DeActive(void* pArg = nullptr)override;

private:
	CUI_Object*		m_pChildren[ENUM(CHILD::END)] = {};

	MODE			m_mode = { MODE::ATTACK };
	INTERACT_STATE	m_interactState = { INTERACT_STATE::ENABLE };

private:
	void Load_Json(const string& resourceKey);
	void Cache_Children();
	void Bind_EventListener();

	void Set_ActionMode(MODE eMode);
	void Set_InteractState(INTERACT_STATE state);
	 
	void Refresh_Visual();

	void Apply_DisableVisual();
	void Apply_EnableVisual();
	void Apply_AvailableVisual();

	void Set_ChildAlive(CHILD child, _bool isAlive);
	void Set_ChildColor(CHILD child, _float4 vColor);
	void Set_ChildAnimation(CHILD child, _int iIndex);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END