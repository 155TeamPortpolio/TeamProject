#pragma once
#include "UI_Object.h"

NS_BEGIN(Engine)
class CSprite2D;
class CTextSlot;
NS_END

NS_BEGIN(Client)

class CUI_ResultBanner final : public CUI_Object
{
public:
	typedef struct tagResultDesc {
		string strTextureKey = { "empty.png" };
		wstring wstrText1 = {};
		wstring wstrText2 = {};
	}RESULT_DESC;

private:
	enum class STATE { INVISIBLE, VISIBLE, END };

	enum class CHILD { OVERLAY_OK, TEXT_OK, ICON_OK, END };
	inline static const string INSTANCENAMES[ENUM(CHILD::END)] = { "overlayOK", "textOK", "iconOK" };

	enum class TEXT { TEXT1, TEXT2, END };
	inline static const string TEXT_NAMES[ENUM(TEXT::END)] = { "text1", "text2" };

private:
	CUI_ResultBanner() {}
	CUI_ResultBanner(const CUI_ResultBanner& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_ResultBanner() DEFAULT;

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
	STATE				m_eState = { STATE::END };

	CUI_Object*			m_pChildren[ENUM(CHILD::END)] = {};
	class CButtonUI*	m_pBtnOk = {};
	class CSprite2D*	m_pSpriteIcon = {};
	class CTextSlot*	m_pTextSlot[ENUM(TEXT::END)] = {};

private:
	void Cache();

	void Change_State(STATE eState);

	void Set_ChildColor(CHILD child, _float4 vColor);
	void Set_ChildAnimation(CHILD child, _int iIndex);
	void Set_Text(TEXT text, const _wstring& strText);
	void Change_IconTexture(const string& strTextureKey);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END