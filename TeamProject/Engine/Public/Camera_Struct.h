#pragma once

NS_BEGIN(Engine)

struct CamPose
{
	_vector3   pos{};
	Quaternion rot = Quaternion::Identity;
	float      fov = 60.f;
};
struct CamBlendState
{
	_bool   blending = false;
	_float  time     = 0.f;
	_float  duration = 0.25f;

	CamPose from{};
	CamPose to{};
};
struct CamKeyFrame // 한 스퀀스 안에서 특정 시점의 카메라 상태를 표현하는 단일 키프레임 데이터
{
	_uint        keyId{}; // "배열이 재정렬/삭제/병합돼도 같은 키를 계속 가리키기 위한 고유 식별자"
	_float       time{};
			     
	_vector3     pos{};
	_vector3     look{};
	_float       roll{};
	_float       fov{};

	_bool        useCustomInterp = false;
	CamPosInterp outPosInterp    = CamPosInterp::Linear;
	CamRotInterp outRotInterp    = CamRotInterp::Slerp;
	CamFovInterp outFovInterp    = CamFovInterp::Linear;
};
struct CamSequenceDesc // 하나의 카메라 시퀀스(컷씬/연출)를 정의하는 전체 프리셋 데이터.
{                      // 어떤 타입/리그의 카메라인지와, 그 스퀀스를 구성하는 키프레임/마커 목록을 가짐.
	string              name; 
					    
	CamType             camType  = CamType::Cinematic;
	CamRigType          rigType  = CamRigType::Free;
	CamProjType         projType = CamProjType::Perspective;
					    
	CamPlaybackMode     playbackMode = CamPlaybackMode::Once;
					    
	CamPosInterp        posInterp = CamPosInterp::Linear;
	CamRotInterp        rotInterp = CamRotInterp::Slerp;
	CamFovInterp        fovInterp = CamFovInterp::Linear;

	vector<CamKeyFrame> keyframes;

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