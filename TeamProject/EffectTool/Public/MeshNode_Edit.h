#pragma once
#include "MeshNode.h"
#include "EffectContainer_Edit.h"

NS_BEGIN(EffectTool)
class CMeshNode_Edit :
    public CMeshNode
{
public:
    typedef struct tagMeshNodeEditDesc : public GAMEOBJECT_DESC
    {
        CEffectContainer_Edit::EFFECT_EDIT_CONTEXT* pContext = nullptr;
    }PARTICLE_NODE_EDIT_DESC;
private:
    CMeshNode_Edit();
    CMeshNode_Edit(const CMeshNode_Edit& rhs);
    virtual ~CMeshNode_Edit() DEFAULT;

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
    static CMeshNode_Edit* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;

private:
    CEffectContainer_Edit::EFFECT_EDIT_CONTEXT* m_pContext = nullptr;

    void AddTexture();
    void SetMesh();
    void SetUp_MeshEffect();

};
NS_END
