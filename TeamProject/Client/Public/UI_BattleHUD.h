#pragma once
#include "UI_Object.h"

NS_BEGIN(Engine)
class CSprite2D; class CTextSlot; 
NS_END

NS_BEGIN(Client)
class CUI_DamageText; class CGaugeUI;

class CUI_BattleHUD final : public CUI_Object
{
public:
	typedef struct tagShowDesc {
		_bool isFade = {};
	}SHOW_DESC;

private:
	enum CHILD {
		ROLE1,         ROLE2,         ROLE3,
		ICON1,         ICON2,         ICON3,
		HP_BACK1,      HP_BACK2,      HP_BACK3,
		HP_FRONT1,     HP_FRONT2,     HP_FRONT3,
		SPECIAL1,      SPECIAL2,      SPECIAL3,
		SPECIALARROW1, SPECIALARROW2, SPECIALARROW3,
		ULTIMATE1,     ULTIMATE2,     ULTIMATE3,
		ULTIMATEICON1, ULTIMATEICON2, ULTIMATEICON3,
		CUR_HP_TEXT,   MAX_HP_TEXT,
		ACTION,
		DAMAGE_TEXT,
		END
	};

	inline static const string INSTANCENAMES[ENUM(CHILD::END)] =	{
		"role1",         "role2",         "role3",
		"icon1",         "icon2",         "icon3",
		"hpBack1",       "hpBack2",       "hpBack3",
		"hpFront1",      "hpFront2",      "hpFront3",
		"special1",      "special2",      "special3",
		"specialArrow1", "specialArrow2", "specialArrow3",
		"ultimate1",     "ultimate2",     "ultimate3",
		"ultimateIcon1", "ultimateIcon2", "ultimateIcon3",
		"curHpText",     "maxHpText",
		"action",
		"damageText",
	};

	inline static constexpr CHILD  ROLE_CHILD[]         = { ROLE1, ROLE2, ROLE3 };
	inline static constexpr CHILD  ICON_CHILD[]         = { ICON1, ICON2, ICON3 };
	inline static constexpr CHILD  HPBACK_CHILD[]       = { HP_BACK1, HP_BACK2, HP_BACK3 };
	inline static constexpr CHILD  HPFRONT_CHILD[]      = { HP_FRONT1, HP_FRONT2, HP_FRONT3 };
	inline static constexpr CHILD  SPECIAL_CHILD[]      = { SPECIAL1, SPECIAL2, SPECIAL3 };
	inline static constexpr _float SPECIAL_THRESHOLD[]  = { 150.f, 48.f, 48.f };
	inline static constexpr CHILD  SPECIALARROW_CHILD[] = { SPECIALARROW1, SPECIALARROW2, SPECIALARROW3 };
	inline static constexpr CHILD  ULTIMATE_CHILD[]     = { ULTIMATE1, ULTIMATE2, ULTIMATE3 };
	inline static constexpr CHILD  ULTIMATEICON_CHILD[] = { ULTIMATEICON1, ULTIMATEICON2, ULTIMATEICON3 };

	inline static const string ICONTEXTURES[ENUM(CHARACTER::END)] = { "IconRoleGeneral24.png", "IconRoleGeneral09.png" };

private:
	CUI_BattleHUD() {}
	CUI_BattleHUD(const CUI_BattleHUD& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_BattleHUD() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }
	virtual void	UI_Active(void* pArg = {})       override;
	virtual void	UI_DeActive(void* pArg = {})     override;

private:
	CUI_Object*     m_pChildren[ENUM(CHILD::END)]    = {};
	CSprite2D*	    m_pSprites[ENUM(CHILD::END)]     = {};
	CTextSlot*	    m_ptextSlots[ENUM(CHILD::END)]   = {};
	CGaugeUI*		m_pGauges[ENUM(CHILD::END)]      = {}; 
	CUI_DamageText* m_pDamageTexts[ENUM(CHILD::END)] = {};

	static constexpr _int   ROLE_COUNT        = 3;
	static constexpr _float HPBACK_DELTA      = 5.f;
	static constexpr _float HPBACK_LERP_SPEED = 7.f;

	GAUGE_DELAY_DESC	m_hpBack[ROLE_COUNT] = {};

private:
	const _int			m_iPlayerHPWidth = 5;
	const _int			m_iBossHPWidth   = 2;

private:
	void Add_PartObject( const string& strLevelKey, const string& strPrototypeTag, const string& strInstanceName, CHILD child, _float2 vOffset = _float2(), UI_DESC* pDesc = {});
	void Cache_Children();
	 
	void Set_Values(UI_PLAYER_STATUS_DESC desc);
	void Set_Special(_int iIndex, _float fRatio, _float fThresRatio);
	void Set_UltimateIcon(_int iIndex, _float fRatio);

	void Update_HPBackGauge(_float dt);

	_bool Is_ChildAlive(CHILD child);
	void Set_ChildAlive(CHILD child, _bool isAlive);
	void Set_ChildColor(CHILD child, _float4 vColor);
	void Set_ChildAnimation(CHILD child, _int iIndex);
	void Set_ChildAnchorOffsetX(CHILD child, _float fOffsetX);

	void Change_ChildTexture(CHILD child, const string& strTextureKey);
	void Set_GaugeFill(CHILD child, _float fFillAmount);
	void Set_NumberText(CHILD child, _int iNum, _int iWidth);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END