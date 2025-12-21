#pragma once
#include "AnimationTool_Defines.h"
#include "Animator3D.h"

NS_BEGIN(Engine)
class CAnimationClip;
NS_END

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
    virtual void Render_GUI() override;
    virtual void Update_Animation(_float fTrackPosition) override;

public:
    vector<CAnimationClip*>* Get_Clips();

public:
    static CAnimator3DEX* Create();
    virtual CComponent* Clone();
    virtual void Free() override;
};

NS_END