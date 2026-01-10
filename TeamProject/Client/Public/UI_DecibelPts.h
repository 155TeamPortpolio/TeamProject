#pragma once
#include "UI_Object.h"
#include "UI_Decibel.h"

NS_BEGIN(Client)

class CUI_DecibelPts final : public CUI_Object
{
public:
	typedef struct tagPtsDesc : public UI_DESC {
		const _float4*	pColor = { nullptr };
	}PTS_DESC;

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
	const _float	m_fHeight = 20.f;
	const _vector2	m_vPadding = { 5.f, 5.f };

	const _float4*	m_pColor = { nullptr };

	UI_HANDLE		m_hPts = {};

private:
	void Ready_PartObjects();
	void Init_PtsObject(CUI_Object* pPts);
	void Init_BgObject(CUI_Object* pBg, CUI_Object* pPts); 

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END