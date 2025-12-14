#pragma once
#include "GameObject.h"
NS_BEGIN(Engine)
class ENGINE_DLL CUI_Object abstract : public CGameObject
{
protected:
	CUI_Object();
	CUI_Object(const CUI_Object& rhs);
	virtual ~CUI_Object() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(INIT_DESC* pArg = nullptr)override;

	virtual void Pre_EngineUpdate(_float dt)override;
	virtual void Post_EngineUpdate(_float dt)override;

	virtual void Priority_Update(_float dt)override;
	virtual void Update(_float dt)override;
	virtual void Late_Update(_float dt)override;
public:
	/*활성 비활성에 대한 로직을 스스로*/
	virtual void UI_Active(void* pArg = nullptr) {};
	virtual void UI_DeActive(void* pArg = nullptr) {};
public :
	_uint Get_Priority() { return m_iPriority; };
	void Set_Priority(_uint priority) { m_iPriority = priority; }
	void Set_CenterPos(_float2 pos) { m_fLocalX = pos.x; m_fLocalY = pos.y; }
	void Set_Size(_float2 size) { m_fSizeX = size.x; m_fSizeY = size.y; }
	void Set_Size(_fvector size);

	_bool Size_To(_fvector size, _float Speed);
	_bool Move_To(_fvector size, _float Speed);
	_bool Rotate_To(_float rad, _float Speed);

	_float2 Get_CenterPos() { return{ m_fLocalX ,m_fLocalY }; }

public:
	void Render_GUI() override;

public:
	void Update_UITransform();
	void Rotate_Left(_float _radian);

	/*Get Size*/
	_float HalfX() { return m_fSizeX * 0.5f; }
	_float HalfY() { return m_fSizeY * 0.5f; }


	/*Get Anchor*/
	_float2 LT(_float x = 0.f, _float y = 0.f) { return { m_fWorldX - HalfX() + x, m_fWorldY - HalfY() + y }; }
	_float2 LC(_float x = 0.f, _float y = 0.f) { return { m_fWorldX - HalfX() + x, m_fWorldY + y }; }
	_float2 LB(_float x = 0.f, _float y = 0.f) { return { m_fWorldX - HalfX() + x, m_fWorldY + HalfY() + y }; }

	_float2 CT(_float x = 0.f, _float y = 0.f) { return { m_fWorldX + x , m_fWorldY - HalfY() + y }; }
	_float2 Center(_float x = 0.f, _float y = 0.f) { return { m_fWorldX + x,m_fWorldY + y }; }
	_float2 CB(_float x = 0.f, _float y = 0.f) { return { m_fWorldX + x , m_fWorldY + HalfY() + y }; }

	_float2 RT(_float x = 0.f, _float y = 0.f) { return { m_fWorldX + HalfX() + x, m_fWorldY - HalfY() + y }; }
	_float2 RC(_float x = 0.f, _float y = 0.f) { return { m_fWorldX + HalfX() + x, m_fWorldY + y }; }
	_float2 RB(_float x = 0.f, _float y = 0.f) { return   { m_fWorldX + HalfX() + x, m_fWorldY + HalfY() + y }; }

	/*Get Anchor*/
	_float2 Local_LT(_float x = 0.f, _float y = 0.f) { return { m_fLocalX - HalfX() + x, m_fLocalY - HalfY() + y }; }
	_float2 Local_LC(_float x = 0.f, _float y = 0.f) { return { m_fLocalX - HalfX() + x, m_fLocalY + y }; }
	_float2 Local_LB(_float x = 0.f, _float y = 0.f) { return { m_fLocalX - HalfX() + x, m_fLocalY + HalfY() + y }; }

	_float2 Local_CT(_float x = 0.f, _float y = 0.f) { return { m_fLocalX + x, m_fLocalY - HalfY() + y }; }
	_float2 Local_Center(_float x = 0.f, _float y = 0.f) { return { m_fLocalX + x,m_fLocalY + y }; }
	_float2 Local_CB(_float x = 0.f, _float y = 0.f) { return { m_fLocalX + x, m_fLocalY + HalfY() + y }; }

	_float2 Local_RT(_float x = 0.f, _float y = 0.f) { return { m_fLocalX + HalfX() + x, m_fLocalY - HalfY() + y }; }
	_float2 Local_RC(_float x = 0.f, _float y = 0.f) { return { m_fLocalX + HalfX() + x, m_fLocalY + y }; }
	_float2 Local_RB(_float x = 0.f, _float y = 0.f) { return   { m_fLocalX + HalfX() + x, m_fLocalY + HalfY() + y }; }


	_float2 Align_To(ANCHOR pivot, _float2 _pivot);

public:
	void Set_OnSystem(const string& Level, _int systemIndex) { m_Level = Level; m_SystemIndex = systemIndex; }
	_int Get_SystemIndex() { return m_SystemIndex; }
	string Get_SystemLevel() { return m_Level; }

protected:
	_float m_WinSizeX = {};
	_float m_WinSizeY = {};

	_float m_fLocalX = {};
	_float m_fLocalY = {};
	_float m_fSizeX = {};
	_float m_fSizeY = {};
	_uint m_iPriority = {UINT_MAX};

	_float m_fRadian = {};

	string m_Level = {};
	_int m_SystemIndex = {-1};

	_float m_fWorldX = {};
	_float m_fWorldY = {};

	_bool m_bAttachParent = { true };
public:
	virtual void Free() override;
};
NS_END
