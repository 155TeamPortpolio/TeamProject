#pragma once
#include "Engine_Defines.h"

enum class ANIM_LAYER_STATE { OVERRIDE, INTERRUPT, ADDITIVE };
enum class ANIM_BLEND_STATE {};
enum class ANIM_EVENT_STATE { EFFECT, SOUND };

//NLOHMANN_JSON_SERIALIZE_ENUM(ANIM_EVENT_STATE, {
//    {ANIM_EVENT_STATE::Sound, "Sound"},
//    {ANIM_EVENT_STATE::Effect, "Effect"}
//}) 

typedef struct AnimationEvent {
    float   fEventTime;     //이벤트 작동 시간
    int     iEventState;    //ANIM_EVENT_STATE
    string  EventTag;       //이벤트 태그 
}ANIM_EVENT;

typedef struct AnimationClip {
    string ClipTag; 
    vector<ANIM_EVENT> Events;
}ANIM_CLIP;

typedef struct AnimationNode {
    string NodeTag;     //노드 이름
    int    iClipIndex;  //클립 인덱스
    float  fPlaySpeed;  //재생속도
    bool   bisLoop;     //반복여부
}ANIM_NODE;

typedef struct AnimationTransition { // to와 from을 둘 다 충족시킬시 변경
    string toNode;      //현재 재생중인 노드
    string fromNode;    //에서 바꿀노드
    int    iBlendState;  //ANIM_BLEND_STATE
}ANIM_TRANSIT;

typedef struct AnimationLayer {
    //레이어 데이터
    string  LayerTag;                                        // 레이어 이름
    int   iLayerIndex = { -1 };                              // 레이어 인덱스 (낮을수록 베이스)
    int   iLayerType = { ENUM(ANIM_LAYER_STATE::OVERRIDE) };  // 레이어 타입
    int   iStartBoneIndex = { -1 };                          // 어느 본부터 적용해 내려갈지
    int   iDefaultNodeIndex = { 0 };                          // 기본 노드

    //노드 데이터
    int iNumNodes;                      //노드 개수
    vector<ANIM_NODE> AnimNode;         //노드 데이터
    int iNumTransition;                 //트랜지션 개수
    vector<ANIM_TRANSIT> TransitNode;   //트랜지션 데이터
}ANIM_LAYER;


typedef struct AnimationGraph {
    string              GraphTag;           //파일이름
    int                 NumLayers = { 0 };  //레이어 개수
    vector<ANIM_LAYER>  Layers;             //레이어 데이터
}ANIM_GRAPH;
