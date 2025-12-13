#pragma once

NS_BEGIN(Engine)

enum class CamType // 카메라가 "무슨 용도"로 쓰이는지에 대한 분류, 게임 로직/상태에 따라 어떤 카메라를 활성화할지 결정할 때 사용.
{
	GamePlay,	// 플레이 중 메인 카메라 (입력/상태머신 영향 받음) -> 주로 전투
	Cinematic,	// 컷씬/연출용 카메라 (키프레임 시퀀스 재생)
	Debug       // 디버그 전용 자유 시점 카메라
};
enum class CamRigType // 카메라가 "어떻게 움직이는지"에 대한 분류, 같은 CamType 안에서도 RigType 에 따라 업데이트 로직이 달라짐.
{
	Free,	  // 키프레임(or 입력) 기반 pos/rot 를 그대로 보간해서 사용.
	Follow,	  // 타겟을 따라가며 일정 거리/오프셋 유지.
	Rail,	  // 미리 정의된 레일(경로)을 따라 이동.
	Orbit,	  // 타겟을 중심으로 공전하듯 빙글 도는 카메라.
	Fixed     // 완전히 고정된 카메라 (pos/rot 변하지 않음).
};
enum class CamSequenceMarkerType // 시퀀스 타임라인 위에 찍는 "트리거 이벤트" 종류. 카메라 움직임이 아니라, 특정 시점에서 발동할 연출/이벤트
{
	CamShake,        // 카메라 흔들기 (amp/freq/dur 등..)
	// 일단은 카메라 셰이크만 추가..
};
enum class CamPlaybackMode // 시퀀스 "끝까지 재생된 다음에" 어떻게 할지에 대한 규칙 (주로 Menu 쪽에서 사용예정)
{
	Once,
	Loop,
	PingPong, // 앞->뒤->앞 왕복 재생
};
enum class CamProjType
{
	Perspective,  // 원근투영
	Orthographic, // 직교투영
};
//enum class CamBlendType // 카메라 A에서 B로 갈아탈 때
//{
//	Cut,    // 바로 전환
//	Smooth, // 일정 시간 동안 부드럽게 블렌딩
//};

NS_END