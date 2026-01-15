#pragma once

#include "GameObject.h"
#include "Component.h"
#include "CamUtil.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCamera final : public CComponent
{
private:
	CCamera() DEFAULT;
	CCamera(const CCamera& rhs) : CComponent(rhs) {}
	~CCamera() DEFAULT;

public:
	HRESULT Initialize_Prototype()           override { return S_OK; }
	HRESULT Initialize(COMPONENT_DESC* pArg) override;

public:
	Matrix         Get_ViewMatrix()  const;
	Matrix         Get_ProjMatrix()  const;			
	_vector        Get_Pos()         const;
	_float         Get_FOV()         const { return m_lens.fov;    }
	_float         Get_Near()        const { return m_lens.zNear;  }
	_float         Get_Far()         const { return m_lens.zFar;   }
	_float         Get_Aspect()      const { return m_lens.aspect; }
	CamProjType    Get_ProjType()    const { return m_projType;    }
	OBJECT_HANDLE  Get_OwnerHandle() const { return m_pOwner->Get_Handle(); }
	_float         Get_OrthoSize()   const { return m_orthoSize; }

	void           Set_FOV(_float fov)                { m_lens.fov    = fov;      }
	void           Set_Far(_float zFar)               { m_lens.zFar   = zFar;     }
	void           Set_Near(_float zNear)             { m_lens.zNear  = zNear;    }
	void           Set_Aspect(_float aspect)          { m_lens.aspect = aspect;   }
	void           Set_ProjType(CamProjType projType) { m_projType    = projType; }
	void           Set_Lens(_float fov, _float aspect, _float zNear, _float zFar);
				   
	_bool          Lerp_FOV(_float dst, _float dt);

	virtual void   Render_GUI() override;

private:
	Lens        m_lens{};
	CamProjType m_projType = CamProjType::Perspective;
	_float      m_orthoSize = 10.f;
	Vector3     m_viewOffset{};

public:
	static  CCamera*    Create();
	virtual CComponent* Clone() { return new CCamera(*this); }
};
NS_END