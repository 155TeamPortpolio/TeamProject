#pragma once
#include "UI_Object.h"

NS_BEGIN(Engine)
class CTextSlot;
NS_END

NS_BEGIN(Client)

class CUI_Dialogue final : public CUI_Object
{
private:
	enum class CHILD { NAME, TEXT, ARROW1, ARROW2, END };
	inline static const string INSTANCENAMES[ENUM(CHILD::END)] = { "name", "text", "arrow1", "arrow2" };

	enum class TYPING { NAME, TEXT, END };
	inline static const string TEXTNAMES[ENUM(TYPING::END)] = { "name", "text" };

	typedef struct tagTypingDesc {
		wstring	strFullText = L"";
		wstring	strCurText = L"";

		_uint	iCurChar = {};
		_float	fCharAcc = {};
		_float	fCharInterval = { 0.05f };
		_bool	isTyping = {};
	}TYPING_DESC;

private:
	CUI_Dialogue() {}
	CUI_Dialogue(const CUI_Dialogue& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_Dialogue() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }

private:
	CUI_Object*		m_pChildren[ENUM(CHILD::END)] = {};
	CTextSlot*		m_pTexts[ENUM(TYPING::END)] = {};

	TYPING_DESC		m_tTyping[ENUM(TYPING::END)] = {};

private:
	void Cache_Children();

	void Start_Typing(TYPING typing, const _wstring& strText);

	void Set_ChildText(TYPING typing, const wstring& strText);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END