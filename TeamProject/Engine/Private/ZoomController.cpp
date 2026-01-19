#include "Engine_Defines.h"
#include "ZoomController.h"

namespace
{
    static const CamZoomPreset kZoomPresets[(int)CamZoomType::End] =
    {
        CamZoomPreset{0.45f, 0.050f, 0.120f},
        CamZoomPreset{0.70f, 0.045f, 0.140f},
        CamZoomPreset{0.90f, 0.045f, 0.150f},
        CamZoomPreset{1.30f, 0.050f, 0.180f},
        CamZoomPreset{1.70f, 0.055f, 0.200f},

        CamZoomPreset{1.10f, 0.055f, 0.220f},
        CamZoomPreset{2.10f, 0.060f, 0.260f},

        CamZoomPreset{0.80f, 0.050f, 0.170f},
        CamZoomPreset{1.40f, 0.055f, 0.230f},

        CamZoomPreset{0.55f, 0.045f, 0.110f},
    };

    _float Clamp01(_float t)
    {
        if (t < 0.f) return 0.f;
        if (t > 1.f) return 1.f;
        return t;
    }

    _float Smooth01(_float t)
    {
        t = Clamp01(t);
        return t * t * (3.f - 2.f * t);
    }

    _float Weight(const CamZoomInstance& s)
    {
        const _float a = s.attackSec;
        const _float r = s.releaseSec;

        if (a <= 0.f && r <= 0.f) return 0.f;

        if (a > 0.f && s.elapsed < a)
            return Smooth01(s.elapsed / a);

        if (r <= 0.f) return 0.f;

        const _float t = s.elapsed - max(0.f, a);
        if (t >= r) return 0.f;

        return 1.f - Smooth01(t / r);
    }
}

const CamZoomPreset& ZoomController::GetPreset(CamZoomType type) const
{
    return kZoomPresets[(int)type];
}

void ZoomController::Reset()
{
    m_instances.clear();
}

void ZoomController::SetPunch(_float amountDeg, _float attackSec, _float releaseSec)
{
    m_instances.clear();
    AddPunch(amountDeg, attackSec, releaseSec);
}

void ZoomController::AddPunch(_float amountDeg, _float attackSec, _float releaseSec)
{
    CamZoomInstance s{};
    s.amountDeg = amountDeg;
    s.attackSec = attackSec;
    s.releaseSec = releaseSec;
    s.elapsed = 0.f;

    m_instances.push_back(s);

    if (m_instances.size() > 4)
        m_instances.erase(m_instances.begin());
}

void ZoomController::Set(CamZoomType type, _float strength)
{
    m_instances.clear();
    Add(type, strength);
}

void ZoomController::Add(CamZoomType type, _float strength)
{
    AddPreset(GetPreset(type), strength);
}

void ZoomController::AddPreset(const CamZoomPreset& p, _float strength)
{
    CamZoomInstance s{};
    s.amountDeg = p.amountDeg * strength;
    s.attackSec = p.attackSec;
    s.releaseSec = p.releaseSec;
    s.elapsed = 0.f;

    m_instances.push_back(s);

    if (m_instances.size() > 4)
        m_instances.erase(m_instances.begin());
}

void ZoomController::Clear(_float fadeOutSec)
{
    if (fadeOutSec <= 0.f)
    {
        m_instances.clear();
        return;
    }

    for (auto& s : m_instances)
    {
        const _float w = Weight(s);
        s.amountDeg *= w;
        s.attackSec = 0.f;
        s.releaseSec = fadeOutSec;
        s.elapsed = 0.f;
    }
}

_float ZoomController::Apply(_float dt)
{
    if (m_instances.empty()) return 0.f;

    _float acc = 0.f;

    for (auto& s : m_instances)
    {
        const _float w = Weight(s);
        if (w > 0.f) acc += s.amountDeg * w;

        s.elapsed += dt;
    }

    m_instances.erase(remove_if(m_instances.begin(), m_instances.end(), [&](const CamZoomInstance& s)
        {
            const _float endT = max(0.f, s.attackSec) + max(0.f, s.releaseSec);
            return s.elapsed >= endT;
        }), m_instances.end());

    return acc;
}