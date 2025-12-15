#pragma once

NS_BEGIN(CameraTool)

struct CamOrbitState
{
	_bool    initialized     = false;
	_bool    lookAtCenter    = true;
	_bool    useCustomCenter = false;

	_vector3 center = _vector3{ 0.f, 0.f, 0.f };
	_vector3 axis   = _vector3{ 0.f, 1.f, 0.f };

	_float   distance      = 5.f;
	_float   distanceSpeed = 5.f;

	_float   angleDeg        = 0.f;
	_float   angularSpeedDeg = 120.f;

	_vector3 targetPos = _vector3{ 0.f, 0.f, 0.f };
	_float   offsetY   = 0.f;
};

NS_END