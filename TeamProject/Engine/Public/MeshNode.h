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

public:
    static CMeshNode* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;

protected:
    void Update_TextureSlotModule(_float dt);
    void Update_ColorModule(_float dt);
    void Update_ScaleModule(_float dt);
    void Update_UVAnimationModule(_float dt);
    void Update_SpriteAnimationModule(_float dt);
    void Update_DissolveModule(_float dt);
    void Update_BloomModule(_float dt);
    void Bind_Params();

    /*Default Params*/
    _float m_fProgress{};  //전체 진행도

    /*-----Modules-----*/
    TEXTURE_SLOT_MODULE m_TextureSlotModule{};
    COLOR_MODULE m_ColorModule{};
    SCALE_MODULE m_ScaleModule{};
    UV_ANIMATION_MODULE m_UVAnimaitonModule{};
    SPRITE_ANIMATION_MODULE m_SpriteAnimationModule{};
    DISSOLVE_MODULE m_DissolveModule{};
    BLOOM_MODULE m_BloomModule{};

};
NS_END
