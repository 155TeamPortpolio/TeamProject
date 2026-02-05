#pragma once
#include "EffectNode.h"
#include "MeshEffectModule.h"

NS_BEGIN(Engine)
class ENGINE_DLL CMeshNode :
    public CEffectNode
{
protected:
    enum class MODE { UV_ANIMATION, SPRITE_ANIAMTION, END };

protected:
    CMeshNode();
    CMeshNode(const CMeshNode& rhs);
    virtual ~CMeshNode() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void Awake() override;
    void Priority_Update(_float dt) override;
    void Update(_float dt) override;
    void Late_Update(_float dt) override;
    void Render_GUI()override;

public:
    void Play() override;
    void Stop() override;

public:
    static CMeshNode* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;

protected:
    void Bind_Textures();
    void Reset();
    void Update_TextureSlotModule(_float dt);
    void Update_ColorModule(_float dt);
    void Update_ScaleModule(_float dt);
    void Update_UVAnimationModule(_float dt);
    void Update_SpriteAnimationModule(_float dt);
    void Update_DissolveModule(_float dt);
    void Update_BloomModule(_float dt);
    void Update_NoiseModule(_float dt);
    void Update_MaskModule(_float dt);
    void Update_DistortionModule(_float dt);
    void Update_GradientModule(_float dt);
    void Bind_Params();

    void Update_PendingStop();

    /*Default Params*/
    _bool m_IsPendingStop = false;
    _float m_fPendingDuration = 0.2f;
    _float m_fPendingElapsedTime{};

    _float m_fScreenWidth{};
    _float m_fScreenHeight{};

    _float m_fProgress{};  //전체 진행도
    string m_DiffuseTextureTag{};
    string m_DissolveTextureTag{};
    string m_NoiseTextureTag{};
    string m_MaskTextureTagA{};
    string m_MaskTextureTagB{};
    string m_DistortionTextureTag{};
    string m_DistortionMaskTextureTag{};
    string m_GradientTextureTag{};

    /*-----Modules-----*/
    TEXTURE_SLOT_MODULE m_TextureSlotModule{};
    COLOR_MODULE m_ColorModule{};
    SCALE_MODULE m_ScaleModule{};
    UV_ANIMATION_MODULE m_UVAnimaitonModule{};
    SPRITE_ANIMATION_MODULE m_SpriteAnimationModule{};
    DISSOLVE_MODULE m_DissolveModule{};
    BLOOM_MODULE m_BloomModule{};
    NOISE_MODULE m_NoiseModule{};
    MASK_MODULE m_MaskModule{};
    DISTORTION_MODULE m_DistortionModule{};
    GRADIENT_MODULE m_GradientModule{};
};
NS_END
