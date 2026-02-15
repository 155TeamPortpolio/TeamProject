#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_SwitchGauge final : public CUI_Object
{
private:
	enum class GAUGE { FRAME, FILL, END };
	inline static const string INSTANCENAMES[ENUM(GAUGE::END)] = { "gaugeFrame", "gaugeFill" };

private:
	CUI_SwitchGauge() {}
	CUI_SwitchGauge(const CUI_SwitchGauge& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_SwitchGauge() DEFAULT;

public:
	void Set_Gauge(_float fRatio);

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }
	virtual void	UI_Active(void* pArg = nullptr)  override;
	virtual void	UI_DeActive(void* pArg = nullptr) override;

private:
	class CUI_Object* m_pGauges[ENUM(GAUGE::END)] = {};
	const _float m_fCapWidth = {17.f};

private:
	void Cache();

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END