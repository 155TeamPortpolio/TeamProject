#pragma once
#include "UI_Object.h"

NS_BEGIN(Engine)
class CSprite2D;
NS_END

NS_BEGIN(Client)

class CUI_ScratchCard final : public CUI_Object
{
private:
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
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }
	virtual void	UI_Active(void* pArg)			 override;
	virtual void	UI_DeActive(void* pArg)			 override;

private:
	CUI_Object*			m_pBrush = {};
	class CSprite2D*	m_pBrushSprite = {};
	class CSprite2D*	m_pRewardSprite = {};

	_float4x4	m_ViewMatrix = {};
	_float4x4   m_ProjMatrix = {};

	_float		m_fThreshold = { -0.1f };

	_bool		m_isClear = {};

private:
	void Cache_Brush();
	void Cache_Reward();

	void Change_RewardTexture(const string& strTextureKey);

	void Render_RTBrush(ID3D11DeviceContext* pContext);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END