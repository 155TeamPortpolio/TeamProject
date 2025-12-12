#pragma once
#include "ModelEditor_Defines.h"
#include "Model.h"
#include "Assimps.h"

NS_BEGIN(Engine)
class CModel;
NS_END 

NS_BEGIN(ModelEdit)
class ENGINE_DLL CAI_STModel :
    public CModel
{
private:
    CAI_SKModel();
    CAI_SKModel(const CAI_SKModel& rhs);
    virtual ~CAI_SKModel() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(COMPONENT_DESC* pArg = nullptr) override;
    virtual void Render_GUI() override;

public:
    virtual const D3D11_INPUT_ELEMENT_DESC* Get_ElementDesc(_uint DrawIndex) override;
    virtual const _uint Get_ElementCount(_uint DrawIndex) override;
    virtual const string_view Get_ElementKey(_uint DrawIndex) override;
    virtual HRESULT Draw(ID3D11DeviceContext* pContext, _uint Index) override;
    virtual HRESULT Link_Model(const string& levelKey, const string& modelDataKey)override;

public:
    virtual _uint Get_MeshCount() override;
    virtual _uint Get_MaterialIndex(_uint Index) override;
    virtual _bool isDrawable(_uint Index) override;
    virtual void SetDrawable(_uint Index, _bool isDraw) override;
    virtual _bool isReadyToDraw()	override;

public:
    virtual MINMAX_BOX Get_LocalBoundingBox()	override;
    virtual vector<MINMAX_BOX> Get_MeshBoundingBoxes()	override;
    virtual MINMAX_BOX Get_MeshBoundingBox(_uint meshIndex)	override;
    virtual MINMAX_BOX Get_WorldBoundingBox()	override;

public:
    HRESULT Load_AIModel(string fbxFilePath);

private:
    HRESULT Ready_Skeleton(const aiNode* pAINode);
    HRESULT Ready_Meshes();
    HRESULT Ready_Materials();

private:
    const aiScene*      m_pAIScene = { nullptr };
    Assimp::Importer	m_Importer = {};
    _matrix             m_PreTransform = {};


public:
    static CAI_STModel* Create(string fbxFilePath);
    virtual CComponent* Clone() override;
    virtual void Free() override;
};

NS_END