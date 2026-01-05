#pragma once
#include "UIObject_Tool.h"

NS_BEGIN(UITool)

class CSpriteAnimationUI final : public CUIObject_Tool
{
private:
	CSpriteAnimationUI() {}
	CSpriteAnimationUI(const CSpriteAnimationUI& rhs) : CUIObject_Tool(rhs) {}
	virtual ~CSpriteAnimationUI() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(INIT_DESC* pArg = nullptr) override;

	virtual void	Priority_Update(_float dt) override {}
	virtual void	Update(_float dt) override;
	virtual void	Late_Update(_float dt) override {}
	virtual void	Render_GUI() override;

public:
	virtual void	Save(nlohmann::ordered_json& data) override;
	virtual void	Load(const nlohmann::ordered_json& data) override;

private:
	_bool		m_isPlaying = { true };
	_bool		m_isLoop = {};

	_uint		m_iFrameCountX = { 1 };
	_uint		m_iFrameCountY = { 1 };
	_uint		m_iFrameCountTotal = { 1 };
	_float		m_fFrameSpeed = { 30.f };

	_float		m_fFrameAccTime = {};
	_uint		m_iCurrentFrameIndex = {};

private:
	string		m_strTextureKey = {};

public:
	inline static const string m_strTypeTag = "SpriteAnimation";
	inline static       _uint  m_iCount{};

public:
	static CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = nullptr) override;
	virtual void Free();
};

NS_END