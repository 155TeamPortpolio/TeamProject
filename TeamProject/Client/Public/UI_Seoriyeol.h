#pragma once

#include "UI_WorldToScreen.h"

NS_BEGIN(Client)

class CUI_Seoriyeol final : public CUI_WorldToScreen
{
private:
	enum CHILD
	{
		SEO_IN_1,     SEO_IN_2,
		RI_IN_1,      RI_IN_2,
		YEOL_IN_1,    YEOL_IN_2,
		FACTORY_1,    FACTORY_2,   FACTORY_3, FACTORY_4,
		SPRITEANIM_1, SPRITEANIM_2, SPRITEANIM_3, SPRITEANIM_4,
		ICE, 
		END
	};

	inline static const string CHILD_INSTNAMES[ENUM(CHILD::END)] =  {
		"Seo_In_1",     "Seo_In_2", 
		"Ri_In_1",      "Ri_In_2",
		"Yeol_In_1",    "Yeol_In_2",
		"Factory_1",    "Factory_2",     "Factory_3",    "Factory_4", 
		"SpriteAnim_1", "SpriteAnim_2",  "SpriteAnim_3", "SpriteAnim_4",
		"Ice"
	};

private:
	CUI_Seoriyeol() {}
	CUI_Seoriyeol(const CUI_Seoriyeol& rhs) : CUI_WorldToScreen(rhs) {}
	virtual ~CUI_Seoriyeol() DEFAULT;

public:
	HRESULT Initialize_Prototype()          override;
	HRESULT Initialize(INIT_DESC* arg = {}) override;
	void    Update(_float dt)               override;
	void    UI_Active(void* arg)            override;
	void    UI_DeActive(void* arg)          override;
	_bool   Is_AnimFinished()              override;

private:
	void    Cache_Children();
	void    SetAllChildAnim(_int idx) const;

private:
	CUI_Object* m_children[ENUM(CHILD::END)]{};

public:
	static CUI_Seoriyeol* Create();
	CGameObject* Clone(INIT_DESC* pArg = {}) override;
};

NS_END