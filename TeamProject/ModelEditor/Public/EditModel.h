#pragma once
#include "GameObject.h"
#include "Assimps.h"

NS_BEGIN(ModelEdit)
class CEditModel :
    public CGameObject
{
    enum class MODEL_IMPORT_MODE : int
    {
        AUTO = 0,
        FORCED_STATIC,
        FORCED_SKELETAL,
    };

private:
    CEditModel();
    CEditModel(const CEditModel& rhs);
    virtual ~CEditModel() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void Awake() override;
    void Priority_Update(_float dt) override;
    void Update(_float dt) override;
    void Late_Update(_float dt) override;

public:
    void Render_GUI() override;

private:
    HRESULT Load_AIScene(const string& filePath);
    HRESULT Save_AIScene();
    void Clear_Models();
    _bool HasBones();
    HRESULT ExportBoneInfo();

    void Test();
private:
    const aiScene*      m_pAIScene = { nullptr };
    Assimp::Importer	m_Importer = {};
    MODEL_IMPORT_MODE m_eMode = { MODEL_IMPORT_MODE::AUTO };

    _bool isTesting = {};
    _float time = {};
    struct SUBMESH_EXPLODE_PARAM
    {
        float explodeStrength = 1.f;
        float explodeDelay = 0.f;
        _float3 explodeDir = { 0.f, 1.f, 0.f };
        float padding = 0.f;
    };

    vector<SUBMESH_EXPLODE_PARAM> m_subMeshExplodeParams;

public:
    static CEditModel* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};
NS_END
