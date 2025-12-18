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
    }MESH_NODE_EDIT_DESC;
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
    void Import(nlohmann::ordered_json& json);
    void Export(nlohmann::ordered_json& json);

public:
    static CMeshNode_Edit* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;

private:
    CEffectContainer_Edit::EFFECT_EDIT_CONTEXT* m_pContext = nullptr;

    void SetMaterial();
    void SetMesh();
    _bool ChangeEaseType(EaseType& ioValue, EaseType shownValue);
    void SetUp_MeshEffect();

    string m_ModelKey{};
    string m_MaterialKey{};

    _bool m_IsSpriteMode = false;
    _bool m_SetMesh = false;
    _bool m_SetMaterial = false;
};
NS_END
