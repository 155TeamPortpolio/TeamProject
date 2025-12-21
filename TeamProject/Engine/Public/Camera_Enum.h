#pragma once

NS_BEGIN(Engine)
enum class CamPosInterp
{
	Linear, CatmullRom, Centripetal, BSpline, Hermite, Hold, OrbitArc
};
enum class CamRotInterp
{
	Slerp, Squad, Hold
};
enum class CamFovInterp
{
	Linear, Smooth, Hold
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
enum class CamSpace
{
	Local, World
};
NS_END