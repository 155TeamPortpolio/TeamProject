#pragma once

NS_BEGIN(Engine)
struct CamPose;
// 입력, 타겟, 키프레임 등을 보고 "이번 프레임 카메라가 어디에 있어야 하고, 어디를 봐야 하고, FOV가 얼마여야 하는지" 계산해서 CamPose로 뽑아줌
// RenderCam, Source (RenderCam을 어떻게 움직일지 결정하는 후보들), Free, Third, First, Sequence 같은 모드가 각각 Source가 됨.
class ICamPoseSource abstract 
{
public:
	virtual ~ICamPoseSource() DEFAULT;

public:
	virtual _bool   IsEnabled() const PURE;
	virtual CamPose EvaluatePose(_float dt) PURE;

public:
	virtual void OnEnter() {}
	virtual void OnExit()  {}
};

NS_END