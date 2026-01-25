#pragma once
#include "OrbitArcData.h"
#include "Engine_math.h"

NS_BEGIN(Engine)

struct CamPose
{
	_vector3   pos{};
	Quaternion rot  = Quaternion::Identity;
	float      fov  = 60.f;
	float      roll = 0.f;
};
struct CamBlendState
{
	_bool   blending = false;
	_float  time     = 0.f;
	_float  duration = 0.25f;

	CamPose from{};
	CamPose to{};
};
struct CamKeyFrame 
{
	_uint        keyId{}; 
	_float       time{};
			     
	_vector3     pos{};
	_vector3     look{};
	_float       roll{};
	_float       fov{};

	_bool        useCustomInterp = false;
	CamPosInterp outPosInterp    = CamPosInterp::Linear;
	CamRotInterp outRotInterp    = CamRotInterp::Slerp;
	CamFovInterp outFovInterp    = CamFovInterp::Linear;

	_bool        useCustomEase   = false;
	EaseType     outEase         = EaseType::None;

	string       eventTag;
};
struct CamSequenceDesc 
{ 
	string              name;

	CamProjType         projType = CamProjType::Perspective;
	CamPlaybackMode     playbackMode = CamPlaybackMode::Once;

	CamPosInterp        posInterp = CamPosInterp::Linear;
	CamRotInterp        rotInterp = CamRotInterp::Slerp;
	CamFovInterp        fovInterp = CamFovInterp::Linear;

	EaseType            segmentEase = EaseType::None;
	CamOrbitArcDesc     orbitArc{};

	CamSpace            space = CamSpace::World;

	vector<CamKeyFrame> keyframes;

	float               refAnimDurSec = 0.f;

	float GetDuration() const
	{
		if (keyframes.empty())
			return 0.f;

		return keyframes.back().time;
	}
};
struct CamKeySegment
{
	_uint  segmentIdx = 0;
	_float normalizedTime = 0.f;
};

NS_END