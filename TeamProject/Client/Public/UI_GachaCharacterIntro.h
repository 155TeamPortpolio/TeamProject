#pragma once
#include "UI_Object.h"
#include "UI_GachaPage.h"

NS_BEGIN(Engine)
class CVideoPlayer;
class CMFVideoDecoderBackend;
NS_END

NS_BEGIN(Client) 

class CUI_GachaCharacterIntro final : public CUI_Object
{
private:
	CUI_GachaCharacterIntro() {}
	CUI_GachaCharacterIntro(const CUI_GachaCharacterIntro& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_GachaCharacterIntro() DEFAULT;

public:
	void Play_Video(CUI_GachaPage::CHANNEL eChannel);

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }
	virtual void	UI_Active(void* pArg = nullptr) override;
	virtual void	UI_DeActive(void* pArg = nullptr) override;

private:
	CVideoPlayer* m_pPlayer = { nullptr };
	CMFVideoDecoderBackend* m_pDecoder = { nullptr };
	_uint64 m_startTimeSec = 0.0;
	_uint m_PlayerID = {};

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() override;
};

NS_END