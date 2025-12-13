#pragma once
#include "ParticleNode.h"

NS_BEGIN(EffectTool)
class CParticleNode_Edit :
    public CParticleNode
{
private:
    CParticleNode_Edit();
    CParticleNode_Edit(const CParticleNode_Edit& rhs);
    virtual ~CParticleNode_Edit() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void Awake() override;
    void Priority_Update(_float dt) override;
    void Update(_float dt) override;
    void Late_Update(_float dt) override;

public:
    static CParticleNode_Edit* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;

private:

};
NS_END
