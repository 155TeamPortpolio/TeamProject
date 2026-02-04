#pragma once
#include "UI_Object.h"

NS_BEGIN(Engine)
class CTextSlot;
NS_END

NS_BEGIN(Client)

class CUI_GachaCurrency final : public CUI_Object
{
private:
	enum class CHILD { PADDING, VALUE, END };
	inline static const string INSTANCENAMES[ENUM(CHILD::END)] = { "padding", "value" };

private:
	CUI_GachaCurrency() {}
	CUI_GachaCurrency(const CUI_GachaCurrency& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_GachaCurrency() DEFAULT;

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
	class CUI_Object* m_pChildren[ENUM(CHILD::END)] = {};
	class CTextSlot* m_pTextSlots[ENUM(CHILD::END)] = {};

	const _int m_iMaxDigits = { 8 };

private:
	void Cache();

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END