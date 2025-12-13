#pragma once

#include "CamSequencePlayer.h"

NS_BEGIN(CameraTool)

struct CamToolTarget
{
	CamSequenceDesc*   sequence{};
	_uint              nextKeyId = 1;

    CamObj*            captureCamObj{};
    CCamera*           captureCamComp{};
    CamSequencePlayer* player{};
};
struct CamToolEditState
{
    _uint selectedCam    = 0;
    _int  selectedKeyIdx = -1;

    _bool recording = false;
    _bool playing   = false;
    _bool loop      = false;

    _float curTime   = 0.f;
    _float endTime   = 10.f;
    _float timeScale = 1.f;

    _float editTime = 0.f;
    _float editFov  = 60.f;
    _float editRoll = 0.f;
};
struct CamToolKeyPolicy
{
    _float defaultStepTime = 0.5f;
    _float mergeEpsilon    = 1e-3f;
};

NS_END