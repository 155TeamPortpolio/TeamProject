#pragma once
#include "Base.h"
#include "DynamicBoneStruct.h"

NS_BEGIN(Engine)

class ENGINE_DLL CDynamicBone:
    public CBase
{
protected:
    CDynamicBone();
    virtual ~CDynamicBone() DEFAULT;

public:
    HRESULT Initialize(class CAnimator3D* pAnimator);
    HRESULT Link_ChainData(const vector<DYNAMIC_CHAIN_GROUP>& ChainGrups);
    HRESULT Create_Chain(_int RootIndex);

public:
    void Update(_float dt);

protected:
    void SimulateNode(DYNAMIC_NODE& Node, const _vector3& ParentPos, const CHAIN_PARAM& ChainParam, _float dt);

protected:
    void Create_Node(vector<_int> Indices, DYNAMIC_CHAIN_GROUP& ChineGroup);

protected:
    class CAnimator3D* m_pAnimator = { nullptr };
    vector<DYNAMIC_CHAIN_GROUP> m_ChainGroups;

public:
    static CDynamicBone* Create(class CAnimator3D* pAnimator);
    virtual void Free() override;
};

NS_END