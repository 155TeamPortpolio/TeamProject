#pragma once

#include "Transform.h"
#include "Camera.h"

NS_BEGIN(Engine)

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
	_bool       applyEnabled = true;
	CTransform* transform{};
	CCamera*    cam{};
};
struct CamPlayerEvalState
{
	_bool         dirty = true;
	CamEvaluator* evaluator{};
};

NS_END