#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CUltimateBG :
    public CGameObject
{
private:
	CUltimateBG() {}
	CUltimateBG(const CUltimateBG& rhs) : CGameObject(rhs) {}
	virtual ~CUltimateBG() DEFAULT;

public:
	void Show_Ultimate(CHARACTER eCharacter, _float duration);

public:
	virtual HRESULT Initialize_Prototype()     override;
	virtual HRESULT Initialize(INIT_DESC* pArg) override;
	virtual void	Awake()						override;
	virtual void    Priority_Update(_float dt) override {}
	virtual void    Update(_float dt)          override;
	virtual void    Late_Update(_float dt)     override {}

private:
	void Ready_RT();

private:
	_float2 m_vViewPortSize = {};

	_float4x4 m_ViewMatrix = {};
	_float4x4 m_ProjMatrix = {};

	_float		m_fAccTime = {};
	_float		m_fDuration = {};

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END