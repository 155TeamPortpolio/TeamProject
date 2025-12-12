#pragma once
#include "ModelEditor_Defines.h"
#include "SkeletalModel.h"
#include "Assimps.h"

NS_BEGIN(Engine)
class CSkeletalModel;
NS_END 

NS_BEGIN(ModelEdit)
class CAI_SKModel final
    : public CSkeletalModel
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
    HRESULT Load_AIModel(string fbxFilePath);

private:
    HRESULT Ready_Skeleton(const aiNode* pAINode);
    HRESULT Ready_Meshes();
    HRESULT Ready_Materials();
    HRESULT Ready_Animations();

private:
    //Assimp
    const aiScene*              m_pAIScene = { nullptr };
    Assimp::Importer	        m_Importer = {};
    //Pretransform
    _matrix                     m_PreTransform = {};
    //Skeleton
    class CAISkeleton*          m_pSkeleton = { nullptr };
    //Meshes
    _uint					    m_iNumMeshes = {};
    vector<class CAIMesh*>	    m_Meshes;
    //Materials
    _uint						m_iNumMaterials = {};
    vector<class CAIMaterial*>	m_Materials;
    //Animation
    _uint						m_iNumAnimations = {};
    vector<class CAIAnimation*>	m_Animations;

public:
    static CAI_SKModel* Create(string fbxFilePath);
    virtual CComponent* Clone() override;
    virtual void Free() override;
};

NS_END