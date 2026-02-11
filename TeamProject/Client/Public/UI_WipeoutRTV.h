#pragma once
#include "UI_Object.h"

NS_BEGIN(Engine)
class CMaterialInstance;
NS_END

NS_BEGIN(Client)

class CUI_WipeoutRTV final : public CUI_Object
{
private:
	enum CHILD { RAINBOW, END };
	inline static const string INSTANCENAMES[ENUM(CHILD::END)] = { "rainbow" };
	
private:
	CUI_WipeoutRTV() {}
	CUI_WipeoutRTV(const CUI_WipeoutRTV& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_WipeoutRTV() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }

private:
	_bool	m_isVisible = {};

private:
	class CUI_Object* m_pChildren[ENUM(CHILD::END)] = {};
	
	/* 커스텀 렌더타겟 관련 변수들 */
	_float4x4			m_ViewMatrix = {};			// 브러쉬 뷰
	_float4x4			m_ProjMatrix = {};			// 브러쉬 프로젝션

	class CMaterialInstance* m_pMtrlInst = {};

private:
	void Render_RT(ID3D11DeviceContext* pContext);

	void Render_Recursive(class CGameObject* pObj, ID3D11DeviceContext* pContext);

	void Cache();

	void Set_ChildAnimation(CHILD child, _int iIndex, _bool isPlayChild = false);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END