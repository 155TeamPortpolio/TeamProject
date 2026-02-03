#pragma once

#include "UI_WorldToScreen.h"

NS_BEGIN(Client)

class CUI_Seoriyeol final : public CUI_WorldToScreen
{
private:
	enum CHILD
	{
		ONE, END
	};

	inline static const string CHILD_INSTNAMES[ENUM(CHILD::END)] = {};

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