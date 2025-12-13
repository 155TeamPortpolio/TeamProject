#pragma once
#include "ParticleNode.h"
#include "EffectContainer_Edit.h"

NS_BEGIN(EffectTool)
class CParticleNode_Edit :
    public CParticleNode
{
public:
    typedef struct tagParticleNodeEditDesc : public GAMEOBJECT_DESC
    {
        CEffectContainer_Edit::EFFECT_EDIT_CONTEXT* pContext = nullptr;
    }PARTICLE_NODE_EDIT_DESC;
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
    void Render_GUI()override;
    void Play()override;

public:
    static CParticleNode_Edit* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;

private:
    CEffectContainer_Edit::EFFECT_EDIT_CONTEXT* m_pContext = nullptr;

    void AddTextures();
    void SetUp_ParticleEffect();
};
NS_END
