#pragma once

#include "Component.h"
#include "Transform.h"
#include "CamUtil.h"

NS_BEGIN(Engine)
template<typename T>
inline constexpr bool always_false_v = false;

class ENGINE_DLL CCamera final : public CComponent
{
private:
	CCamera() = default;
	CCamera(const CCamera& rhs) : CComponent(rhs) {}
	~CCamera() = default;

public:
	HRESULT Initialize_Prototype()           override;
	HRESULT Initialize(COMPONENT_DESC* pArg) override;

public:
	template<typename T>
	T           Get_ViewMatrix() const;
	_matrix     Get_ProjMatrix() const;
	CTransform* Get_Transform()  const { return transform; }
	_vector     Get_Pos()        const { return transform->Get_Pos(); }

	_float      Get_FOV()      const { return fov;    }
	_float      Get_Near()     const { return zNear;  }
	_float      Get_Far()      const { return zFar;   }
	_float      Get_Aspect()   const { return aspect; }
	CamProjType Get_ProjType() const { return projType; }

	void  Set_FOV(_float _fov)       { fov = _fov;}
	void  Set_Far(_float _zFar)      { zFar = _zFar; }
	void  Set_Near(_float _zNear)    { zNear = _zNear; }
	void  Set_Aspect(_float _aspect) { aspect = _aspect; }
	void  Set_ProjType(CamProjType _projType) { projType = _projType; }
	void  Set_Lens(_float _fov, _float _aspect, _float _zNear, _float _zFar);

	_bool Lerp_FOV(_float dst, _float dt);

public:
	void      Render_GUI();

private:
	CTransform* transform{};
	_float      fov{};
	_float      zNear{};
	_float      zFar{};
	_float      aspect{};
	
	CamProjType projType  = CamProjType::Perspective;
	_float      orthoSize = 10.f;

public:
	static CCamera* Create();
	virtual CComponent* Clone();
	void Free() override;
};

template<typename T>
inline T CCamera::Get_ViewMatrix() const
{
	if constexpr (is_same_v<T, _float4x4>)
		return transform->Get_InverseWorldMatrix();
	else if constexpr (is_same_v<T, _matrix>)
		return XMLoadFloat4x4(transform->Get_InverseWorldMatrix_Ptr());
	else
		static_assert(always_false_v<T>, "CCamera::GetViewMatrix: 지원하지 않는 타입입니다. ");
}
NS_END