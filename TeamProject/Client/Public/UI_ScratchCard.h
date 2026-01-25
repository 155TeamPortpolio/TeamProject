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

private:
	class CSprite2D* m_pRewardSprite = {};

private:
	void Cache_RewardSprite();
	void Change_RewardTexture(const string& strTextureKey);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END