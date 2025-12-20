#pragma once
#include "Engine_Defines.h"
#include "Json_Inc/json.hpp"

namespace Engine
{
    enum class CLIP_EVENT_TYPE { NOTIFY, EFFECT, SOUND };
    NLOHMANN_JSON_SERIALIZE_ENUM(CLIP_EVENT_TYPE,
    {
        {CLIP_EVENT_TYPE::NOTIFY, "Notify"},
        {CLIP_EVENT_TYPE::EFFECT, "Effect"},
        {CLIP_EVENT_TYPE::SOUND, "Sound"}
    })

    /* 애니매이션 클립 데이터 */
    typedef struct AnimationEvent {
        float           EventTime;  //이벤트 작동 시간
        CLIP_EVENT_TYPE EventType;  //이벤트 타입
        string          EventTag;   //이벤트 태그 
    }ANIM_EVENT;
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ANIM_EVENT, EventTime, EventType, EventTag);
    

    typedef struct AnimationClip {
        string              ClipTag; //애니매이션 이름
        vector<ANIM_EVENT>  Events;
    }ANIM_CLIP;
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ANIM_CLIP, ClipTag, Events);
    /* ------------------------ */
}