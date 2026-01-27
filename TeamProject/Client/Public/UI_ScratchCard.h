#pragma once
#include "UI_Object.h"

NS_BEGIN(Engine)
class CSprite2D;
NS_END

NS_BEGIN(Client)

class CUI_ScratchCard final : public CUI_Object
{
private:
	enum class STATE { INVISIBLE, VISIBLE, END };
	enum class ANIMATION { APPEAR, DISAPPEAR, IDLE, END };

	enum class CHILD { BRUSH, SCRATCH, REWARD, END };
	inline static const string INSTANCENMAES[ENUM(CHILD::END)] = { "brush", "scratch", "reward" };

	enum class REWARD { REWARD1, REWARD2, REWARD3, REWARD4, REWARD5, END };
	inline static const string REWARD_TEXTURES[ENUM(REWARD::END)] = { "ScratchCardRewardIcon01.png", "ScratchCardRewardIcon02.png",
	"ScratchCardRewardIcon03.png", "ScratchCardRewardIcon04.png", "ScratchCardRewardIcon05.png" };

private:
	CUI_ScratchCard() {}
	CUI_ScratchCard(const CUI_ScratchCard& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_ScratchCard() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override {}
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }
	virtual void	UI_Active(void* pArg)			 override;
	virtual void	UI_DeActive(void* pArg)			 override;

private:
	STATE				m_eState = { STATE::END };

	CUI_Object*			m_pChildren[ENUM(CHILD::END)] = {};
	class CSprite2D*	m_pSprites[ENUM(CHILD::END)] = {};

	/* 커스텀 렌더타겟 관련 변수들 */
	_float4x4			m_ViewMatrix = {};			// 브러쉬 뷰
	_float4x4			m_ProjMatrix = {};			// 브러쉬 프로젝션
	_float				m_fThreshold = { -0.1f };	// 브러쉬로 그린 렌더타겟을 마스크로 쓸 때 흑백 반전시키려고
	_bool				m_isClear = {};				// 렌더타겟 클리어

	/* 스크래치 진행 정도 확인을 위한 변수들 */
	_bool				m_isScratchComplete = {};	// 스크래치 완료
	_float				m_fScratchTimer = {};		// 스크래치 확인 타이머
	const _float	    m_fScratchDuration = { 1.f }; // 스크래치 확인 시간 간격
	const _float		m_fScratchRatio = { 0.3f };	// 스크래치 정도

	/* 결과배너 관련 변수들 */
	_bool				m_hasShownResult = {};		// 결과 배너를 보여줌
	_float				m_fResultWaitTime = {};		// 결과 배너 타이머
	const _float		m_fResultWaitDuration = { 1.f };	// 결과 배너 몇 초 후에

private:
	void Cache();

	_bool Check_Scratch(_float dt);
	_float Calculate_ScratchRatio();

	void Change_State(STATE eState);
	void Change_RewardTexture(const string& strTextureKey);
	 
	void Reset();

	void Render_RTBrush(ID3D11DeviceContext* pContext);
	void Render_RT(CHILD child, ID3D11DeviceContext* pContext);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() override;
};

NS_END