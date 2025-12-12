#pragma once
#include "ModelEditor_Defines.h"
#include "StaticModel.h"
#include "Assimps.h"

NS_BEGIN(Engine)
class CModel;
NS_END 

NS_BEGIN(ModelEdit)
class CAI_STModel :
    public CStaticModel
{
private:
    CAI_STModel();
    CAI_STModel(const CAI_STModel& rhs);
    virtual ~CAI_STModel() DEFAULT;

public:
    virtual HRESULT Initialize(string fbxFilePath);
    virtual void Render_GUI() override;

public:
    HRESULT Load_AIModel(string fbxFilePath);

private:
    HRESULT Ready_AIModelData();
    HRESULT Ready_AIMaterials();
    HRESULT Ready_AIAnimations();

private:
    //Assimp
    const aiScene* m_pAIScene = { nullptr };
    Assimp::Importer	        m_Importer = {};
    //PreTransform
    _matrix                     m_PreTransform = {};

public:
    static CAI_STModel* Create(string fbxFilePath);
    virtual CComponent* Clone() override;
    virtual void Free() override;
};

NS_END