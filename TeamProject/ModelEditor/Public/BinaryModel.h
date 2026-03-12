#pragma once
#include "GameObject.h"
#include "Assimps.h"

NS_BEGIN(ModelEdit)
class CBinaryModel :
    public CGameObject
{
private:
    CBinaryModel();
    CBinaryModel(const CBinaryModel& rhs);
    virtual ~CBinaryModel() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void Awake() override;
    void Priority_Update(_float dt) override;
    void Update(_float dt) override;
    void Late_Update(_float dt) override;

public:
    void Render_GUI() override;
    void Test();

private:
    HRESULT Load_Binary(const string& filePath);
    _bool isTesting = {};
    _float time = {};
    struct SUBMESH_EXPLODE_PARAM
    {
        float explodeStrength = 1.f;
        float explodeDelay = 0.f;
        _float3 explodeDir = { 0.f, 1.f, 0.f };
        float padding = 0.f;
    };
    float m_scatterDistance = 5.f;
    float m_rotationStrength = 0.1f;
    float m_upBias = 0.15f;
    vector<SUBMESH_EXPLODE_PARAM> m_subMeshExplodeParams;
    vector<uint32_t> m_subMeshScatterIndices;
    vector<_float3> m_subMeshLocalCenters;
public:
    static CBinaryModel* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};
NS_END
