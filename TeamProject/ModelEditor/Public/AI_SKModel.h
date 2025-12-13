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
    virtual HRESULT Initialize(COMPONENT_DESC* pArg);
    virtual void Render_GUI() override;

public:
    HRESULT Load_AIModel(const aiScene* pAIScene, string fileName);
private:
    HRESULT Ready_AIModelData(const aiScene* pAIScene);

private:
    HRESULT Release_Mesh();
private:
    string m_fileName = {};
    _matrix                     m_PreTransform = {};

public:
    static CAI_SKModel* Create();
    virtual CComponent* Clone() override;
    virtual void Free() override;
};

NS_END