#pragma once

#include "Component.h"
#include "Transform.h"
#include "CamUtil.h"

NS_BEGIN(Engine)
class ENGINE_DLL CCamera final : public CComponent
{
private:
	CCamera() DEFAULT;
	CCamera(const CCamera& rhs) : CComponent(rhs) {}
	~CCamera() DEFAULT;

public:
	HRESULT Initialize_Prototype()           override;
	HRESULT Initialize(COMPONENT_DESC* pArg) override;

public:
	Matrix      Get_ViewMatrix() const { return m_transform->Get_InverseWorldMatrix(); }
	Matrix      Get_ProjMatrix() const;			
	CTransform* Get_Transform()  const { return m_transform; }
	_vector     Get_Pos()        const { return m_transform->Get_Pos(); }
	_float      Get_FOV()        const { return m_fov;      }
	_float      Get_Near()       const { return m_zNear;    }
	_float      Get_Far()        const { return m_zFar;     }
	_float      Get_Aspect()     const { return m_aspect;   }
	CamProjType Get_ProjType()   const { return m_projType; }

	void        Set_FOV(_float _fov)                { m_fov      = _fov;      }
	void        Set_Far(_float _zFar)               { m_zFar     = _zFar;     }
	void        Set_Near(_float _zNear)             { m_zNear    = _zNear;    }
	void        Set_Aspect(_float _aspect)          { m_aspect   = _aspect;   }
	void        Set_ProjType(CamProjType _projType) { m_projType = _projType; }
	void        Set_Lens(_float _fov, _float _aspect, _float _zNear, _float _zFar);

	_bool       Lerp_FOV(_float dst, _float dt);

public:
	void        Render_GUI();

private:
	CTransform* m_transform{};
	_float      m_fov{};
	_float      m_zNear{};
	_float      m_zFar{};
	_float      m_aspect{};

	CamProjType m_projType  = CamProjType::Perspective;
	_float      m_orthoSize = 10.f;

public:
	static CCamera* Create();
	virtual CComponent* Clone();
	void Free() override;
};
NS_END