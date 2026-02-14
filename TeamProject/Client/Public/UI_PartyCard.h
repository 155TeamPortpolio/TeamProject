#pragma once
#include "UI_Object.h"

NS_BEGIN(Engine)
class CSprite2D;
class CTextSlot;
NS_END

NS_BEGIN(Client)

class CUI_PartyCard final : public CUI_Object
{
public:
	typedef struct tagCardDesc : public UI_DESC {
		string strRenderTargetKey = "";
	}CARD_DESC;

private:
	enum class CHILD { BG_BACK, BG_FRONT, EMPTY, OCCUPIED, END };
	inline static const string INSTANCENAMES[ENUM(CHILD::END)] = { "bgBack", "bgFront", "empty", "occupied" };

	enum class SPRITE2D { ICON_PAIR, ICON_ATTRIBUTE, ICON_SPECIALTY, END };
	inline static const string INSTANCENAMES_SPRITE2D[ENUM(SPRITE2D::END)] = { "iconPair", "iconAttribute", "iconSpecialty" };

	enum class TEXTSLOT { NAME, LEVEL, END };
	inline static const string INSTANCENAMES_TEXTSLOT[ENUM(TEXTSLOT::END)] = { "name", "level" };

private:
	CUI_PartyCard() {}
	CUI_PartyCard(const CUI_PartyCard& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_PartyCard() DEFAULT;

public:
	void Change_Character(CHARACTER eCharacter);
	
public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }

private:
	CUI_Object* m_pRTDraw = { nullptr };
	OBJECT_HANDLE m_hAvatar = {};

	CUI_Object* m_pChildren[ENUM(CHILD::END)] = {}; 
	class CSprite2D* m_pSprites[ENUM(SPRITE2D::END)] = {};
	class CTextSlot* m_pTextSlots[ENUM(TEXTSLOT::END)] = {};

private:
	void Cache();
	void Create_RenderTarget(const string& strRenderTargetKey);
	HRESULT Create_Avatar(const string& strRenderTargetKey);

	void Change_Avatar(const string& strModelKey, const string& strMaterialKey, const string& strMetaKey, const string& strAnimClipKey, _float3 vPosition);
	void Change_Icons(const string& strPairKey, const string& strAttributeKey, const string& strSpecialtyKey);
	void Change_Texts(const wstring& strName, _int iLevel);

	void Set_ChildAlive(CHILD child, _bool isAlive);
	void Change_SpriteTexture(SPRITE2D sprite, const string& strTextureKey);
	void Set_Text(TEXTSLOT textSlot, const _wstring& strText);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END