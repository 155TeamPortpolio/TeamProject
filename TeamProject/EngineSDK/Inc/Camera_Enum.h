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
enum class CamPlaybackMode 
{
	Once, Loop, PingPong,
};
enum class CamProjType
{
	Perspective, Orthographic, 
};
enum class CamSpace
{
	Local, World
};
enum class CamBoneMode
{
	Parent, LookAt
};

NS_END