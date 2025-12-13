#pragma once
#include "AnimationTool_Defines.h"
#include "Animator3D.h"

NS_BEGIN(AnimTool)

class CAnimation3DEX final
	: public CAnimator3D
{
private:
    struct AnimationLayer {
        //레이어 데이터
        _uint   LayerIndex;
        string  LayerTag;

        //노드 데이터
        _int NumNodes; //노드 개수
        _int CurNodeIndex = { -1 }; //현재 노드 인덱스(시작인덱스)
        _int

    };

    struct AnimationNode {
        _uint   NodeIndex;  //노드 인덱스
        string  NodeTag;    //노드 이름
        _int    ClipIndex;  //클립 인덱스
        _float  PlaySpeed;  //재생속도 (Playback Speed)
        _float  TrackPos;   //트랙포지션
        _bool   isLoop;     //반복여부
    };

private:
    CAnimation3DEX();
    CAnimation3DEX(const CAnimation3DEX& rhs);
    ~CAnimation3DEX() DEFAULT;

public:
    HRESULT Initialize_Prototype();
    HRESULT Initialize(COMPONENT_DESC* pArg);

private:
    _uint m_iNumAnimLayer = { -1 }; //애니매이션 레이어까지 등록을 해줘야 사용이 가능하도록
    AnimationLayer** m_ppAnimLayer = { nullptr };

public:
    static CAnimation3DEX* Create();
    virtual CComponent* Clone();
    void Free() override;
};

NS_END