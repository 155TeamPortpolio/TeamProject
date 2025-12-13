#pragma once

NS_BEGIN(Engine)

template<typename T>
struct Keyframe
{
	float time = 0.f;
	T value;
};
struct Campose
{
	_vector3   pos{};
	Quaternion rot{};
	float      fov{};
};
struct CamKeyFrame // 한 스퀀스 안에서 특정 시점의 카메라 상태를 표현하는 단일 키프레임 데이터
{
	_float time = 0.f;

	_vector3 pos{};
	_vector3 look{};
	_float   roll{};
	_float   fov{};
};
struct CamSequenceMarker // 시퀀스 타임라인 위에서 특정 시간에 한 번 발동되는 카메라 연출 트리거 정보
{                        // CamShake 같은 효과를 "언제, 어떤 파라미터로 실행할지"를 저장하는 이벤트 데이터
	_float time = 0.f;

	CamSequenceMarkerType markerType = CamSequenceMarkerType::CamShake;

	_float param0;
	_float param1;
	_float param2;
};
struct CamSequenceDesc // 하나의 카메라 시퀀스(컷씬/연출)를 정의하는 전체 프리셋 데이터.
{                      // 어떤 타입/리그의 카메라인지와, 그 스퀀스를 구성하는 키프레임/마커 목록을 가짐.
	string          name; 

	CamType         camType  = CamType::Cinematic;
	CamRigType      rigType  = CamRigType::Free;
	CamProjType     projType = CamProjType::Perspective;

	CamPlaybackMode playbackMode = CamPlaybackMode::Once;

	vector<CamKeyFrame>       keyframes;
	vector<CamSequenceMarker> markers;

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