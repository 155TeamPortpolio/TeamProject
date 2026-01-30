#pragma once
#include "UI_Object.h"

NS_BEGIN(Engine)
class CSprite2D;
NS_END

NS_BEGIN(Client)

class CUI_RamenAttribute final : public CUI_Object
{
public:
	typedef struct tagAttributeDesc : public UI_DESC {
		string strAttributeID;
	}ATTRIBUTE_DESC;

private:
	inline static const unordered_map<string, string> ICON_TEXTURES = {
		{"atk", "IconATK.png"},
		{"max_hp", "IconMaxHP.png"},
		{"dmg_ether", "IconEther.png"},
		{"dmg_fire", "IconFire.png"},
		{"dmg_ice", "IconIce.png"}, 
		{"dmg_physical", "IconPhysDmg.png"},  
		{"dmg_thunder", "IconThunder.png"}, 
		{"dmg_to_boss", "IconToBossDmg.png"},
	};

private:
	CUI_RamenAttribute() {}
	CUI_RamenAttribute(const CUI_RamenAttribute& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_RamenAttribute() DEFAULT;

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
	class CSprite2D* m_pIconSprite = {};

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END