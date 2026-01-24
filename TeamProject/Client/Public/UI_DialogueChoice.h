#pragma once
#include "UI_Object.h"

NS_BEGIN(Engine)
class CTextSlot;
NS_END

NS_BEGIN(Client)

class CUI_DialogueChoice final : public CUI_Object
{
public:
	typedef struct tagChoiceDesc {
		_int iChoiceNum = {};
		vector<string> strChoices = {};
	}CHOICE_DESC;

private:
	enum CHILD { CHOICE1, CHOICE2, CHOICE3, END };
	inline static const string CHILD_INSTNAMES[ENUM(CHILD::END)] = { "choice1", "choice2", "choice3" };

	enum class TEXTSLOT { MESSAGE1, MESSAGE2, MESSAGE3, END };
	inline static const string TEXTSLOT_INSTNAMES[ENUM(TEXTSLOT::END)] = { "message1", "message2", "message3" };

	enum class BTNS { BTN1, BTN2, BTN3, END };
	inline static const string BTN_INSTNAMES[ENUM(BTNS::END)] = { "btn1", "btn2", "btn3" };

	inline static constexpr CHILD CHOICES[] = { CHOICE1, CHOICE2, CHOICE3 };

private:
	CUI_DialogueChoice() {}
	CUI_DialogueChoice(const CUI_DialogueChoice& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_DialogueChoice() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override {}
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }
	virtual void	UI_Active(void* pArg)			 override;
	virtual void	UI_DeActive(void* pArg)			 override;

private:
	CUI_Object*			m_pChildren[ENUM(CHILD::END)] = {};
	CTextSlot*			m_pTextSlots[ENUM(TEXTSLOT::END)] = {};
	class CButtonUI*	m_pBtns[ENUM(BTNS::END)] = {};

	_uint				m_iNumChoices = {};
	ChoiceDesc			m_pChoiceDesc[3] = {};

private:
	void Cache_Children();

	void Change_Dialogue(_int iIndex);

	void Set_ChildAnimation(CHILD eChild, _int iIndex);
	void Set_ChildText(TEXTSLOT eTextSlot, const wstring& strText);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END