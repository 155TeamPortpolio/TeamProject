#pragma once
#include "Component.h"
NS_BEGIN(Engine)
class ENGINE_DLL CCamera final :
    public CComponent
{
private:
	CCamera();
	CCamera(const CCamera& rhs);
	~CCamera() DEFAULT;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(COMPONENT_DESC* pArg) override;

public:
	_float4x4 Get_ViewMatrix();
	_matrix Get_PureViewMatrix();
	_matrix Get_ProjMatrix();
	_float Get_Far() { return m_fFar; };
	_bool Lerp_FOV(_float dst, _float dt);
	void Set_FOV(_float fov) { m_fFov = fov; };
	void Set_Far(_float zfar) { m_fFar = zfar; };

public:
	void Render_GUI();

private:
	class CTransform* m_pTransform = { nullptr };
	_float m_fFov = {};
	_float m_fNear = {};
	_float m_fFar = {};
	_float m_fAspect = {};

public:
	static CCamera* Create();
	virtual CComponent* Clone();
	void Free() override;
};
NS_END
