#include "pch.h"
#include "CamDialogueController.h"
#include "OrbitCam.h"
#include "FieldSystem.h"
// Engine
#include "GameInstance.h"
#include "Camera.h"
#include "Helper_Func.h"
#include "CharacterController.h"

namespace
{
    Vector3 GetLookDir(CTransform* tr)
    {
        const Vector4 look4 = tr->Dir(STATE::LOOK);
        Vector3 look(look4.x, look4.y, look4.z);
        if (look.LengthSquared() <= 0.f) look = Vector3(0.f, 0.f, 1.f);
        look.Normalize();
        return look;
    }

    Vector3 GetPivotWorldFromHandle(OBJECT_HANDLE handle, _float offsetY)
    {
        if (!handle.isValid()) return Vector3::Zero;

        auto obj = ObjectManager()->Request_Object(handle);
        if (!obj) return Vector3::Zero;

        auto cc = obj->Get_Component<CCharacterController>();
        if (!cc) return Vector3::Zero;

        const Vector4 foot4 = cc->Get_FootPosition();
        const Vector3 foot(foot4.x, foot4.y, foot4.z);

        return foot + Vector3(0.f, cc->Get_HalfSize() * 1.5f + offsetY, 0.f);
    }

    Vector3 ClampOffset(const Vector3& offset, _float maxLen)
    {
        if (maxLen <= 0.f) return offset;

        const _float len = offset.Length();
        if (len <= maxLen) return offset;
        if (len <= 0.f) return Vector3::Zero;

        return offset * (maxLen / len);
    }
}

void CCamDialogueController::Reset()
{
    m_state = {};
    m_state.holdFov = 30.f;
    m_state.dur = 0.5f;
    m_state.ease = EaseType::InOutSine;
    m_state.assumedPartnerFrontDist = 1.f;
    m_state.blendInit = false;
    m_state.fromPivotWorld = Vector3::Zero;
    m_state.toPivotWorld = Vector3::Zero;
    m_state.maxPivotOffset = 0.8f;
}

void CCamDialogueController::Begin(_float targetFov, _float blendSec, _float assumedPartnerFrontDist, OBJECT_HANDLE partnerHandle)
{
    m_state.assumedPartnerFrontDist = assumedPartnerFrontDist;
    m_state.partnerHandle = partnerHandle;

    m_state.holdFov = targetFov;

    m_state.holding = true;
    m_state.restoring = false;

    m_state.blending = (blendSec > 0.f);
    m_state.time = 0.f;
    m_state.dur = blendSec > 0.f ? blendSec : 0.f;

    m_state.fromFov = 0.f;
    m_state.toFov = 0.f;

    m_state.blendInit = false;
}

void CCamDialogueController::End(_float blendSec)
{
    m_state.holding = false;
    m_state.restoring = true;

    m_state.blending = (blendSec > 0.f);
    m_state.time = 0.f;
    m_state.dur = blendSec > 0.f ? blendSec : 0.f;

    m_state.fromFov = 0.f;
    m_state.toFov = 0.f;

    m_state.blendInit = false;
}

void CCamDialogueController::Update(_float dt, CCamera* cam, COrbitCam* orbit, CTransform* focusTr)
{
    if (!m_state.holding && !m_state.blending) return;

    const _float curFov = cam->Get_FOV();

    if ((m_state.holding || m_state.restoring) && m_state.savedFov == 0.f)
        m_state.savedFov = curFov;

    const Vector3 basePivot = orbit->GetBasePivotWorld();

    Vector3 desiredPivot = basePivot;

    {
        const _float offsetY = orbit->GetProfileOffsetY();
        const Vector3 partnerPivot = GetPivotWorldFromHandle(m_state.partnerHandle, offsetY);

        if (partnerPivot != Vector3::Zero)
        {
            desiredPivot = (basePivot + partnerPivot) * 0.5f;
        }
        else
        {
            const Vector3 look = GetLookDir(focusTr);
            const _float front = m_state.assumedPartnerFrontDist * 0.5f;
            desiredPivot = basePivot + look * front;
        }

        const Vector3 rawOffset = desiredPivot - basePivot;
        desiredPivot = basePivot + ClampOffset(rawOffset, m_state.maxPivotOffset);
    }

    _float desiredHoldFov = m_state.holdFov;
    if (orbit->IsDistConstrained()) desiredHoldFov = m_state.savedFov;

    if (m_state.blending && !m_state.blendInit)
    {
        m_state.fromFov = curFov;
        m_state.toFov = m_state.restoring ? m_state.savedFov : desiredHoldFov;

        m_state.fromPivotWorld = orbit->GetCurPivotWorld();
        m_state.toPivotWorld = m_state.restoring ? basePivot : desiredPivot;

        m_state.time = 0.f;
        m_state.blendInit = true;
    }

    if (m_state.blending)
    {
        if (m_state.dur <= 0.f)
        {
            cam->Set_FOV(m_state.toFov);
            orbit->SetPivotExternalOffset(m_state.toPivotWorld - orbit->GetBasePivotWorld());
            m_state.blending = false;
        }
        else
        {
            m_state.time += dt;

            _float rawT = m_state.time / m_state.dur;
            if (rawT >= 1.f)
            {
                rawT = 1.f;
                m_state.blending = false;
            }

            const _float t = Math::ApplyEase(m_state.ease, clamp(rawT, 0.f, 1.f));

            const _float toFov = m_state.restoring ? m_state.savedFov : desiredHoldFov;
            const Vector3 toPivot = m_state.restoring ? basePivot : desiredPivot;

            const _float outFov = m_state.fromFov + (toFov - m_state.fromFov) * t;
            const Vector3 outPivotWorld = Vector3::Lerp(m_state.fromPivotWorld, toPivot, t);

            cam->Set_FOV(outFov);
            orbit->SetPivotExternalOffset(outPivotWorld - orbit->GetBasePivotWorld());

            m_state.toFov = toFov;
            m_state.toPivotWorld = toPivot;
        }
    }
    else
    {
        if (m_state.holding)
        {
            cam->Set_FOV(desiredHoldFov);
            orbit->SetPivotExternalOffset(desiredPivot - orbit->GetBasePivotWorld());
        }
        else
        {
            cam->Set_FOV(m_state.savedFov);
            orbit->SetPivotExternalOffset(basePivot - orbit->GetBasePivotWorld());
        }
    }

    if (!m_state.blending && m_state.restoring)
    {
        orbit->ClearPivotExternalOffset();

        m_state.restoring = false;
        m_state.holding = false;
        m_state.savedFov = 0.f;

        m_state.fromFov = 0.f;
        m_state.toFov = 0.f;

        m_state.blendInit = false;
        m_state.fromPivotWorld = Vector3::Zero;
        m_state.toPivotWorld = Vector3::Zero;

        m_state.partnerHandle.Reset();
    }
}