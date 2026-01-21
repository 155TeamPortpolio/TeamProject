#pragma once
#include "UI_WorldToScreen.h"

NS_BEGIN(Engine)
class CTextSlot;
class CCharacterController;
NS_END

NS_BEGIN(Client)

class CUI_NameIndicator final : public CUI_WorldToScreen
{
public:
	typedef struct tagIndicatorDesc : public UI_DESC {
		wstring strName = {};
		class CCharacterController* pCCT = { nullptr };
	}INDICATOR_DESC;

private:
	enum CHILD { NAME, ARROWL, ARROWR, END };
	inline static const string INSTANCENAMES[ENUM(CHILD::END)] = { "name", "arrowL", "arrowR" };

	enum class STATE { HIDDEN, VISIBLE, INTERACTABLE, END };

private:
	CUI_NameIndicator() {}
	CUI_NameIndicator(const CUI_NameIndicator& rhs) : CUI_WorldToScreen(rhs) {}
	virtual ~CUI_NameIndicator() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }

private:
	CUI_Object*			m_pChildren[ENUM(CHILD::END)] = {};

	class CTextSlot*	m_pName = { nullptr };
	class CCharacterController* m_pCCT = { nullptr };
	wstring				m_strName = { };
	_float3				m_vPosition = {};

	STATE				m_eState = {};

	static constexpr _float	m_fRadius = { 5.f };
	static constexpr _float m_fPadding = { 5.f };

private:
	void Cache_Children();

	void Set_Name(const wstring& strName);
	void Update_State(STATE eNewState);

	STATE CalcState_ByDistance();

	_float2 Get_ChildSize(CHILD child);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END