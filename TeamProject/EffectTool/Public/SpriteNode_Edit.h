#pragma once
#include "SpriteNode.h"
#include "EffectContainer_Edit.h"

NS_BEGIN(EffectTool)
class CSpriteNode_Edit :
    public CSpriteNode
{
public:
    typedef struct tagSpriteNodeEditDesc : public GAMEOBJECT_DESC
    {
        CEffectContainer_Edit::EFFECT_EDIT_CONTEXT* pContext = nullptr;
    }SPRITE_NODE_EDIT_DESC;
private:
    CSpriteNode_Edit();
    CSpriteNode_Edit(const CSpriteNode_Edit& rhs);
    virtual ~CSpriteNode_Edit() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void Awake() override;
    void Priority_Update(_float dt) override;
    void Update(_float dt) override;
    void Late_Update(_float dt) override;

public:
    void Render_GUI() override;
    void Play() override;
public:
    static CSpriteNode_Edit* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;

private:
    CEffectContainer_Edit::EFFECT_EDIT_CONTEXT* m_pContext = nullptr;

    void AddTextures();
    void SetUp_SpriteEffect();
};
NS_END
