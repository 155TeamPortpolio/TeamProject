#pragma once
#include "ParticleNode.h"
#include "EffectContainer_Edit.h"
#include "ParticleSystem.h"

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
    void Import(nlohmann::ordered_json& json);
    void Export(nlohmann::ordered_json& json);

public:
    static CParticleNode_Edit* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;

private:
    CEffectContainer_Edit::EFFECT_EDIT_CONTEXT* m_pContext = nullptr;

    void AddTextures();
    void SetUp_ParticleEffect();

    string m_TextureKey{};
    string m_TexturePath{};

    /*Main Params*/
    _uint m_iRGBMaskMode{};
    CParticleSystem::SPAWN_SHAPE m_eSpawnShape{};
    CParticleSystem::COLOR_MODE m_eColorMode{};
    _bool m_IsWorld = true;
    _uint m_iBurstCount{};

    _float m_fSpawnPerSec{};
    _float m_fSpawnAcc{};
    _uint m_iSpawnParticleCount{};
    _uint m_iMaxSpawnParticleCount{};

    _float2 m_vStartSpeed{};
    _float2 m_vStartLifeTime{ 1.f,1.f };
    _float2 m_vStartSize{ 1.f,1.f };

    _float3 m_vCenter{};
    _float3 m_vHalfBox{ 1.f,1.f,1.f };
    _float m_fRaidus{};

    /*Gravity mode*/
    _bool m_UseGravity = false;
    _float m_fGravityScale{};

    /*Module Params*/
    _float m_fDampScale{};

    _float2 m_vStartScale{ 1.f,1.f };
    _float2 m_vEndScale{ 1.f,1.f };

    _float4 m_vStartColor{ 1.f,1.f,1.f,1.f };
    _float4 m_vEndColor{ 1.f,1.f,1.f,1.f };

    _float4 m_vAlphaKey{ 1.f,1.f,1.f,1.f };
    _float2 m_vRatio{ 0.3f,0.6f };

    _bool m_IsRandomFrameIndex = false;
    _bool m_IsParticleAnimated = false;
    _uint m_iCol{ 1 };
    _uint m_iRow{ 1 };
    _uint m_iMaxFrameIndex{ 1 };

    _float3 m_vStrength{};
    _float3 m_vFrequency{};
    _float3 m_vScrollSpeed{};

};
NS_END
