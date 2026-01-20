#pragma once
#include "UI_Object.h"
#include "UI_Decibel.h"

NS_BEGIN(Engine)
class CSprite2D;
NS_END

NS_BEGIN(Client)

class CUI_DecibelPts final : public CUI_Object
{
public:
	typedef struct tagPtsDesc : public UI_DESC {
		const _float4*	pColor = { nullptr };
	}PTS_DESC;

private:
	enum CHILD { BG, PTS, END };

private:
	CUI_DecibelPts() {}
	CUI_DecibelPts(const CUI_DecibelPts& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_DecibelPts() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }

private:
	CUI_Object*		m_pChildren[ENUM(CHILD::END)] = {};
	class CSprite2D* m_pSprites[ENUM(CHILD::END)] = {};

	const _float	m_fHeight = 20.f;
	const _vector2	m_vPadding = { 20.f, 10.f };

	const _float4*	m_pColor = { nullptr };

private:
	void Ready_PartObjects();
	void Init_PtsObject();
	void Init_BgObject(_float2 vSize);

	void Set_ChildColor(CHILD child, _float4 vColor);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END