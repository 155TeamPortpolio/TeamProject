#pragma once
#include "UI_Object.h"

NS_BEGIN(Engine)
class CSprite2D;
NS_END

NS_BEGIN(Engine)

class CUI_Newspaper final : public CUI_Object
{
private:
	enum class CHILD { NEWS1, NEWS2, END };
	inline static const string INSTANCENAMAES[ENUM(CHILD::END)] = { "news1", "news2" };

	inline static const size_t TEXTURE_COUNT = 3;
	inline static const string TEXTURES[TEXTURE_COUNT] = { "NewsPage01.png", "NewsPage02.png", "NewsPage03.png" };

private:
	CUI_Newspaper() {}
	CUI_Newspaper(const CUI_Newspaper& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_Newspaper() DEFAULT;

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
	_uint			m_iChildIndex = { ENUM(CHILD::NEWS1)};
	_uint			m_iTextureIndex = {};

	CUI_Object*		m_pChildren[ENUM(CHILD::END)] = {};
	class CSprite2D* m_pSprites[ENUM(CHILD::END)] = {};

	_bool			m_isPaging = {};
	_float			m_fPagingTimer = {};
	const _float	m_fPagingDuration = { 0.5f };

private:
	void Cache();

	void Update_Paging(_float dt);

	void Set_ChildAnimation(CHILD child, _int iIndex);
	void Change_ChildTexture(CHILD child, const string& strTextureKey);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END