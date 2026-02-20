#pragma once
#include "UI_Object.h"

NS_BEGIN(Engine)
class CTextSlot;
NS_END

NS_BEGIN(Client)

class CUI_TutorialGuideSlot final : public CUI_Object
{
public:
	typedef struct tagSlotDesc : public UI_DESC {
		TUTORIAL_ACTION_DESC desc = {};
	}SLOT_DESC;

private:
	CUI_TutorialGuideSlot() {}
	CUI_TutorialGuideSlot(const CUI_TutorialGuideSlot& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_TutorialGuideSlot() DEFAULT;

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
	TUTORIAL_ACTION m_eAction = {};
	class CTextSlot* m_pCountText = {};

	_uint m_iCurrentCount = {};
	_uint m_iTargetCount = {};

private:
	void Cache();

	void Set_CountText();
	void Set_Animations(_int iIndex = 0);

	string Get_PrefabPath(TUTORIAL_ACTION eAction);
	
public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() override;
};

NS_END