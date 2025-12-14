#pragma once
#include "AnimationTool_Defines.h"
#include "Animator3D.h"

NS_BEGIN(AnimTool)

class CAnimator3DEX final
	: public CAnimator3D
{
private:
    CAnimator3DEX();
    CAnimator3DEX(const CAnimator3DEX& rhs);
    ~CAnimator3DEX() DEFAULT;

public:



private:
    _int m_iNumAnimLayer = { -1 }; //애니매이션 레이어까지 등록을 해줘야 사용이 가능하도록

public:
    static CAnimator3DEX* Create();
    virtual CComponent* Clone();
    void Free() override;
};

NS_END