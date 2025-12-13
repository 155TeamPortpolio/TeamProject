#pragma once
#include "Engine_Defines.h"

enum class ANIM_LAYER_STATE { OVERRIDE, INTERRUPT, ADDITIVE };
enum class ANIM_BLEND_STATE {};




typedef struct AnimationNode {
    string  NodeTag;    //노드 이름
    _int    ClipIndex;  //클립 인덱스
    _float  PlaySpeed;  //재생속도 (Playback Speed)
    _bool   isLoop;     //반복여부
}ANIM_NODE;

typedef struct AnimationTransition { // to와 from을 둘 다 충족시킬시 변경
    string toNode;      //현재 재생중인 노드
    string fromNode;    //에서 바꿀노드
}ANIM_TRANSIT;

typedef struct AnimationLayer {
    //레이어 데이터
    string  LayerTag;                                          // 레이어 이름
    _int    LayerIndex = { -1 };                               // 레이어 인덱스 (낮을수록 베이스)
    _uint   LayerType = { ENUM(ANIM_LAYER_STATE::OVERRIDE) };  // 레이어 타입
    _int    StartBoneIndex = { -1 };                           // 어느 본부터 적용해 내려갈지
    _uint   DefaultNodeIndex = { 0 };                          // 기본 노드

    //노드 데이터
    _int NumNodes;                //노드 개수
    vector<ANIM_NODE>  mAnimNode; //노드 데이터
    _int NumTransition;           //트랜지션 개수
    vector<ANIM_NODE>  mAnimNode; //트랜지션 데이터
}ANIM_LAYER;


typedef struct AnimationGraph {
    string              GraphTag;   //파일이름
    _uint               NumLayers;  //레이어 개수
    vector<ANIM_LAYER>  Layers;     //레이어 데이터
}ANIM_GRAPH;
