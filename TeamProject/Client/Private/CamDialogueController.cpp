#include "pch.h"
#include "CamDialogueController.h"

#include "OrbitCam.h"
#include "Camera.h"
#include "Helper_Func.h"

void CCamDialogueController::Reset()
{
    m_state = {};
    m_state.holdFov = 30.f;
    m_state.dur = 0.5f;
    m_state.ease = EaseType::InOutSine;
    m_state.assumedPartnerFrontDist = 1.f;
    m_state.holdPivotDist = 0.5f;
    m_state.fromPivotDist = -1.f;
    m_state.lastPivotDist = 0.f;
    m_state.blendInit = false;
    m_state.fromPivotWorld = Vector3::Zero;
    m_state.toPivotWorld = Vector3::Zero;
}

void CCamDialogueController::Begin(_float targetFov, _float blendSec, _float assumedPartnerFrontDist)
{
    m_state.assumedPartnerFrontDist = assumedPartnerFrontDist;
    m_state.holdPivotDist = assumedPartnerFrontDist * 0.5f;

    m_state.holdFov = targetFov;

    m_state.holding = true;
    m_state.restoring = false;

    m_state.blending = true;
    m_state.time = 0.f;
    m_state.dur = blendSec > 0.f ? blendSec : 0.f;

    m_state.toFov = targetFov;

    m_state.fromFov = 0.f;
    m_state.fromPivotDist = -1.f;
    m_state.toPivotDist = m_state.holdPivotDist;

    m_state.blendInit = false;
}

void CCamDialogueController::End(_float blendSec)
{
    m_state.holding = false;
    m_state.restoring = true;

    m_state.blending = true;
    m_state.time = 0.f;
    m_state.dur = blendSec > 0.f ? blendSec : 0.f;

    m_state.toFov = m_state.savedFov;

    m_state.fromFov = 0.f;
    m_state.fromPivotDist = -1.f;
    m_state.toPivotDist = 0.f;

    m_state.blendInit = false;
}

void CCamDialogueController::BeginBlend(_float fromFov, _float toFov, _float fromPivotDist, _float toPivotDist, _float dur)
{
    m_state.blending = (dur > 0.f);
    m_state.time = 0.f;
    m_state.dur = dur;

    m_state.fromFov = fromFov;
    m_state.toFov = toFov;

    m_state.fromPivotDist = fromPivotDist;
    m_state.toPivotDist = toPivotDist;
}

void CCamDialogueController::Update(_float dt, CCamera* cam, COrbitCam* orbit, CTransform* focusTr)
{
    if (!m_state.holding && !m_state.blending) return;

    const _float curFov = cam->Get_FOV();

    if ((m_state.holding || m_state.restoring) && m_state.savedFov == 0.f)
        m_state.savedFov = curFov;

    if (m_state.blending && !m_state.blendInit)
    {
        m_state.fromFov = curFov;
        m_state.toFov = m_state.restoring ? m_state.savedFov : m_state.holdFov;

        m_state.fromPivotWorld = orbit->GetCurPivotWorld();

        const Vector3 basePivot = orbit->GetBasePivotWorld();

        const Vector4 look4 = focusTr->Dir(STATE::LOOK);
        Vector3 look(look4.x, look4.y, look4.z);
        look.Normalize();

        m_state.toPivotWorld = m_state.restoring ? basePivot : (basePivot + look * m_state.holdPivotDist);

        m_state.blendInit = true;
    }

    _float outFov = m_state.holding ? m_state.holdFov : m_state.savedFov;
    Vector3 outPivotWorld = orbit->GetBasePivotWorld();

    if (m_state.blending)
    {
        if (m_state.dur <= 0.f)
        {
            outFov = m_state.toFov;
            outPivotWorld = m_state.toPivotWorld;
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

            const _float t = Math::ApplyEase(m_state.ease, rawT);

            outFov = m_state.fromFov + (m_state.toFov - m_state.fromFov) * t;
            outPivotWorld = Vector3::Lerp(m_state.fromPivotWorld, m_state.toPivotWorld, t);
        }
    }
    else
    {
        if (m_state.holding)
        {
            const Vector4 look4 = focusTr->Dir(STATE::LOOK);
            Vector3 look(look4.x, look4.y, look4.z);
            look.Normalize();

            outPivotWorld = orbit->GetBasePivotWorld() + look * m_state.holdPivotDist;
            outFov = m_state.holdFov;
        }
        else
        {
            outPivotWorld = orbit->GetBasePivotWorld();
            outFov = m_state.savedFov;
        }
    }

    cam->Set_FOV(outFov);

    const Vector3 baseNow = orbit->GetBasePivotWorld();
    orbit->SetPivotOverrideOffset(outPivotWorld - baseNow);

    if (!m_state.blending && m_state.restoring)
    {
        orbit->ClearPivotOverrideOffset();

        m_state.restoring = false;
        m_state.savedFov = 0.f;
        m_state.fromFov = 0.f;
        m_state.toFov = 0.f;
        m_state.fromPivotDist = -1.f;
        m_state.lastPivotDist = 0.f;
        m_state.blendInit = false;
        m_state.fromPivotWorld = Vector3::Zero;
        m_state.toPivotWorld = Vector3::Zero;
    }
}