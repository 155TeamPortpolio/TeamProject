#pragma once
#include "UI_Object.h"

NS_BEGIN(Engine)
class CTextSlot;
NS_END

NS_BEGIN(Client)

class CUI_Dialogue final : public CUI_Object
{
private:
	enum class CHILD { MESSAGE, CHOICE, END };
	enum class STATE { INVISIBLE, VISIBLE, END };

private:
	CUI_Dialogue() {}
	CUI_Dialogue(const CUI_Dialogue& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_Dialogue() DEFAULT;

public:
	void Change_Dialogue();
	void Change_Dialogue(ChoiceDesc desc);
	void Show_Choices();

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override {}
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }

private:
	CUI_Object*			m_pChildren[ENUM(CHILD::END)] = {};

	STATE				m_eState = {};

	NpcDialogueDesc		m_tDialogueDesc = {};

private:
	void Add_Children(const string& strLevelTag, const string& strPrototypeTag, CHILD child);
	void Bind_EventListener();

	void Open_Dialogue(const string& strNewSequenceID, _uint iNewSequenceID);
	
	void Change_State(STATE eState);

	void BroadCast_NPCInteractDesc(wstring strName, _uint iCurSequenceID, _uint iNextSequenceID, DialogueResult eResult);

	void Set_ChildUIActive(CHILD child);
	void Set_ChildUIDeActive(CHILD child);
	_bool Is_ChildAnimFinished(CHILD child);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END