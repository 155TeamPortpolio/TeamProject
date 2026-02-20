#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_TutorialGuide final : public CUI_Object
{
private:
	enum class STATE { READY, ACTIVE, DEACTIVATING, INACTIVE, END };

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
	STATE m_eState = { STATE::END };
	TUTORIAL_TYPE m_eType = {}; 

	CUI_Object* m_pGuideStart = {};
	CUI_Object* m_pSlotComplete = {};
	map<TUTORIAL_ACTION, CUI_Object*> m_pSlots;
	map<TUTORIAL_ACTION, _bool> m_slotsProgress;

	_bool m_isFadeout = {};
	_float m_fTimer = {};
	const _float m_fDurationWipeout = { 1.f };// { 4.7f };
	const _float m_fDurationFadeout = { 0.6f };	// 0.6f

private:
	HRESULT Create_GuideStart();
	HRESULT Create_SlotComplete();
	HRESULT Show_ResultBanner();

	void Change_State(STATE eState);
	void Ready_Slots(TUTORIAL_TYPE eType);

	void Activate_Slot(TUTORIAL_ACTION_DESC desc);
	HRESULT Create_Slot(TUTORIAL_ACTION_DESC desc);

	void AdvanceTutorial();

	TUTORIAL_TYPE GetNextTutorialType(TUTORIAL_TYPE eType);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() override;
};

NS_END