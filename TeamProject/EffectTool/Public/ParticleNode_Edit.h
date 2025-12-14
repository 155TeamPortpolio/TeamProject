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

    /*Main Params*/
    _bool m_IsWorld = true;
    _bool m_IsLoop = false;
    _uint m_iBurstCount{};

    _float m_fSpawnPerSec{};
    _float m_fSpawnAcc{};
    _uint m_iSpawnParticleCount{};
    _uint m_iMaxSpawnParticleCount{};

    _float2 m_vStartSpeed{};
    _float2 m_vStartLifeTime{};
    _float2 m_vStartSizeMin{};
    _float2 m_vStartSizeMax{};
    _float3 m_vSpawnAreaMin{};
    _float3 m_vSpawnAreaMax{};

    /*Gravity mode*/
    _bool m_UseGravity = false;
    _float m_fGravityScale{};

    /*Module Params*/
    _float m_fDampScale{};

};
NS_END
