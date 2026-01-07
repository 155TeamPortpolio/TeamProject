#include "Engine_Defines.h"
#include "FootIK.h"
#include "Animator3D.h"
#include "GameInstance.h"
#include "PhysicsSystem.h"
#include "ModelData.h"

CFootIK::CFootIK()
    : m_fLeftUpperLength(0.f)
    , m_fLeftLowerLength(0.f)
    , m_fRightUpperLength(0.f)
    , m_fRightLowerLength(0.f)
    , m_bLengthCached(false)
{
}

HRESULT CFootIK::Initialize(void* pArg)
{
    if (pArg)
        m_Desc = *static_cast<FOOTIK_DESC*>(pArg);
    return S_OK;
}

void CFootIK::Solve(IK_CONTEXT& context)
{
    if (!context.pAnimator || context.BoneIndices.size() != 7)
    {
        context.bSuccess = false;
        return;
    }

    context.OutRotations.resize(7, _quaternion::Identity);
    context.OutPositions.resize(7, _vector3::Zero);

    if (!m_bLengthCached)
        Cache_BoneLengths(context);

    _vector3 vRightPole = m_Desc.vPoleVector;
    _vector3 vRightTarget, vRightNormal;

    _bool bRightGroundFound = Find_GroundTarget_Right(context, vRightTarget, vRightNormal);

    if (!bRightGroundFound)
    {
        context.bSuccess = false;
        return;
    }

    m_bDebugRightGroundFound = bRightGroundFound;

    // 오른발만 IK 적용
    Calculate_TwoBone_Right(context, vRightTarget, 0.f, vRightPole);
    Align_Foot_Right(context, vRightNormal);

    context.bSuccess = true;
}

void CFootIK::Reset()
{
    m_bLengthCached = false;
    m_fLeftUpperLength = 0.f;
    m_fLeftLowerLength = 0.f;
    m_fRightUpperLength = 0.f;
    m_fRightLowerLength = 0.f;
}

void CFootIK::Render_GUI()
{
    ImGui::SeparatorText("Foot IK Debug");

    ImGui::Text("Cached: %s", m_bLengthCached ? "Yes" : "No");
    ImGui::Text("Left Leg - Upper: %.3f, Lower: %.3f", m_fLeftUpperLength, m_fLeftLowerLength);
    ImGui::Text("Right Leg - Upper: %.3f, Lower: %.3f", m_fRightUpperLength, m_fRightLowerLength);

    ImGui::Separator();
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Pole Vectors");

    ImGui::Checkbox("Dynamic Pole Vector", &m_Desc.bDynamicPoleVector);

    if (m_Desc.bDynamicPoleVector)
    {
        ImGui::Text("Left Pole: (%.2f, %.2f, %.2f)", m_vLeftPole.x, m_vLeftPole.y, m_vLeftPole.z);
        ImGui::Text("Right Pole: (%.2f, %.2f, %.2f)", m_vRightPole.x, m_vRightPole.y, m_vRightPole.z);

        // 중요: Pole Vector 방향이 급격히 바뀌는지 확인
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Pole Direction Check:");
        ImGui::Text("Left Y: %.2f (should be positive)", m_vLeftPole.y);
        ImGui::Text("Right Y: %.2f (should be positive)", m_vRightPole.y);
    }
    else
    {
        ImGui::DragFloat3("Fallback Pole", &m_Desc.vPoleVector.x, 0.1f);
    }

    ImGui::Separator();
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "IK Results Debug");

    ImGui::Text("Left Ground: %s", m_bDebugLeftGroundFound ? "Found" : "Not Found");
    ImGui::Text("Right Ground: %s", m_bDebugRightGroundFound ? "Found" : "Not Found");
    ImGui::Text("Pelvis Offset: %.3f", m_fDebugPelvisOffset);

    // 회전값 확인
    ImGui::Separator();
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Output Rotations (Debug)");

    if (ImGui::TreeNode("Left Leg Rotations"))
    {
        ImGui::Text("Thigh: (%.2f, %.2f, %.2f, %.2f)",
            m_vDebugLeftThighRot.x, m_vDebugLeftThighRot.y, m_vDebugLeftThighRot.z, m_vDebugLeftThighRot.w);
        ImGui::Text("Calf: (%.2f, %.2f, %.2f, %.2f)",
            m_vDebugLeftCalfRot.x, m_vDebugLeftCalfRot.y, m_vDebugLeftCalfRot.z, m_vDebugLeftCalfRot.w);
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Right Leg Rotations"))
    {
        ImGui::Text("Thigh: (%.2f, %.2f, %.2f, %.2f)",
            m_vDebugRightThighRot.x, m_vDebugRightThighRot.y, m_vDebugRightThighRot.z, m_vDebugRightThighRot.w);
        ImGui::Text("Calf: (%.2f, %.2f, %.2f, %.2f)",
            m_vDebugRightCalfRot.x, m_vDebugRightCalfRot.y, m_vDebugRightCalfRot.z, m_vDebugRightCalfRot.w);
        ImGui::TreePop();
    }

    ImGui::Separator();
    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Quaternion Validation");

    _float fLeftThighLen = m_vDebugLeftThighRot.Length();
    _float fLeftCalfLen = m_vDebugLeftCalfRot.Length();
    _float fRightThighLen = m_vDebugRightThighRot.Length();
    _float fRightCalfLen = m_vDebugRightCalfRot.Length();

    ImGui::Text("Left Thigh Length: %.3f %s", fLeftThighLen,
        abs(fLeftThighLen - 1.f) > 0.01f ? "(BAD!)" : "(OK)");
    ImGui::Text("Left Calf Length: %.3f %s", fLeftCalfLen,
        abs(fLeftCalfLen - 1.f) > 0.01f ? "(BAD!)" : "(OK)");
    ImGui::Text("Right Thigh Length: %.3f %s", fRightThighLen,
        abs(fRightThighLen - 1.f) > 0.01f ? "(BAD!)" : "(OK)");
    ImGui::Text("Right Calf Length: %.3f %s", fRightCalfLen,
        abs(fRightCalfLen - 1.f) > 0.01f ? "(BAD!)" : "(OK)");

    if (ImGui::Button("Reset Cache"))
        Reset();
}

void CFootIK::Cache_BoneLengths(IK_CONTEXT& context)
{
    _int iLeftCalf = context.BoneIndices[1];
    _int iLeftFoot = context.BoneIndices[2];
    _int iRightCalf = context.BoneIndices[4];
    _int iRightFoot = context.BoneIndices[5];
    // 로컬 행렬
    _smatrix matLeftCalf = context.pAnimator->Get_BoneTransformationMatrix(iLeftCalf);
    _smatrix matLeftFoot = context.pAnimator->Get_BoneTransformationMatrix(iLeftFoot);
    _smatrix matRightCalf = context.pAnimator->Get_BoneTransformationMatrix(iRightCalf);
    _smatrix matRightFoot = context.pAnimator->Get_BoneTransformationMatrix(iRightFoot);
    // 위치 추출(부모 기준 상대 위치),길이 계산
    m_fLeftUpperLength = matLeftCalf.Translation().Length();
    m_fLeftLowerLength = matLeftFoot.Translation().Length();
    m_fRightUpperLength = matRightCalf.Translation().Length();
    m_fRightLowerLength = matRightFoot.Translation().Length();

    m_bLengthCached = true;
}

_vector3 CFootIK::Calculate_PoleVector(IK_CONTEXT& context, _int iThigh, _int iCalf, _int iFoot)
{
    return _vector3(0.f, 0.f, 1.f);
}

_bool CFootIK::Find_GroundTarget_Left(IK_CONTEXT& context, _vector3& outTargetPos, _vector3& outGroundNormal)
{
    _int iLeftFoot = context.BoneIndices[2];
    _smatrix matFoot = context.pAnimator->Get_BoneCombinedMatrix(iLeftFoot);
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

    if (!pPhysics->Raycast(rayDesc, hit))
        return false;

    _float fHeightDiff = abs(hit.vPoint.y - vFootPos.y);
    if (fHeightDiff > m_Desc.fMaxHeightDiff)
        return false;

    outTargetPos = hit.vPoint;
    outGroundNormal = hit.vNormal;

    return true;
}

_bool CFootIK::Find_GroundTarget_Right(IK_CONTEXT& context, _vector3& outTargetPos, _vector3& outGroundNormal)
{
    _int iRightFoot = context.BoneIndices[5];
    _smatrix matFoot = context.pAnimator->Get_BoneCombinedMatrix(iRightFoot);
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

    if (!pPhysics->Raycast(rayDesc, hit))
        return false;

    _float fHeightDiff = abs(hit.vPoint.y - vFootPos.y);
    if (fHeightDiff > m_Desc.fMaxHeightDiff)
        return false;

    outTargetPos = hit.vPoint;
    outGroundNormal = hit.vNormal;

    return true;
}

_float CFootIK::Calculate_PelvisOffset(IK_CONTEXT& context, _vector3 vLeftTarget, _vector3 vRightTarget)
{
    _int iLeftFoot = context.BoneIndices[2];
    _int iRightFoot = context.BoneIndices[5];

    _vector3 vLeftFootPos = context.pAnimator->Get_BoneCombinedPosition(iLeftFoot);
    _vector3 vRightFootPos = context.pAnimator->Get_BoneCombinedPosition(iRightFoot);

    _float fMinGroundY = min(vLeftTarget.y, vRightTarget.y);
    _float fMinFootY = min(vLeftFootPos.y, vRightFootPos.y);

    _float fOffset = fMinGroundY - fMinFootY;
    fOffset = clamp(fOffset, -m_Desc.fMaxPelvisOffset, m_Desc.fMaxPelvisOffset);

    return fOffset;
}

void CFootIK::Calculate_TwoBone_Left(IK_CONTEXT& context, _vector3 vTargetPos, _float fPelvisOffset, _vector3 vPoleVector)
{
    _int iThigh = context.BoneIndices[0];
    _int iCalf = context.BoneIndices[1];
    _int iFoot = context.BoneIndices[2];

    // 현재 본 위치들 (월드 공간)
    _vector3 vHipPos = context.pAnimator->Get_BoneCombinedPosition(iThigh);
    _vector3 vKneePos = context.pAnimator->Get_BoneCombinedPosition(iCalf);
    _vector3 vFootPos = context.pAnimator->Get_BoneCombinedPosition(iFoot);

    vHipPos.y += fPelvisOffset;

    // Hip → Target 벡터
    _vector3 vToTarget = vTargetPos - vHipPos;
    _float fTargetDist = vToTarget.Length();
    _float fMaxReach = m_fLeftUpperLength + m_fLeftLowerLength;

    // 거리 제한
    if (fTargetDist > fMaxReach * 0.99f)
    {
        vToTarget.Normalize();
        vTargetPos = vHipPos + vToTarget * (fMaxReach * 0.99f);
        fTargetDist = fMaxReach * 0.99f;
    }

    if (fTargetDist < 0.01f)
    {
        fTargetDist = 0.01f;
        _vector3 vDir = vFootPos - vHipPos;
        vDir.Normalize();
        vTargetPos = vHipPos + vDir * fTargetDist;
    }

    vToTarget = vTargetPos - vHipPos;
    vToTarget.Normalize();

    // 코사인 법칙으로 무릎 각도 계산
    _float fUpperSq = m_fLeftUpperLength * m_fLeftUpperLength;
    _float fLowerSq = m_fLeftLowerLength * m_fLeftLowerLength;
    _float fTargetSq = fTargetDist * fTargetDist;

    _float fCosKnee = (fUpperSq + fLowerSq - fTargetSq) / (2.f * m_fLeftUpperLength * m_fLeftLowerLength);
    fCosKnee = clamp(fCosKnee, -1.f, 1.f);
    _float fKneeAngle = acosf(fCosKnee);
    fKneeAngle = clamp(fKneeAngle, m_Desc.fMinKneeAngle, m_Desc.fMaxKneeAngle);

    // 허벅지 각도 계산
    _float fCosThigh = (fUpperSq + fTargetSq - fLowerSq) / (2.f * m_fLeftUpperLength * fTargetDist);
    fCosThigh = clamp(fCosThigh, -1.f, 1.f);
    _float fThighAngle = acosf(fCosThigh);

    // Pole Vector 처리
    _vector3 vPole = vPoleVector;
    if (vPole.LengthSquared() < 0.001f)
        vPole = _vector3(0.f, 0.f, 1.f);
    vPole.Normalize();

    // 무릎 평면 법선
    _vector3 vKneePlaneNormal = vToTarget.Cross(vPole);
    if (vKneePlaneNormal.LengthSquared() < 0.001f)
    {
        _vector3 vAlternate = abs(vToTarget.y) < 0.99f ? _vector3::Up : _vector3::Right;
        vKneePlaneNormal = vToTarget.Cross(vAlternate);
    }
    vKneePlaneNormal.Normalize();

    // === 허벅지 회전 계산 ===
    // Step 1: Hip → Target 방향으로 회전
    _quaternion qRotateToTarget = _quaternion::CreateFromAxisAngle(vKneePlaneNormal, -fThighAngle);

    // Step 2: 기본 Down 방향을 Target 방향으로
    _vector3 vRotatedDir = _vector3::Transform(vToTarget, _smatrix::CreateFromAxisAngle(vKneePlaneNormal, -fThighAngle));

    // Step 3: 실제 월드 회전 계산 - 단순히 vToTarget 방향으로 향하도록
    _smatrix matThighWorld = _smatrix::CreateWorld(_vector3::Zero, vToTarget, vKneePlaneNormal);
    matThighWorld = matThighWorld * _smatrix::CreateFromAxisAngle(vKneePlaneNormal, -fThighAngle);

    _quaternion qThighWorld = _quaternion::CreateFromRotationMatrix(matThighWorld);
    qThighWorld.Normalize();

    // 로컬 회전으로 변환
    _quaternion qThighLocal = WorldRotationToLocal(context, iThigh, qThighWorld);
    qThighLocal.Normalize();

    // === 종아리 회전 계산 ===
    // 종아리는 단순히 무릎 각도만큼 굽힘
    _int iThighParent = context.pAnimator->Get_ModelData()->Get_BoneParentIndex(iThigh);
    _smatrix matThighParent;
    if (iThighParent == -1)
    {
        matThighParent = _smatrix::Identity;
    }
    else
    {
        matThighParent = context.pAnimator->Get_BoneCombinedMatrix(iThighParent);
    }

    // 무릎 평면 법선을 로컬 공간으로
    _smatrix matThighParentInv = matThighParent.Invert();
    _vector3 vPlaneNormalLocal = _vector3::TransformNormal(vKneePlaneNormal, matThighParentInv);
    vPlaneNormalLocal.Normalize();

    // 종아리 회전 (무릎 굽힘)
    _quaternion qCalfLocal = _quaternion::CreateFromAxisAngle(vPlaneNormalLocal, XM_PI - fKneeAngle);
    qCalfLocal.Normalize();

    context.OutRotations[0] = qThighLocal;
    context.OutRotations[1] = qCalfLocal;
    context.OutRotations[2] = _quaternion::Identity;

    m_vDebugLeftThighRot = qThighLocal;
    m_vDebugLeftCalfRot = qCalfLocal;
}

void CFootIK::Calculate_TwoBone_Right(IK_CONTEXT& context, _vector3 vTargetPos, _float fPelvisOffset, _vector3 vPoleVector)
{
    _int iThigh = context.BoneIndices[3];
    _int iCalf = context.BoneIndices[4];
    _int iFoot = context.BoneIndices[5];

    _vector3 vHipPos = context.pAnimator->Get_BoneCombinedPosition(iThigh);
    _vector3 vKneePos = context.pAnimator->Get_BoneCombinedPosition(iCalf);
    _vector3 vFootPos = context.pAnimator->Get_BoneCombinedPosition(iFoot);

    vHipPos.y += fPelvisOffset;

    _vector3 vToTarget = vTargetPos - vHipPos;
    _float fTargetDist = vToTarget.Length();

    if (fTargetDist < 0.01f)
    {
        context.OutRotations[3] = _quaternion::Identity;
        context.OutRotations[4] = _quaternion::Identity;
        context.OutRotations[5] = _quaternion::Identity;
        return;
    }

    _float fMaxReach = m_fRightUpperLength + m_fRightLowerLength;

    if (fTargetDist > fMaxReach * 0.99f)
    {
        vToTarget.Normalize();
        vTargetPos = vHipPos + vToTarget * (fMaxReach * 0.99f);
        fTargetDist = fMaxReach * 0.99f;
    }

    _vector3 vTargetDir = vToTarget;
    vTargetDir.Normalize();

    _float fUpperSq = m_fRightUpperLength * m_fRightUpperLength;
    _float fLowerSq = m_fRightLowerLength * m_fRightLowerLength;
    _float fTargetSq = fTargetDist * fTargetDist;

    _float fCosKnee = (fUpperSq + fLowerSq - fTargetSq) / (2.f * m_fRightUpperLength * m_fRightLowerLength);
    fCosKnee = clamp(fCosKnee, -1.f, 1.f);
    _float fKneeAngle = acosf(fCosKnee);

    _float fCosThigh = (fUpperSq + fTargetSq - fLowerSq) / (2.f * m_fRightUpperLength * fTargetDist);
    fCosThigh = clamp(fCosThigh, -1.f, 1.f);
    _float fThighAngle = acosf(fCosThigh);

    _vector3 vPole = vPoleVector;
    vPole.Normalize();

    _vector3 vKneePlaneNormal = vTargetDir.Cross(vPole);
    if (vKneePlaneNormal.LengthSquared() < 0.01f)
    {
        vKneePlaneNormal = vTargetDir.Cross(_vector3::Right);
    }
    vKneePlaneNormal.Normalize();

    // === 목표 무릎 위치 계산 - 부호 반전 ===
    _smatrix matKneeRotation = _smatrix::CreateFromAxisAngle(vKneePlaneNormal, -fThighAngle);  // 음수!
    _vector3 vTargetKneeDir = _vector3::Transform(vTargetDir, matKneeRotation);
    vTargetKneeDir.Normalize();

    // === 허벅지 회전 ===
    _vector3 vCurrentThighDir = vKneePos - vHipPos;
    vCurrentThighDir.Normalize();

    _quaternion qThighWorld = _quaternion::FromToRotation(vCurrentThighDir, vTargetKneeDir);
    qThighWorld.Normalize();

    _quaternion qThighLocal = WorldRotationToLocal(context, iThigh, qThighWorld);
    qThighLocal.Normalize();

    // === 종아리 회전 ===
    _int iThighParent = context.pAnimator->Get_ModelData()->Get_BoneParentIndex(iThigh);
    _smatrix matThighParentWorld;
    if (iThighParent == -1)
    {
        matThighParentWorld = _smatrix::Identity;
    }
    else
    {
        matThighParentWorld = context.pAnimator->Get_BoneCombinedMatrix(iThighParent);
    }

    _quaternion qThighParentWorld;
    _vector3 parentScale, parentTrans;
    matThighParentWorld.Decompose(parentScale, qThighParentWorld, parentTrans);

    _quaternion qThighNewWorld = qThighParentWorld * qThighLocal;
    qThighNewWorld.Normalize();

    _smatrix matThighNew = _smatrix::CreateFromQuaternion(qThighNewWorld);
    _smatrix matThighNewInv = matThighNew.Invert();
    _vector3 vKneePlaneNormalLocal = _vector3::TransformNormal(vKneePlaneNormal, matThighNewInv);
    vKneePlaneNormalLocal.Normalize();

    // 종아리 로컬 회전 - 부호도 조정 가능
    _quaternion qCalfLocal = _quaternion::CreateFromAxisAngle(vKneePlaneNormalLocal, -(XM_PI - fKneeAngle));  // 음수!
    qCalfLocal.Normalize();

    context.OutRotations[3] = qThighLocal;
    context.OutRotations[4] = qCalfLocal;
    context.OutRotations[5] = _quaternion::Identity;

    m_vDebugRightThighRot = qThighLocal;
    m_vDebugRightCalfRot = qCalfLocal;
}

void CFootIK::Apply_PelvisOffset(IK_CONTEXT& context, _float fOffset)
{
    context.OutPositions[6] = _vector3::Zero;
}

void CFootIK::Align_Foot_Left(IK_CONTEXT& context, _vector3 vGroundNormal)
{
    _uint iFoot = context.BoneIndices[2];

    _vector3 vNormal = vGroundNormal;
    vNormal.Normalize();

    _float fDot = vNormal.Dot(_vector3::Up);
    if (fDot > 0.99f)
        return;

    _quaternion qRotation = _quaternion::FromToRotation(_vector3::Up, vNormal);
    _quaternion qLocal = WorldRotationToLocal(context, iFoot, qRotation);

    context.OutRotations[2] = qLocal;
}

void CFootIK::Align_Foot_Right(IK_CONTEXT& context, _vector3 vGroundNormal)
{
    _uint iFoot = context.BoneIndices[5];

    _vector3 vNormal = vGroundNormal;
    vNormal.Normalize();

    _float fDot = vNormal.Dot(_vector3::Up);
    if (fDot > 0.99f)
        return;

    _quaternion qRotation = _quaternion::FromToRotation(_vector3::Up, vNormal);
    _quaternion qLocal = WorldRotationToLocal(context, iFoot, qRotation);

    context.OutRotations[5] = qLocal;
}

_quaternion CFootIK::WorldRotationToLocal(IK_CONTEXT& context, _int iBoneIndex, _quaternion qWorldRotation)
{
    _int iParent = context.pAnimator->Get_ModelData()->Get_BoneParentIndex(iBoneIndex);
    if (iParent == -1)
        return qWorldRotation;

    _smatrix matParentWorld = context.pAnimator->Get_BoneCombinedMatrix(iParent);
    _quaternion qParent;
    _vector3 scale, trans;
    matParentWorld.Decompose(scale, qParent, trans);

    qParent.Normalize();

    // Conjugate 사용 (정규화된 쿼터니언의 역)
    _quaternion qParentInv(-qParent.x, -qParent.y, -qParent.z, qParent.w);

    _quaternion qLocal = qParentInv * qWorldRotation;
    qLocal.Normalize();

    return qLocal;
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
