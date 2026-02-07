#pragma once
#include "UI_Object.h"

NS_BEGIN(Engine)
class CSprite2D;
NS_END

NS_BEGIN(Client)

class CUI_GachaResultItem final : public CUI_Object
{
private:
	enum class SPRITE { ITEM_ICON_BACK, ITEM_ICON_FRONT, RANK_ICON, END };
	inline static const string SPRITE_INSTANCENAMES[ENUM(SPRITE::END)] = { "itemIconBack", "itemIconFront", "rankIcon" };

private:
	CUI_GachaResultItem() {}
	CUI_GachaResultItem(const CUI_GachaResultItem& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_GachaResultItem() DEFAULT;

public:
	void Set_ResultDesc(GACHA_RESULT_DESC desc);

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
	CUI_Object* m_pOverlay = {};

	class CUI_Object* m_pChildren[ENUM(SPRITE::END)] = {};
	class CSprite2D* m_pSprites[ENUM(SPRITE::END)] = {};

	_float3 m_vRGB = {};

private:
	void Cache();

	void Change_SpriteTexture(SPRITE sprite, const string& strTextureKey);
	void Set_ChildAnchorOffset(SPRITE sprite, _float2 vOffset);
	void Set_ChildSize(SPRITE sprite, _float2 vSize);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END