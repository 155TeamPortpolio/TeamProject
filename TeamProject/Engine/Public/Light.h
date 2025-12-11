#pragma once
#include "Component.h"
NS_BEGIN(Engine)
class ENGINE_DLL CLight :
	public CComponent
{
private:
	CLight();
	CLight(const CLight& rhs);
	virtual ~CLight() DEFAULT;
public:
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(COMPONENT_DESC* pArg) override;
public:
	void Render_GUI();
	void Set_Desc(const LIGHT_DESC& desc, LIGHT_TYPE eType);
	_float4 Get_Position();

	LIGHT_DESC* Get_Desc();
	LIGHT_TYPE Get_Type()	{return m_eType;}

private:
	_int m_ID = {-1};
	LIGHT_TYPE m_eType = { LIGHT_TYPE::DIRECTIONAL};
	LIGHT_DESC m_Light = {}; 
public:
	static CLight* Create(const LIGHT_DESC& desc);
	static CLight* Create();
	virtual CComponent* Clone() override;
	virtual void Free() override;
};

NS_END