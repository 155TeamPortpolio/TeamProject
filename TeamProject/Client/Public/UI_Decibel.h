#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_Decibel final : public CUI_Object
{
	enum UI { KANJI, DIGIT_1000, DIGIT_100, DIGIT_10, DIGIT_1, POINT, END };

	struct tagUIDesc {
		UI		type;
		string  strTextureTag;
		_float	fHeight;
		_float	fOffsetY;
		UI		left;

	};

	static const tagUIDesc UIDescs[];

private:
	CUI_Decibel() {}
	CUI_Decibel(const CUI_Decibel& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_Decibel() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }

private:
	_int				m_iDecibel = {};

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END