#pragma once

NS_BEGIN(CameraTool)

class DebugFreeCam final : public CamObj
{
private:
	DebugFreeCam() = default;
	DebugFreeCam(const DebugFreeCam& rhs) : CamObj(rhs) {}
	virtual ~DebugFreeCam() = default;

public:
	HRESULT Initialize_Prototype()      override;
	HRESULT Initialize(INIT_DESC* pArg) override;

	void    Priority_Update(_float dt)  override;
	void    Update(_float dt)           override;
	void    Late_Update(_float dt)      override;

	void    Render_GUI()                override;

private:
	void ApplyRotation(_float dt);

private:
	_float moveSpeed   = 10.f;
	_float sensitivity = 0.5f;

	_vector2   rotDegTarget{ 0.f, 0.f };
	Quaternion rotQuatCurrent = Quaternion::Identity;
	Quaternion rotQuatTarget  = Quaternion::Identity;

	_float rotSmoothSpeed = 10.f;

public:
	static DebugFreeCam* Create();
	CGameObject* Clone(INIT_DESC* pArg) override;
	virtual void Free() override;
};

NS_END