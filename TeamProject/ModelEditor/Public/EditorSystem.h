#pragma once
#include "Base.h"
NS_BEGIN(Engine)
class CLayer;
class CMode;
class CMaterial;
NS_END


NS_BEGIN(ModelEdit)
class CEditorSystem :
    public CBase
{
    struct SelectedMesh
    {
        _float4x4* pWorldMatrix = { nullptr };
        CModel* pModel = { nullptr };
        CMaterial* pMaterial = { nullptr };
    };

    DECLARE_SINGLETON(CEditorSystem);
private:
    explicit CEditorSystem();
    virtual ~CEditorSystem() DEFAULT;

public:
    HRESULT Initialize();
    void Update();

public:
    void Load_MaterialMaps(const string& mapDirectory);
    HRESULT Read_MaterialMaps();
    void Load_TextureMaps();

private:
    class CRayCaster* m_pRayCast = { nullptr };
    CLayer* m_pModelLayer = { nullptr };
    SelectedMesh mesh = {};

    vector<string> m_Directorys;
    unordered_map<string, json> m_fileMaps;

public:
    virtual void Free() override;
};
NS_END