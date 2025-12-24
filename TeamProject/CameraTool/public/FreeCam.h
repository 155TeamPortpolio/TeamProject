#pragma once

#include "DebugCamData.h"

NS_BEGIN(CameraTool)
class CFreeCam final : public CCamObj
{
private:
	CFreeCam() = default;
	CFreeCam(const CFreeCam& rhs) : CCamObj(rhs) {}
	virtual ~CFreeCam() = default;

public:
	virtual HRESULT Initialize_Prototype()                    override;
	virtual HRESULT Initialize(INIT_DESC* pArg)               override;
	virtual void    Priority_Update(_float dt)                override;
	virtual void    Update(_float dt)                         override {}
	virtual void    Late_Update(_float dt)                    override {}
	virtual void    Render_GUI()                              override;

	virtual void    SetControlEnabled(_bool enabled)          override;
	virtual void    SetMoveConstraint(CamMoveConstraint mode) override;
	virtual void    SetOrbitState(const CamOrbitState& next)  override;

	virtual const   CamOrbitState& GetOrbitState() const { return orbit; }

private:
	void    ApplyRotation(_float dt);
	void    SyncRotation();

private:
	_float            moveSpeed      = 5.f;
	_float            sensitivity    = 0.5f;
	_vector2          targetRotDeg   = {};
	Quaternion        curRot         = Quaternion::Identity;
	Quaternion        targetRot      = Quaternion::Identity;
	_float            rotSmoothSpeed = 20.f;
	_bool             controlEnabled = true;

	CamMoveConstraint moveConstraint = CamMoveConstraint::Free;
	CamOrbitState     orbit{};

public:
	static CFreeCam* Create();
	CGameObject* Clone(INIT_DESC* pArg) override;
	virtual void Free() override { __super::Free(); }
};
NS_END