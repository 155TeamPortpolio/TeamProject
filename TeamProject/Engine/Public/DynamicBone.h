#pragma once
#include "Base.h"
#include "DynamicBoneStruct.h"

NS_BEGIN(Engine)

class ENGINE_DLL CDynamicBone :
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
    /* World */
    void WorldChain(DYNAMIC_CHAIN_GROUP& Group, _float dt);
    void Update_WorldAnchor(DYNAMIC_CHAIN_GROUP& Group);
    void Update_WorldNodes(DYNAMIC_CHAIN_GROUP& Group);
    void Calc_AnchorDelta(DYNAMIC_CHAIN_GROUP& Group,
        _vector3& outDeltaPos,
        _quaternion& outDeltaQuat);
    void Calc_DynamicNode(DYNAMIC_NODE& Node,
        const _vector3& AnchorPrevPos,
        const _vector3& AnchorDeltaPos,
        const _quaternion& AnchorDeltaQuat);
    void Simulate_WorldNode(DYNAMIC_NODE& Node,
        const _vector3& parentPos,
        const _quaternion& parentQuat,
        const CHAIN_PARAM& ChainParam,
        _float dt);
    void Simulate_WorldQuat(DYNAMIC_NODE& Node,
        const _quaternion& parentQuat,
        const CHAIN_PARAM& ChainParam);
    void ApplySimulatedWorldNode(DYNAMIC_CHAIN_GROUP& Group);

    /* Local */
    void LocalChain(DYNAMIC_CHAIN_GROUP& Group, _float dt);
    void Update_LocalAnchorNode(DYNAMIC_CHAIN_GROUP& Group);
    void Simulate_LocalNode(DYNAMIC_NODE& Node,
        const _vector3& parentPos,
        const _quaternion& parentQuat,
        const CHAIN_PARAM& ChainParam,
        _float dt);

    void ApplySimulatedLocalNode(DYNAMIC_CHAIN_GROUP& Group);

protected:
    void Create_Node(vector<_int> Indices, DYNAMIC_CHAIN_GROUP& ChineGroup);

public:
    virtual void Render_GUI();
    void Render_DynamicBone(_vector3 vPos, ImU32 color = IM_COL32(255, 0, 0, 255));

protected:
    class CGameObject*          m_pOwner = { nullptr };
    class CAnimator3D*          m_pAnimator = { nullptr };

    vector<DYNAMIC_CHAIN_GROUP> m_ChainGroups;
    _bool                       m_bInitUpdated = { false };

public:
    static CDynamicBone* Create(class CAnimator3D* pAnimator);
    virtual void Free() override;
};

NS_END