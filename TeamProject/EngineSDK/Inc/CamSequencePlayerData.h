#pragma once

#include "Transform.h"
#include "Camera.h"

NS_BEGIN(Engine)
class CCamPosPerSegmentEvaluator;
class CCamRotPerSegmentEvaluator;
class CCamFovPerSegmentEvaluator;

struct CamPlayerTarget
{
	const CamSequenceDesc* seq{};
};
struct CamPlayerPlaybackState
{
	_bool  playing   = false;
	_float playTime  = 0.f;
	_float timeScale = 1.f;
};
struct CamPlayerApplyState
{
	_bool         applyEnabled = true;
	CTransform*   transform{};
	CCamera*      cam{};
	OBJECT_HANDLE spaceRefHandle{};
};
struct CamPlayerEvalState
{
	_bool                       dirty = true;
	CCamEvaluator*              evaluator{};

	CCamPosPerSegmentEvaluator* pos{};
	CCamRotPerSegmentEvaluator* rot{};
	CCamFovPerSegmentEvaluator* fov{};
};

NS_END