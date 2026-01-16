#pragma once

#include "OrbitCam.h"
#include "FreeCam.h"
#include "SequenceCam.h"

NS_BEGIN(Engine)
class CCamSequencePlayer; class ICameraService; class IObjectService;
NS_END

NS_BEGIN(Client)
class CBattlePlayer; class CFieldCharacter; class CPlayer; 

enum class CamType { None, Free, Orbit, Sequence, End };
enum class CamReturnMode { None, SnapToEnd, RestorePrev };

struct CamSequenceRequestDesc
{
    _float blendInSec  = 0.25f;
    _float blendOutSec = 0.25f;
    _bool  resetTime   = true;

    CamReturnMode returnMode    = CamReturnMode::SnapToEnd;
    CamType       returnCamType = CamType::None;
};

struct CamDirectorSeqEntry
{
    filesystem::path       path{};
    CamSequenceDesc        seqDesc{};
    CamSequenceRequestDesc defaultReq{};
};

struct CamDirectorPlayingState
{
    _uint            handle = 0u;
    string           key{};
    _bool            active = false;
                     
    _bool            pendingStart = false;
    _float           blendInRemain = 0.f;
                     
    _float           defaultBlendOutSec = 0.25f;
                     
    CamReturnMode    returnMode    = CamReturnMode::SnapToEnd;
    CamType          returnCamType = CamType::None;
    OBJECT_HANDLE    returnCamHandle{};
                     
    Vector3          prevCamPos{};
    Quaternion       prevCamRot = Quaternion::Identity;

    OrbitCamSnapshot prevOrbit{};
};

using CamDirectorSeqMap     = unordered_map<string, CamDirectorSeqEntry>;
using CamDirectorCamHandles = array<OBJECT_HANDLE, ENUM(CamType::End)>;

NS_END