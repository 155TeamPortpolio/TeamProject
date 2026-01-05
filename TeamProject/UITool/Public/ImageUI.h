#pragma once
#include "UIObject_Tool.h"

NS_BEGIN(UITool)

class CImageUI final : public CUIObject_Tool
{
private:
	CImageUI() {}
	CImageUI(const CImageUI& rhs) : CUIObject_Tool(rhs) {}
	virtual ~CImageUI() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void    Priority_Update(_float dt)       override {}
	virtual void    Update(_float dt)                override;
	virtual void    Late_Update(_float dt)           override {}
	virtual void    Render_GUI()                     override;

public:
	virtual void	Save(nlohmann::ordered_json& data) override;
	virtual void	Load(const nlohmann::ordered_json& data) override;

private:
	string		m_strTextureKey = {};

	_bool		m_isFlipX = {};
	_bool		m_isFlipY = {};
	_float2		m_vFlip = {};

public:
	inline static const string m_strTypeTag = "Image";
	inline static       _uint  m_iCount{};

public:
	static CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = nullptr) override;
	virtual void Free() { __super::Free(); }
};

NS_END