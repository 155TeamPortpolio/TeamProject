#pragma once
#include "UI_WorldToScreen.h"

NS_BEGIN(Engine)
class CTextSlot;
class CCharacterController;
NS_END

NS_BEGIN(Client)

class CUI_SpeechBubble final : public CUI_WorldToScreen
{
public:
	typedef struct tagSpeechBubbleDesc : public UI_DESC {
		wstring strSpeech = {};
		_float3 vPosition = {};
	}SPEECHBUBBLE_DESC;

private:
	enum class STATE_VISIBLE { VISIBLE, INVISIBLE, END };

private:
	CUI_SpeechBubble() {}
	CUI_SpeechBubble(const CUI_SpeechBubble& rhs) : CUI_WorldToScreen(rhs) {}
	virtual ~CUI_SpeechBubble() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }

private:
	CUI_Object* m_pBg = {};
	class CTextSlot* m_pTextSlot = { nullptr };
	 
	class CCharacterController* m_pCCT = { nullptr }; 
	_float3 m_vPosition = {};
	_float3 m_vOffset = { 0.f, 0.2f, 0.f};

	STATE_VISIBLE		m_eVisible = { STATE_VISIBLE::INVISIBLE };

	static constexpr _float	m_fRadius = { 3.f };// { 6.f };
	static constexpr _float2 m_vPadding = { 48.f, 24.f };

private:
	void Cache();

	void Set_SpeechBubble(wstring strSpeech);
	void Update_Visible(STATE_VISIBLE eNewState);

	STATE_VISIBLE CalcState_ByDistance();

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END