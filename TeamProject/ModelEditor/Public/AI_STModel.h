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
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(COMPONENT_DESC* pArg);
    virtual void Render_GUI() override;

public:
    HRESULT Load_AIModel(const aiScene* pAIScene, string fileName);
    HRESULT Save_Model();

private:
    HRESULT Ready_AIModelData(const aiScene* pAIScene);
private:
    HRESULT Release_Mesh();
private:
    string m_fileName = {};
    _matrix                     m_PreTransform = {};

public:
    static CAI_STModel* Create();
    virtual CComponent* Clone() override;
    virtual void Free() override;
};

NS_END