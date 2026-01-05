#pragma once
#include "ModelEditor_Defines.h"
#include "SkeletalModel.h"

NS_BEGIN(ModelEdit)
class CAI_SKModel final : public CSkeletalModel
{
private:
    CAI_SKModel() {}
    CAI_SKModel(const CAI_SKModel& rhs) : CSkeletalModel(rhs) {}
    virtual ~CAI_SKModel() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype() override { return S_OK; }
    virtual HRESULT Initialize(COMPONENT_DESC* pArg) { return S_OK; }
    virtual void Render_GUI() override { __super::Render_GUI(); }
    HRESULT Save_Model(const string& SavePath, _fmatrix WorldMatrix);

public:
    HRESULT Load_AIModel(const aiScene* pAIScene, string fileName);
    class CAIModelData* Get_AIModelData();
    void Clear_Proxy();

private:
    HRESULT Ready_AIModelData(const aiScene* pAIScene);

private:
    HRESULT Release_Mesh();

private:
    string  m_fileName = {};

public:
    static CAI_SKModel* Create();
    virtual CComponent* Clone() override;
    virtual void Free() override { __super::Free(); }
};

NS_END