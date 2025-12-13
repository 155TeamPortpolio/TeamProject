#pragma once
#include "EffectNode.h"

NS_BEGIN(Engine)
class ENGINE_DLL CParticleNode :
    public CEffectNode
{
protected:
    CParticleNode();
    CParticleNode(const CParticleNode& rhs);
    virtual ~CParticleNode() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void Awake() override;
    void Priority_Update(_float dt) override;
    void Update(_float dt) override;
    void Late_Update(_float dt) override;

public:
    static CParticleNode* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;

protected:

};
NS_END
