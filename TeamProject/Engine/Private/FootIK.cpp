#include "Engine_Defines.h"
#include "FootIK.h"
#include "Animator3D.h"
#include "GameInstance.h"
#include "PhysicsSystem.h"
#include "ModelData.h"

CFootIK::CFootIK()
	: m_fUpperLength(0.f)
	, m_fLowerLength(0.f)
	, m_bLengthCached(false)
{
}

HRESULT CFootIK::Initialize(void* pArg)
{
    if (pArg)
        m_Desc = *static_cast<FOOTIK_DESC*>(pArg);
    else
    {
        // 기본값
        m_Desc.fRayStartOffset = 0.3f;
        m_Desc.fRayDistance = 1.0f;
        m_Desc.fMaxHeightDiff = 0.5f;
        m_Desc.fMinKneeAngle = 0.1f;
        m_Desc.fMaxKneeAngle = XM_PI - 0.1f;
        m_Desc.iCollisionMask = 0xFFFFFFFF;
    }

    return S_OK;
}

void CFootIK::Solve(IK_CONTEXT& context)
{
    if (!context.pAnimator || context.BoneIndices.size() != 3)
    {
        context.bSuccess = false;
        return;
    }

    context.OutRotations.resize(3);

    if (!m_bLengthCached)
        Cache_BoneLengths(context);

    _vector3 vTargetPos;
    _vector3 vGroundNormal;

    if (!Find_GroundTarget(context, vTargetPos, vGroundNormal))
    {
        context.bSuccess = false;
        return;
    }

    Calculate_TwoBone(context, vTargetPos);
    Align_FootToGround(context, vGroundNormal);
    context.bSuccess = true;
}

void CFootIK::Reset()
{
    m_bLengthCached = false;
    m_fUpperLength = 0.f;
    m_fLowerLength = 0.f;
}

void CFootIK::Render_GUI()
{
    ImGui::SeparatorText("Foot IK");

    ImGui::Text("Upper Length: %.3f", m_fUpperLength);
    ImGui::Text("Lower Length: %.3f", m_fLowerLength);
    ImGui::Text("Cached: %s", m_bLengthCached ? "Yes" : "No");

    ImGui::Separator();
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Raycast Settings");

    ImGui::DragFloat("Ray Start Offset", &m_Desc.fRayStartOffset, 0.01f, 0.f, 1.f);
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("How high above foot to start ray");

    ImGui::DragFloat("Ray Distance", &m_Desc.fRayDistance, 0.01f, 0.1f, 5.f);
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Max distance to check for ground");

    ImGui::DragFloat("Max Height Diff", &m_Desc.fMaxHeightDiff, 0.01f, 0.f, 2.f);
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Ignore ground if too far");

    ImGui::Text("Collision Mask: 0x%08X", m_Desc.iCollisionMask);

    ImGui::Separator();
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Knee Angle Limits");

    _float fMinAngleDeg = XMConvertToDegrees(m_Desc.fMinKneeAngle);
    _float fMaxAngleDeg = XMConvertToDegrees(m_Desc.fMaxKneeAngle);

    if (ImGui::DragFloat("Min Knee Angle", &fMinAngleDeg, 1.f, 0.f, 180.f))
        m_Desc.fMinKneeAngle = XMConvertToRadians(fMinAngleDeg);

    if (ImGui::DragFloat("Max Knee Angle", &fMaxAngleDeg, 1.f, 0.f, 180.f))
        m_Desc.fMaxKneeAngle = XMConvertToRadians(fMaxAngleDeg);

    ImGui::Separator();

    if (ImGui::Button("Reset Cache"))
        Reset();
}

void CFootIK::Cache_BoneLengths(IK_CONTEXT& context)
{
    _int iThigh = context.BoneIndices[0];
    _int iCalf = context.BoneIndices[1];
    _int iFoot = context.BoneIndices[2];
    // 로컬 행렬
    _smatrix matCalf = context.pAnimator->Get_BoneTransformationMatrix(iCalf);
    _smatrix matFoot = context.pAnimator->Get_BoneTransformationMatrix(iFoot);

    // 위치 추출(부모 기준 상대 위치)
    _vector3 vCalfLocal = matCalf.Translation();
    _vector3 vFootLocal = matFoot.Translation();
    // 길이 계산
    m_fUpperLength = vCalfLocal.Length();
    m_fLowerLength = vFootLocal.Length();
    m_bLengthCached = true;

    // 디버깅 출력
    //string msg = "FootIK Length Cached - Upper: " + to_string(m_fUpperLength)
    //    + ", Lower: " + to_string(m_fLowerLength) + "\n";
    //OutputDebugStringA(msg.c_str());
}

_bool CFootIK::Find_GroundTarget(IK_CONTEXT& context, _vector3& outTargetPos, _vector3& outGroundNormal)
{
    // 발목 본 위치 가져오기
    _int iFoot = context.BoneIndices[2];
    // 월드 위치(For RayCast)
    _smatrix matFoot = context.pAnimator->Get_BoneCombinedMatrix(iFoot);
    _vector3 vFootPos = matFoot.Translation();
    _vector3 vRayOrigin = vFootPos + _vector3::Up * m_Desc.fRayStartOffset;
    _vector3 vRayDir = _vector3::Down;

    PHYSICS_RAY rayDesc;
    rayDesc.vOrigin = vRayOrigin;
    rayDesc.vDirection = vRayDir;
    rayDesc.fMaxDistance = m_Desc.fRayDistance;
    rayDesc.iCollisionMask = m_Desc.iCollisionMask;
    rayDesc.bQueryTrigger = false;

    IPhysicsService* pPhysics = CGameInstance::GetInstance()->Get_PhysicsSystem();
    PHYSICS_RAY_HIT hit;
    if (!pPhysics->Raycast(rayDesc, hit)) return false;

    // 높이 차이 검사
    _float fHeightDiff = abs(hit.vPoint.y - vFootPos.y);
    if (fHeightDiff > m_Desc.fMaxHeightDiff)
        return false;

    outTargetPos = hit.vPoint;
    outGroundNormal = hit.vNormal;

    //string msg = "FootIK Hit - Ground Y: " + to_string(fGroundY)
    //    + ", Foot Y: " + to_string(fFootY)
    //    + ", Diff: " + to_string(fHeightDiff) + "\n";
    //OutputDebugStringA(msg.c_str());

    return true;
}

void CFootIK::Calculate_TwoBone(IK_CONTEXT& context, _vector3 vTargetPos)
{
    // 현재 본 위치들
    _int iThigh = context.BoneIndices[0];
    _int iCalf = context.BoneIndices[1];

    _smatrix matThigh = context.pAnimator->Get_BoneCombinedMatrix(iThigh);
    _smatrix matCalf = context.pAnimator->Get_BoneCombinedMatrix(iCalf);

    _vector3 vHipPos = matThigh.Translation();
    _vector3 vKneePos = matCalf.Translation();

    // 목표 거리 계산 및 제한
    _vector3 vToTarget = vTargetPos - vHipPos;
    _float fTargetDist = vToTarget.Length();
    _float fMaxReach = m_fUpperLength + m_fLowerLength;

    if (fTargetDist > fMaxReach * 0.99f)
    {
        vToTarget.Normalize();
        vTargetPos = vHipPos + vToTarget * (fMaxReach * 0.99f);
        fTargetDist = fMaxReach * 0.99f;
        vToTarget = vTargetPos - vHipPos;
    }

    // 코사인 법칙으로 각도 계산
    _float fUpperSq = m_fUpperLength * m_fUpperLength;
    _float fLowerSq = m_fLowerLength * m_fLowerLength;
    _float fTargetSq = fTargetDist * fTargetDist;

    // 무릎 내각
    _float fCosKnee = (fUpperSq + fLowerSq - fTargetSq)
        / (2.f * m_fUpperLength * m_fLowerLength);
    fCosKnee = clamp(fCosKnee, -1.f, 1.f);
    _float fKneeAngle = acosf(fCosKnee);
    fKneeAngle = clamp(fKneeAngle, m_Desc.fMinKneeAngle, m_Desc.fMaxKneeAngle);

    // 허벅지 각도
    _float fCosThigh = (fUpperSq + fTargetSq - fLowerSq)
        / (2.f * m_fUpperLength * fTargetDist);
    fCosThigh = clamp(fCosThigh, -1.f, 1.f);
    _float fThighAngle = acosf(fCosThigh);

    // 회전 계산
    _vector3 vTargetDir = vToTarget;
    vTargetDir.Normalize();
    // Pole Vector와 교차하여 무릎 굽힘 평면 법선 계산
    _vector3 vPole = context.vPoleVector;
    vPole.Normalize();
    _vector3 vKneePlaneNormal = vTargetDir.Cross(vPole);
    vKneePlaneNormal.Normalize();

    _quaternion qTargetDir = _quaternion::FromToRotation(_vector3::Down, vTargetDir);
    _quaternion qThighBend = _quaternion::CreateFromAxisAngle(vKneePlaneNormal, -fThighAngle);
    _quaternion qThighWorld = qTargetDir * qThighBend;

    _quaternion qCalfBend = _quaternion::CreateFromAxisAngle(
        vKneePlaneNormal,
        XM_PI - fKneeAngle
    );

    // 월드 → 로컬 변환
    context.OutRotations[0] = WorldRotationToLocal(context, iThigh, qThighWorld);
    context.OutRotations[1] = WorldRotationToLocal(context, iCalf, qCalfBend);
    context.OutRotations[2] = _quaternion::Identity;
}

void CFootIK::Align_FootToGround(IK_CONTEXT& context, _vector3 vGroundNormal)
{
    _uint iFoot = context.BoneIndices[2];

    _vector3 vNormal = vGroundNormal;
    vNormal.Normalize();

    // 법선이 거의 위를 향하면 생략
    _float fDot = vNormal.Dot(_vector3::Up);
    if (fDot > 0.99f)
        return;

    // Up → Normal 회전 계산
    _quaternion qRotation = _quaternion::FromToRotation(_vector3::Up, vNormal);
    _quaternion qLocal = WorldRotationToLocal(context, iFoot, qRotation);

    context.OutRotations[2] = qLocal;
}

_quaternion CFootIK::WorldRotationToLocal(IK_CONTEXT& context, _int iBoneIndex, _quaternion qWorldRotation)
{
    _smatrix matParentWorld = _smatrix::Identity;
    // 부모 본의 월드 매트릭스
    _int iParent = context.pAnimator->Get_ModelData()->Get_BoneParentIndex(iBoneIndex);
    if (iParent != -1)
    {
        _smatrix matParent = context.pAnimator->Get_BoneCombinedMatrix(iParent);
        matParentWorld = matParent;
    }

    // 부모 역행렬
    _smatrix matParentInv = matParentWorld.Invert();
    // 월드 회전 → 로컬 회전
    _smatrix matWorldRot = _smatrix::CreateFromQuaternion(qWorldRotation);
    _smatrix matLocalRot = matWorldRot * matParentInv;

    // 쿼터니언 추출
    _vector3 scale, trans;
    _quaternion rot;
    matLocalRot.Decompose(scale, rot, trans);

    return rot;
}

CFootIK* CFootIK::Create(void* pArg)
{
    CFootIK* pInstance = new CFootIK();
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Create : CFootIK");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CFootIK::Free()
{
    __super::Free();
}
