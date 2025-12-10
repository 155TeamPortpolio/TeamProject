#pragma once
#include "Component.h"

NS_BEGIN(Engine)
class ENGINE_DLL CBoneFollower :
    public CComponent
{
private:
    CBoneFollower();
    CBoneFollower(const CBoneFollower& rhs);
    virtual ~CBoneFollower() override;

public:
    HRESULT Initialize_Prototype();
    HRESULT Initialize(COMPONENT_DESC* pArg) override;
    virtual void Link_Bone(class CAnimator3D* pAnimator, const string& boneName);
    virtual void Sync_Transform(_float dt, class CTransform* pTransform);
    virtual void Set_Offset(_fmatrix Offset);
    virtual void Dettach();
    _bool HasOwner() { return (m_pMasterAnimator || m_pMasterTransform); }

private:
    class CAnimator3D* m_pMasterAnimator = { nullptr };
    class CTransform* m_pMasterTransform = { nullptr };
    string FollowingBone = {};
    _float4x4 m_Offset = {};
public:
    static CBoneFollower* Create();
    virtual CComponent* Clone();
    virtual void Free() override;
};

NS_END