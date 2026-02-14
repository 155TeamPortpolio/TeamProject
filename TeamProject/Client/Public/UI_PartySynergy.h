#pragma once
#include "UI_Object.h"

// 속성 합 / 합계 속성
// 파티 속성 밸런스
// 현재 편성한 파티의 속성 시너지 요약
// 같은 속성이 2명 이상 -> 속성 강화 보너스 발동
// 특정 조합 

// IconPairUpSkillSmall01.png
// IconPairUpSkillSmall02.png

NS_BEGIN(Engine)
class CSprite2D;
class CTextSlot;
NS_END

NS_BEGIN(Client)

class CUI_PartySynergy final : public CUI_Object
{
private:
	CUI_PartySynergy() {}
	CUI_PartySynergy(const CUI_PartySynergy& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_PartySynergy() DEFAULT;

public:
	void Set_Count(_int iCount);
	void Set_TotalCount(_int iCount);

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }

private:
	class CSprite2D* m_pIcon = {};
	class CTextSlot* m_pText = {};

	wstring m_strText = {};
	_int m_iCount = {};
	_int m_iTotalCount = {};

private:
	void Cache();

	void Set_Text();

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END