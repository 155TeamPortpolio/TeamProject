#pragma once
#include "UI_AnomalyBase.h"

NS_BEGIN(Client)

class CUI_AnomalyGauge final : public CUI_AnomalyBase
{
private:
	enum class TYPE { GAUGE, INDICATOR, END };
	enum class STATE { ACTIVE, INACTIVE, END };

	enum class CHILD { GAUGE, GAUGE_OVERLAY1, GAUGE_OVERLAY2, INDICATOR, INDICATOR_OVERLAY, END };
	inline static const string INSTANCENAMES[ENUM(CHILD::END)] = { "gauge", "gauge_overlay1", "gauge_overlay2", "indicator", "indicator_overlay" };

private:
	CUI_AnomalyGauge() {}
	CUI_AnomalyGauge(const CUI_AnomalyGauge& rhs) : CUI_AnomalyBase(rhs) {}
	virtual ~CUI_AnomalyGauge() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }
	virtual void	UI_Active(void* pArg = nullptr)	 override;
	virtual void	UI_DeActive(void* pArg = nullptr) override;

private:
	CUI_Object* m_pChildren[ENUM(CHILD::END)] = { nullptr };

	STATE m_eState[ENUM(TYPE::END)] = {STATE::END};

	_float m_fRatio = {};

private:
	void Cache();

	void Update_Gauge(_float fRatio);

	void Change_State(TYPE eType, STATE eState);
	void Enter_Active(TYPE eType);
	void Enter_Inactive(TYPE eType);

	void Set_Gauge(_float fRatio);

	void Set_ChildAlive(CHILD child, _bool isAlive);
	void Set_ChildAnimation(CHILD child, _int iIndex);
	void Change_ChildTexture(CHILD child, _int iIndex, const string& strTextureKey);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END