#pragma once

#include "DebugCamData.h"

NS_BEGIN(CameraTool)

class CDebugFreeCam final : public CCamObj
{
private:
	CDebugFreeCam() = default;
	CDebugFreeCam(const CDebugFreeCam& rhs) : CCamObj(rhs) {}
	virtual ~CDebugFreeCam() = default;

public:
	HRESULT Initialize_Prototype()                    override;
	HRESULT Initialize(INIT_DESC* pArg)               override;

	void    Priority_Update(_float dt)                override;
	void    Update(_float dt)                         override;
	void    Late_Update(_float dt)                    override;

	void    Render_GUI()                              override;

	void    SetControlEnabled(_bool enabled)          override;
	void    SetMoveConstraint(CamMoveConstraint mode) override;
	void    SetOrbitState(const CamOrbitState& next)  override;

	const   CamOrbitState& GetOrbitState() const { return orbit; }

private:
	void    ApplyRotation(_float dt);
	void    SyncRotationFromTransform();

private:
	_float            moveSpeed      = 5.f;
	_float            sensitivity    = 0.5f;
	_vector2          rotDegTarget   = {};
	Quaternion        rotQuatCurrent = Quaternion::Identity;
	Quaternion        rotQuatTarget  = Quaternion::Identity;
	_float            rotSmoothSpeed = 20.f;
	_bool             controlEnabled = true;

	CamMoveConstraint moveConstraint = CamMoveConstraint::Free;
	CamOrbitState     orbit{};

public:
	static CDebugFreeCam* Create();
	CGameObject* Clone(INIT_DESC* pArg) override;
	virtual void Free() override;
};

NS_END