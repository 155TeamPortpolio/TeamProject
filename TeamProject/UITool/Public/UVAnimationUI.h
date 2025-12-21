#pragma once
#include "UIObject_Tool.h"

NS_BEGIN(UITool)

class CUVAnimationUI final : public CUIObject_Tool
{
private:
	CUVAnimationUI();
	CUVAnimationUI(const CUVAnimationUI& rhs);
	virtual ~CUVAnimationUI() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(INIT_DESC* pArg = nullptr) override;

	virtual void Priority_Update(_float dt) override;
	virtual void Update(_float dt) override;
	virtual void Late_Update(_float dt) override;
	virtual void Render_GUI() override;

public:
	virtual void ToJson(json& data) override;
	virtual void FromJson(const json& data) override;

public:
	_bool		m_isUseMask = {};

	_float2		m_vUVOffset = {};
	_float2		m_vUVOffsetSpeed = {};

	_int		m_iTextureKeyIndex = { 0 };		// gui에 콤보박스에서 텍스쳐 선택했을 때 인덱스
	_int		m_iMaskTextureKeyIndex = { 0 };	// gui에 콤보박스에서 텍스쳐 선택했을 때 인덱스

public:
	static _uint m_iCount;

public:
	static CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = nullptr) override;
	virtual void Free();
};

NS_END