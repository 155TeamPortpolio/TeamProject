#pragma once
#include "GameObject.h"
NS_BEGIN(Engine)
class ENGINE_DLL CUI_Object abstract : public CGameObject
{
	//		명칭, 스케일 배율
	//		사이즈는 픽셀
	//		
	//		피봇은 트랜스폼 기준점
	//		
	//		앵커는 부모 기준 각
	//		앵커 오프셋 나의fxfy

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

	virtual void Enter_Hover() {}
	virtual void Exit_Hover() {}
	virtual void OnClick() {}

	void Set_Size(_float2 size) { m_vSize= size; } 
	_bool Size_To(_fvector size, _float Speed);
	_bool Move_To(_fvector size, _float Speed);
	_bool Rotate_To(_float rad, _float Speed);

public:
	void Render_GUI() override;

public:
	void Update_UITransform();
	void Set_LeftTop(_float2 desiredLT);
	void Rotate_Left(_float _radian);
	/*Get Size*/
	_float2 Get_PxSize() { return m_vSize * m_vScale; }
	_float2 Half_PxSize() { return Get_PxSize() * 0.5f; }
	_float2 Get_RectTopLeft_Screen() ;


	// Screen anchors
	_float2 LT(_float x = 0.f, _float y = 0.f) { return Get_Point_Screen({ 0.f,   0.f }, x, y); }
	_float2 LC(_float x = 0.f, _float y = 0.f) { return Get_Point_Screen({ 0.f,   0.5f }, x, y); }
	_float2 LB(_float x = 0.f, _float y = 0.f) { return Get_Point_Screen({ 0.f,   1.f }, x, y); }

	_float2 CT(_float x = 0.f, _float y = 0.f) { return Get_Point_Screen({ 0.5f,  0.f }, x, y); }
	_float2 Center(_float x = 0.f, _float y = 0.f) { return Get_Point_Screen({ 0.5f, 0.5f }, x, y); }
	_float2 CB(_float x = 0.f, _float y = 0.f) { return Get_Point_Screen({ 0.5f,  1.f }, x, y); }

	_float2 RT(_float x = 0.f, _float y = 0.f) { return Get_Point_Screen({ 1.f,   0.f }, x, y); }
	_float2 RC(_float x = 0.f, _float y = 0.f) { return Get_Point_Screen({ 1.f,   0.5f }, x, y); }
	_float2 RB(_float x = 0.f, _float y = 0.f) { return Get_Point_Screen({ 1.f,   1.f }, x, y); }

	// Local anchors (parent anchor 기준 좌표계)
	_float2 Local_LT(_float x = 0.f, _float y = 0.f) { return Get_Point_Local({ 0.f,  0.f }, x, y); }
	_float2 Local_LC(_float x = 0.f, _float y = 0.f) { return Get_Point_Local({ 0.f,  0.5f }, x, y); }
	_float2 Local_LB(_float x = 0.f, _float y = 0.f) { return Get_Point_Local({ 0.f,  1.f }, x, y); }

	_float2 Local_CT(_float x = 0.f, _float y = 0.f) { return Get_Point_Local({ 0.5f, 0.f }, x, y); }
	_float2 Local_Center(_float x = 0.f, _float y = 0.f) { return Get_Point_Local({ 0.5f,0.5f }, x, y); }
	_float2 Local_CB(_float x = 0.f, _float y = 0.f) { return Get_Point_Local({ 0.5f, 1.f }, x, y); }

	_float2 Local_RT(_float x = 0.f, _float y = 0.f) { return Get_Point_Local({ 1.f,  0.f }, x, y); }
	_float2 Local_RC(_float x = 0.f, _float y = 0.f) { return Get_Point_Local({ 1.f,  0.5f }, x, y); }
	_float2 Local_RB(_float x = 0.f, _float y = 0.f) { return Get_Point_Local({ 1.f,  1.f }, x, y); }


	void Align_To(ANCHOR anchor);
	void Set_Pivot(_float2 newPivot);

public:
	void Set_OnSystem(const string& Level, _int systemIndex) { m_Level = Level; m_SystemIndex = systemIndex; }
	_int Get_SystemIndex() { return m_SystemIndex; }
	string Get_SystemLevel() { return m_Level; }
	_uint Get_Priority() { return m_iPriority; };
	void Set_Priority(_uint priority) { m_iPriority = priority; }

public:
	void Set_Clickable(_bool isClickable) { m_isClickable = isClickable; }
	_bool Is_Clickable() { return m_isClickable; }

private:
	_float2 Get_Point_Screen(_float2 anchor, _float x = 0.f, _float y = 0.f);
	_float2 Get_Point_Local(_float2 anchor, _float x = 0.f, _float y = 0.f);
	_float2 Calc_AnchorPoint();

protected:
	/*스크린 사이즈*/
	_float2 m_WinSize = {};
	/*부모 기준의 앵커 오프셋*/
	_float2 m_vAnchorOffset = {};
	/*스크린 기준의 오프셋*/
	_float2 m_vScreenOffset = {};
	ANCHOR m_eAnchor = ANCHOR::Left|ANCHOR::Top; 

	/*픽셀 상의 크기*/
	_float2 m_vSize = {};

	/*좌상단 위치*/
	_float2 m_vLeftTop = {};

	/*크기 배율*/
	_float2 m_vScale = {};

	/*트랜스폼 기준점 - 내부 좌표 기준 : 0~1, 0~1 */
	_float2 m_vPivot= {};

	_float m_fRadian = {};
	_uint m_iPriority = {UINT_MAX};

	string m_Level = {};
	_int m_SystemIndex = {-1};

	_bool m_isClickable = {};

public:
	virtual void Free() override;
};
NS_END
