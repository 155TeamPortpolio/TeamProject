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

    const vector<DYNAMIC_CHAIN_GROUP>& Get_ChainGroups() { return m_ChainGroups; };

public:
    void Init_Update();
    void Update(_float dt);

protected:
    void Update_AnchorNode(DYNAMIC_NODE& AnchorNode);
    void SimulateNode(DYNAMIC_NODE& Node,
        const _vector3& parentPos,
        const _quaternion& parentQuat,
        const CHAIN_PARAM& ChainParam,
        _float dt);
    void ApplySimulatedNode();

protected:
    void Create_Node(vector<_int> Indices, DYNAMIC_CHAIN_GROUP& ChineGroup);


protected:
    class CAnimator3D*          m_pAnimator = { nullptr };
    vector<DYNAMIC_CHAIN_GROUP> m_ChainGroups;
    _bool                       m_bInitUpdated = { false };

public:
    static CDynamicBone* Create(class CAnimator3D* pAnimator);
    virtual void Free() override;
};

NS_END