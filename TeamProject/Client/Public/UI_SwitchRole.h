#pragma once
#include "UI_Object.h"

NS_BEGIN(Engine)
class CSprite2D;
NS_END

NS_BEGIN(Client)

class CUI_SwitchRole final : public CUI_Object
{
public:
	enum class SIDE { LEFT, RIGHT, END };

private:
	enum class SPRITE { ROLE, MOUSE, END};
	inline static const string INSTANCENAMES[ENUM(SPRITE::END)] = { "iconRole", "iconMouse" };

private:
	CUI_SwitchRole() {}
	CUI_SwitchRole(const CUI_SwitchRole& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_SwitchRole() DEFAULT;

public:
	void Set_Side(SIDE eSide);
	void Change_RoleIcon(CHARACTER eCharacter);

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
	class CSprite2D* m_pIconSprite[ENUM(SPRITE::END)] = {nullptr};

private:
	void Cache();

	void Change_SpriteTexture(SPRITE eSprite, const string& strTextureKey);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END