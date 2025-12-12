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
    const aiScene*              m_pAIScene = { nullptr };
    Assimp::Importer	        m_Importer = {};
    //저장하기 전 부가 매트릭스
    _matrix                     m_PreTransform = {};

public:
    static CAI_SKModel* Create(string fbxFilePath);
    virtual CComponent* Clone() override;
    virtual void Free() override;
};

NS_END