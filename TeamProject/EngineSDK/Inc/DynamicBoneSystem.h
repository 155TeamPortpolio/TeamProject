#pragma once
#include "Base.h"
#include "Animator3D.h"

NS_BEGIN(Engine)

class ENGINE_DLL CDynamicBoneSystem : public CBase
{
protected:
    CDynamicBoneSystem();
    virtual ~CDynamicBoneSystem() DEFAULT;

public:
    HRESULT Initialize(class CAnimator3D* pAnimator);

public:
    HRESULT Create_Chain(AnimArg BoneArg);
    HRESULT Add_Chain();

protected:


protected:
    class CAnimator3D* m_pAnimator = { nullptr };
    vector<DYNAMIC_CHAIN> m_Chains;

public:
    static CDynamicBoneSystem* Create(class CAnimator3D* pAnimator);
    virtual void Free() override;
};

NS_END