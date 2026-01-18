#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_FieldHUD final : public CUI_Object
{
private:
	enum Child { GRADIENTF, GRADIENTJ, ACTION, END };	/*임시로 F, J 그라디언트 */

private:
	CUI_FieldHUD() {}
	CUI_FieldHUD(const CUI_FieldHUD& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_FieldHUD() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }

private:
	UI_HANDLE		m_handles[ENUM(Child::END)];

	_bool			m_isTemp = {};

private:
	void Ready_PartObjects();
	void Add_PartObject(const string& strPrototypeTag, const string& strInstanceName, UI_HANDLE* pHandleOut, _float2 vOffset);

	void Set_Animation(Child child, _int iIndex);

	//template<typename Func>
	//void ForChild(Child child, Func&& func);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END

//template<typename Func>
//inline void CUI_FieldHUD::ForChild(Child child, Func&& func)
//{
//	auto& handle = m_handles[ENUM(child)];
//	if (!handle.isValid())
//		return;
//
//	func(handle.Get());
//}