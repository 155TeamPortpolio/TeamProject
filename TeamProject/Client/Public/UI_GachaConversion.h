#pragma once
#include "UI_Object.h"

NS_BEGIN(Engine)
class CTextSlot;
NS_END

NS_BEGIN(Client)

class CUI_GachaConversion final : public CUI_Object
{
public:
	typedef struct tagConversionDesc : public UI_DESC {
		_int iCost = {};
		_int iCount = {};
	}CONVERSION_DESC;

private:
	enum class CHILD { BUTTON, OVERLAY, LABEL, COST, COUNT, END };
	inline static const string INSTANCENAMES[ENUM(CHILD::END)] = { "button", "overlay", "label", "cost", "count" };

private:
	CUI_GachaConversion() {}
	CUI_GachaConversion(const CUI_GachaConversion& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_GachaConversion() DEFAULT;

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
	_bool m_isClicked = {};

	class CUI_Object* m_pChildren[ENUM(CHILD::END)] = {};
	class CTextSlot* m_pTextSlots[ENUM(CHILD::END)] = {};
	class CButtonUI* m_pButton = {};

private:
	void Cache();

	void OnClick();

	void Set_ChildAnimation(CHILD child, _int iIndex);
	void Set_ChildText(CHILD child, const _wstring& strText);

	_bool Is_ChildAnimationFinished(CHILD child);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END