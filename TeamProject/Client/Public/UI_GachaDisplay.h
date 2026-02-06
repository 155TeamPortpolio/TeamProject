#pragma once
#include "UI_Object.h"

NS_BEGIN(Engine)
class CTextSlot;
NS_END

NS_BEGIN(Client)

class CUI_GachaDisplay final : public CUI_Object
{
public:
	enum class TYPE { LABEL, SKIP, END };
	typedef struct tagGachaDisplayDesc {
		TYPE	eType;
		wstring strLabel = {};
	}GACHA_DISPLAY_DESC;

private:
	enum class CHILD { BG, LABEL, END };
	inline static const string INSTANCENAMES[ENUM(CHILD::END)] = { "bg", "label" };

private:
	CUI_GachaDisplay() {}
	CUI_GachaDisplay(const CUI_GachaDisplay& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_GachaDisplay() DEFAULT;

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
	CUI_Object* m_pChildren[ENUM(CHILD::END)] = {};
	CUI_Object* m_pSkipButton = {};

	class CTextSlot* m_pLabelTextSlot = {};

	_bool	m_isLabelVisible = {};

private:
	void Cache();
	void Create_SkipButton();

	void Set_ChildAnimation(CHILD child, _int iIndex);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END