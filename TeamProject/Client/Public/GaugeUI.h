#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CGaugeUI final : public CUI_Object
{
private:
	CGaugeUI();
	CGaugeUI(const CGaugeUI& rhs);
	virtual ~CGaugeUI() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(INIT_DESC* pArg = nullptr) override;
	virtual void Priority_Update(_float dt) override;
	virtual void Update(_float dt) override;
	virtual void Late_Update(_float dt) override;
	virtual void Render_GUI() override;

public:
	virtual void Load(const nlohmann::ordered_json& data) override;

private:
	_float		m_fFillAmount = { 1.f };

public:
	static CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = nullptr) override;
	virtual void Free();
};

NS_END