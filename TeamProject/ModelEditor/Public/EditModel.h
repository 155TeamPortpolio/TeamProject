#pragma once
#include "GameObject.h"
#include "Assimps.h"

NS_BEGIN(ModelEdit)
class CEditModel : public CGameObject
{
    enum class MODEL_IMPORT_MODE : int
    {
        AUTO = 0,
        FORCED_STATIC,
        FORCED_SKELETAL,
    };

private:
    CEditModel() {}
    CEditModel(const CEditModel& rhs) :CGameObject(rhs) {}
    virtual ~CEditModel() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void Awake() override;
    void Priority_Update(_float dt) override {}
    void Update(_float dt) override;
    void Late_Update(_float dt) override {}

public:
    void Render_GUI() override;

private:
    HRESULT Load_AIScene(const string& filePath);
    HRESULT Save_AIScene();
    void Clear_Models();
    _bool HasBones();
    HRESULT ExportBoneInfo();

private:
    const aiScene*      m_pAIScene = { nullptr };
    Assimp::Importer	m_Importer = {};
    MODEL_IMPORT_MODE m_eMode = { MODEL_IMPORT_MODE::AUTO };
public:
    static CEditModel* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override { __super::Free(); }
};
NS_END
