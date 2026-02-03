#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_Lottery final : public CUI_Object
{
public:
	/* 복권 사용 완료 상태 */
	enum STATE { READY, USED, END };

private:
	enum class CHILD { 
		BTN_REFRESH, BTN_SCRATCH,
		OVERLAY, OVERLAY_REFRESH,
		ICON_REFRESH, ICON_SCRATCH, 
		NEWSPAPER, SCRATCH, END };

	inline static const string INSTANCENAMES[ENUM(CHILD::END)] = { 
		"btnRefresh", "btnScratch", 
		"overlay", "overlayRefresh",
		"iconRefresh", "iconScratch", 
		"", ""};

	enum class BTN { BTN_SCRATCH, BTN_REFRESH, END };
	inline static const string BTN_NAMES[ENUM(BTN::END)] = { "btnScratch", "btnRefresh" };

	// 뼈따구(888) / 개밥(2888) / 고기(5888) / 강아지(8888) / 황금똥(12888)
	enum class REWARD { REWARD1, REWARD2, REWARD3, REWARD4, REWARD5, END };
	inline static const string REWARD_TEXTURES[ENUM(REWARD::END)] = { "ScratchCardRewardIcon01.png", "ScratchCardRewardIcon02.png",
	"ScratchCardRewardIcon03.png", "ScratchCardRewardIcon04.png", "ScratchCardRewardIcon05.png" };
	inline static const _uint REWARD_DENY[ENUM(REWARD::END)] = { 888, 2888, 5888, 8888, 12888 };

private:
	CUI_Lottery() {}
	CUI_Lottery(const CUI_Lottery& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_Lottery() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override;
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }
	virtual void	UI_Active(void* pArg)			 override;
	virtual void	UI_DeActive(void* pArg)			 override;

private:
	_uint			m_iState = { STATE::END };
	_uint			m_iReward = ENUM(REWARD::END);

	CUI_Object*		m_pChildren[ENUM(CHILD::END)] = {};
	class CButtonUI* m_pButtons[ENUM(BTN::END)] = {};
	CUI_Object* m_pResultBanner = {};

private:
	void Cache(); 
	void Create_Newspaper();
	void Create_ScratchCard();
	void Create_BackButton();
	void Create_ResultBanner();

	void Change_State(STATE eState);

	void OnClick_Back();
	void OnClick_RefreshNews();
	void OnClick_OpenScratch(); 

	void OnScratch_Complete();

	void Set_ChildUIActive(CHILD child, void* pArg = nullptr);
	void Set_ChildUIDeActive(CHILD child, void* pArg = nullptr);
	void Set_ChildAnimation(CHILD child, _int iIndex);
	void Change_ChildTexture(CHILD child, const string& strTextureKey);

	_bool Is_ChildAlive(CHILD child);
	_bool Is_ChildAnimationFinished(CHILD child);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END