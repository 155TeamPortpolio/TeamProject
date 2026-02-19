#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_TutorialGuide final : public CUI_Object
{
private:
	enum class STATE { ACTIVE, DEACTIVATING, INACTIVE, END };

private:
	CUI_TutorialGuide() {}
	CUI_TutorialGuide(const CUI_TutorialGuide& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_TutorialGuide() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }
	virtual void	UI_Active(void* pArg = nullptr) override;
	virtual void	UI_DeActive(void* pArg = nullptr) override;

private:
	TUTORIAL_TYPE m_eType = {};
	STATE m_eState = { STATE::END };

	vector<CUI_Object*> m_pSlots;

private:
	HRESULT Create_Slot();

	void Change_State(STATE eState);
	void Ready_Slots(TUTORIAL_TYPE eType);

	void AdvanceTutorial();

	TUTORIAL_TYPE GetNextTutorialType(TUTORIAL_TYPE eType);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() override;
};

NS_END