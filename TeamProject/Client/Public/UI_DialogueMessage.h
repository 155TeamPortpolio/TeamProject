#pragma once
#include "UI_Object.h"

NS_BEGIN(Engine)
class CTextSlot;
NS_END

NS_BEGIN(Client)

class CUI_DialogueMessage final : public CUI_Object
{
public:
	typedef struct tagMessageDesc {
		wstring strName = {};
		wstring strMessage = {};
		_bool hasChoice = {};
	}MESSAGE_DESC;

private:
	enum class CHILD { NAME, MESSAGE, ARROW1, ARROW2, BTN_NEXT, END };
	inline static const string CHILD_INSTNAMES[ENUM(CHILD::END)] = { "name", "message", "arrow1", "arrow2", "btnNext" };

	enum class TEXTSLOT { NAME, MESSAGE, END };
	inline static const string TEXTSLOT_INSTNAMES[ENUM(TEXTSLOT::END)] = { "name", "message" };

	typedef struct tagTypeWriteDesc {
		wstring	strFullText = L"";	// 원본 텍스트
		wstring	strCurText = L"";	// 현재까지 출력된 텍스트

		_uint	iCurChar = {};		// 진행 인덱스
		_float	fCharAcc = {};		// 시간 누적
		_float	fCharInterval = { 0.05f };	// 출력 간격
		_bool	isTyping = {};		// 재생 중 여부

		function<void(_uint, const wstring&)> onTyped;

		void Start(const wstring& strText)
		{
			strFullText = strText;
			strCurText = L"";
			iCurChar = 0;
			fCharAcc = 0.f;
			isTyping = true;
		}

		void Update(_float dt, _uint iIndex)
		{
			if (!isTyping)
				return;

			fCharAcc += dt;
			if (fCharAcc >= fCharInterval)
			{
				fCharAcc = 0.f;

				strCurText.push_back(strFullText[iCurChar]);
				++iCurChar;

				if (onTyped)
					onTyped(iIndex, strCurText);
			}
		}

		_bool isFinished() const
		{
			return iCurChar >= strFullText.length();
		}

		void Complete()
		{
			isTyping = false;
		}
	}TYPEWRITER_DESC;

private:
	CUI_DialogueMessage() {}
	CUI_DialogueMessage(const CUI_DialogueMessage& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_DialogueMessage() DEFAULT;
		
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
	class CButtonUI*	m_pBtnNext = { nullptr };
	CTextSlot*			m_pTextSlots[ENUM(TEXTSLOT::END)] = {};

	TYPEWRITER_DESC		m_tMessageTypeWriter = {};
	_bool				m_hasChoice = {};

private:
	void Cache_Children();

	void Change_Dialogue();
	void Show_Choices();

	void Start_TypingMessage(const _wstring& strText);
	void Update_TypingMessage(_float dt);
	_bool Complete_TypingMessage();

	void Set_ChildAnimation(CHILD eChild, _int iIndex);
	void Set_ChildText(TEXTSLOT eTextSlot, const wstring& strText);
	void Set_ChildText(_uint iIndex, const wstring& strText);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END