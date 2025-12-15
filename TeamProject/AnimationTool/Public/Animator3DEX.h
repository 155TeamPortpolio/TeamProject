#pragma once
#include "AnimationTool_Defines.h"
#include "Animator3D.h"
#include "AnimationLayout.h"

NS_BEGIN(AnimTool)

class CAnimator3DEX final
	: public CAnimator3D
{
private:
    CAnimator3DEX();
    CAnimator3DEX(const CAnimator3DEX& rhs);
    ~CAnimator3DEX() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(COMPONENT_DESC* pArg) override;

public:
    HRESULT Load_ClipMeta(const string& LevelKey, const string& ClipMetaKey);

private:
    

public:
    static CAnimator3DEX* Create();
    virtual CComponent* Clone();
    void Free() override;
};

NS_END