#pragma once
#include "Component.h"
NS_BEGIN(Engine)
class ENGINE_DLL CSkeletonFollower :
    public CComponent
{
private:
    CSkeletonFollower();
    CSkeletonFollower(const CSkeletonFollower& rhs);
    virtual ~CSkeletonFollower() override;

public:
    HRESULT Initialize_Prototype();
    HRESULT Initialize(COMPONENT_DESC* pArg) override;

    HRESULT Link_MyModel(const string& levelKey, const string& modelDataKey);
    HRESULT Link_MasterModel(const string& levelKey, const string& modelDataKey);

    HRESULT Link_MyModel(class CSkeletalModel* pModel);
    HRESULT Link_MasterModel(class CSkeletalModel* pMasterModel);
    void Set_MasterAnimator(class CAnimator3D* pAnimator);
    HRESULT ReLink_WithMaster();
public:
    
    virtual void Sync_Bones(_float dt);
    const vector<_float4x4>& Get_BoneMatrices() { return m_ReflectedMatices; };

private:
    class CModelData* m_pMyData = { nullptr };
    class CModelData* m_pMasterData = { nullptr };
    class CAnimator3D* m_pMasterAnimator = { nullptr };
    vector<_float4x4> m_ReflectedMatices = {};
    vector<_int> m_ReflectedIndices = {};

public:
    static CSkeletonFollower* Create();
    virtual CComponent* Clone();
    virtual void Free() override;
};
NS_END
