#pragma once

#include "UI_WorldToScreen.h"

NS_BEGIN(Client)
class CUI_Gangta final : public CUI_WorldToScreen
{
private:
	enum CHILD { GANG_OUT, GANG_IN, TA_OUT, TA_IN, GANG_OUTLINE, TA_OUTLINE, FACTORY_OUT_01, FACTORY_OUT_02,
		FACTORY_IN_01, FACTORY_IN_02, END };
	inline static const string CHILD_INSTNAMES[ENUM(CHILD::END)] = 
	{
		"Gang_Out", "Gang_In", "Ta_Out", "Ta_In", "Gang_Outline", "Ta_Outline",
		"Factory_Out_01", "Factory_Out_02",
		"Factory_In_01", "Factory_In_02",
	};

private:
	CUI_Gangta() {}
	CUI_Gangta(const CUI_Gangta& rhs) : CUI_WorldToScreen(rhs) {}
	virtual ~CUI_Gangta() DEFAULT;

public:
	HRESULT Initialize_Prototype()          override;
	HRESULT Initialize(INIT_DESC* arg = {}) override;
	void	Awake()							override {}
	void    Update(_float dt)			    override;
	void    UI_Active(void* arg)            override;
	void    UI_DeActive(void* arg)          override;

	_bool   Is_AnimFinished()               override;

private:
	void  Cache_Children();
	void  Set_ChildAnim(CHILD eChild, _int idx) const;
	_bool Is_ChildAnimFinished(CHILD eChild)    const;

private:
	CUI_Object* m_children[ENUM(CHILD::END)]{};

public:
	static CUI_Gangta* Create();
	CGameObject* Clone(INIT_DESC* pArg = {}) override;
};
NS_END