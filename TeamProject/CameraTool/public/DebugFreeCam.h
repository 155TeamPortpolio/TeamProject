#pragma once

NS_BEGIN(CameraTool)

class CDebugFreeCam final : public CamObj
{
private:
	CDebugFreeCam() = default;
	CDebugFreeCam(const CDebugFreeCam& rhs) : CamObj(rhs) {}
	virtual ~CDebugFreeCam() = default;

public:
	HRESULT Initialize_Prototype()           override;
	HRESULT Initialize(INIT_DESC* pArg)      override;
										     
	void    Priority_Update(_float dt)       override;
	void    Update(_float dt)                override;
	void    Late_Update(_float dt)           override;
										     
	void    Render_GUI()                     override;

	void    SetControlEnabled(_bool enabled) override;

private:
	void    ApplyRotation(_float dt);
	void    SyncRotationFromTransform();

private:
	_float     moveSpeed      = 10.f;
	_float     sensitivity    = 0.5f;
	_vector2   rotDegTarget   = {};
	Quaternion rotQuatCurrent = Quaternion::Identity;
	Quaternion rotQuatTarget  = Quaternion::Identity;
	_float     rotSmoothSpeed = 15.f;
	_bool      controlEnabled = true;

public:
	static CDebugFreeCam* Create();
	CGameObject* Clone(INIT_DESC* pArg) override;
	virtual void Free() override;
};

NS_END