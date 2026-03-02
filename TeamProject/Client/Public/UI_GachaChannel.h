#pragma once
#include "UI_Object.h"
#include "UI_GachaPage.h"

NS_BEGIN(Engine)
class CSprite2D;
class CTextSlot;
NS_END

NS_BEGIN(Client)

class CUI_GachaChannel final : public CUI_Object
{
public:
	typedef struct tagChannelDesc : public UI_DESC {
		CUI_GachaPage::CHANNEL eChannel = {};
		wstring strLabel = {};
		string strTextureKey = {}; 
		function<void(CUI_GachaChannel* pChannel)>	onSelect = {};
	}CHANNEL_DESC;

private:
	CUI_GachaChannel() {}
	CUI_GachaChannel(const CUI_GachaChannel& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_GachaChannel() DEFAULT;

public:
	const CUI_GachaPage::CHANNEL& Get_Channel() { return m_eChannel; }

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
	CUI_GachaPage::CHANNEL m_eChannel = {};

	_bool			m_isSelected = {};
	function<void(CUI_GachaChannel* pChannel)>	m_onSelect = {};

	_float			m_fLerpTimer = {};
	_float			m_fLerpSpeed = { 4.f };
	Vector4			m_vLerpColors[2] = {};

	class CUI_Object* m_pOutline = {};
	class CUI_Object* m_pPattern = {};
	class CSprite2D* m_pIcon = {};
	class CTextSlot* m_pLabel = {};
	class CButtonUI* m_pButton = {};

private:
	void Cache();

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END