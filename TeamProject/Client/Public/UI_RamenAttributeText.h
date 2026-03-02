#pragma once
#include "UI_Object.h"

NS_BEGIN(Engine)
class CTextSlot;
NS_END

NS_BEGIN(Client)

class CUI_RamenAttributeText final : public CUI_Object
{
public:
	typedef struct tagAttributeDesc : public UI_DESC {
		vector<RAMEN_ATTRIBUTE> attributes;
	}ATTRIBUTE_DESC;

private:
	CUI_RamenAttributeText() {}
	CUI_RamenAttributeText(const CUI_RamenAttributeText& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_RamenAttributeText() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override;
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }
	virtual void	UI_Active(void* pArg)			 override;
	virtual void	UI_DeActive(void* pArg)			 override;

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END