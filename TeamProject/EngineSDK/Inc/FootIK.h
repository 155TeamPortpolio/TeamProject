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
    _bool        Find_GroundTarget(IK_CONTEXT& context, _vector3& outTargetPos, _vector3& outGroundNormal);
    void         Calculate_TwoBone(IK_CONTEXT& context, _vector3 vTargetPos);
    void         Align_FootToGround(IK_CONTEXT& context, _vector3 vGroundNormal);
    _quaternion  WorldRotationToLocal(IK_CONTEXT& context, _uint iBoneIndex, _quaternion qWorldRotation);

private:
    // 본 길이 캐시
    _float  m_fUpperLength;     // 허벅지 길이
    _float  m_fLowerLength;     // 종아리 길이
    _bool   m_bLengthCached;    // 캐싱 완료 여부
    // 레이캐스트 설정
    FOOTIK_DESC m_Desc;
public:
    static CFootIK* Create(void* pArg = nullptr);
    virtual void Free() override;
};

NS_END