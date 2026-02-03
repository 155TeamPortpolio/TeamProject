#pragma once

#include "CamSequencePlayer.h"

NS_BEGIN(CameraTool)

struct CamToolTarget
{
    CamSequenceDesc*    sequence{};
    _uint               nextKeyId = 1;

    CCamObj*            captureCamObj{};
    CCamera*            captureCamComp{};
    CCamSequencePlayer* player{};

    OBJECT_HANDLE       spaceRefHandle{};
};
struct CamToolEditState
{
    _uint  selectedCam = 0;
    _int   selectedKeyIdx = -1;
           
    _bool  recording = false;
    _bool  playing = false;
    _bool  loop = false;

    _float curTime = 0.f;
    _float endTime = 10.f;
    _float timeScale = 1.f;

    _float editTime = 0.f;
    _float editFov = 60.f;
    _float editRoll = 0.f;

    bool playAllLink = false;
    OBJECT_HANDLE playAllRefHandle{};

    CamMoveConstraint moveConstraint = CamMoveConstraint::Free;
    CamOrbitState     orbit{};
};
struct CamToolKeyPolicy
{
    _float defaultStepTime = 0.5f;
    _float mergeEpsilon = 1e-3f;
};
struct PanelUIState
{
    bool  hidden    = false;
    float expandedH = 400.f;
    float slideY    = 0.f;
};
struct KeyframeListUIState
{
    _uint pendingDeleteKeyId = 0;

    string lastFileError{};
    bool requestOpenFileErrorPopup = false;

    char prefabNameBuf[128] = "DebugSequence";
    const CamSequenceDesc* lastSeqPtr = nullptr;
    bool nameEditing = false;

    string lastLoadedPath{};
    char   autoLoadPathBuf[260] = "";
    bool   didAutoLoadOnce = false;

    unordered_map<_uint, array<char, 64>> eventTagBufs{};
};
struct KeyframeEditorUIState
{
    _uint pendingTimeSelectedId = 0;
    float pendingTimeValue = 0.f;
    int   pendingOverwriteCount = 0;
    bool  requestOpenTimeCollisionPopup = false;

    _vector3 lookTargetPos{};
    bool     lookTargetInited = false;
};

NS_END