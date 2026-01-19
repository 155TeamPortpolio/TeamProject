#pragma once
#include "IKSolver.h"

NS_BEGIN(Engine)

class ENGINE_DLL CFootIK final : public IIKSolver
{
public:
    typedef struct FootIKDesc
    {
        _float fRayStartOffset = 0.3f;       // 발 위로 레이 시작 오프셋
        _float fRayDistance = 1.0f;          // 레이캐스트 거리
        _float fMaxHeightDiff = 0.5f;        // 최대 허용 높이 차이
        _float fMinKneeAngle = 0.1f;         // 최소 무릎 각도 (라디안)
        _float fMaxKneeAngle = XM_PI - 0.1f; // 최대 무릎 각도
        _uint  iCollisionMask = 0xFFFFFFFF;  // 레이캐스트 충돌 마스크
        _float fMaxPelvisOffset = 0.3f;
        _bool  bDynamicPoleVector = true;
        _vector3 vPoleVector = _vector3(0.f, 0.f, 1.f);
    } FOOTIK_DESC;

private:
    CFootIK();
    CFootIK(const CFootIK& rhs) = delete;
    virtual ~CFootIK() DEFAULT;

public:
    virtual IK_TYPE Get_Type() const override { return IK_TYPE::TWO_BONE; }

public:
    virtual HRESULT Initialize(void* pArg) override;
    virtual void    Solve(IK_CONTEXT& context) override;
    virtual void    Reset() override;
    virtual void    Render_GUI() override;

private:
    void         Cache_BoneLengths(IK_CONTEXT& context);
    _vector3     Calculate_PoleVector(IK_CONTEXT& context, _int iThigh, _int iCalf, _int iFoot);
    _bool        Find_GroundTarget_Left(IK_CONTEXT& context, _vector3& outTargetPos, _vector3& outGroundNormal);
    _bool        Find_GroundTarget_Right(IK_CONTEXT& context, _vector3& outTargetPos, _vector3& outGroundNormal);
    _float       Calculate_PelvisOffset(IK_CONTEXT& context, _vector3 vLeftTarget, _vector3 vRightTarget);
    void         Calculate_TwoBone_Left(IK_CONTEXT& context, _vector3 vTargetPos, _float fPelvisOffset, _vector3 vPoleVector);
    void         Calculate_TwoBone_Right(IK_CONTEXT& context, _vector3 vTargetPos, _float fPelvisOffset, _vector3 vPoleVector);
    void         Apply_PelvisOffset(IK_CONTEXT& context, _float fOffset);
    void         Align_Foot_Left(IK_CONTEXT& context, _vector3 vGroundNormal);
    void         Align_Foot_Right(IK_CONTEXT& context, _vector3 vGroundNormal);
    _quaternion  WorldRotationToLocal(IK_CONTEXT& context, _int iBoneIndex, _quaternion qWorldRotation);

private:
    _float      m_fLeftUpperLength;
    _float      m_fLeftLowerLength;
    _float      m_fRightUpperLength;
    _float      m_fRightLowerLength;
    _bool       m_bLengthCached;
    _vector3    m_vLeftPole;
    _vector3    m_vRightPole;
    FOOTIK_DESC m_Desc;

    _float m_fDebugPelvisOffset;
    _bool m_bDebugLeftGroundFound;
    _bool m_bDebugRightGroundFound;
    _vector4 m_vDebugLeftThighRot;
    _vector4 m_vDebugLeftCalfRot;
    _vector4 m_vDebugRightThighRot;
    _vector4 m_vDebugRightCalfRot;

public:
    static CFootIK* Create(void* pArg = nullptr);
    virtual void Free() override;
};

NS_END