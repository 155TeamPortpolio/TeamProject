#include "pch.h"
#include "CamOcclusionTracker.h"

_int CCamOcclusionTracker::FindIdx(const vector<Entry>& v, const OBJECT_HANDLE& handle) const
{
    for (_int i = 0; i < (_int)v.size(); ++i)
        if (v[i].handle == handle) return i;

    return -1;
}

void CCamOcclusionTracker::AddHit(CGameObject* obj, const Vector3& hitPos, const Vector3& hitNormal, _float hitDist)
{
    auto camCol = dynamic_cast<ICamCollidable*>(obj);
    if (!camCol) return;

    OBJECT_HANDLE handle = obj->Get_Handle();
    _int idx = FindIdx(m_cur, handle);

    if (idx < 0)
    {
        Entry entry;
        entry.handle             = handle;
        entry.hit.other          = handle;
        entry.hit.hitWorldPos    = hitPos;
        entry.hit.hitWorldNormal = hitNormal;
        entry.hit.hitDist        = hitDist;
        m_cur.push_back(entry);
        return;
    }

    if (hitDist >= m_cur[idx].hit.hitDist) return;

    m_cur[idx].hit.hitWorldPos    = hitPos;
    m_cur[idx].hit.hitWorldNormal = hitNormal;
    m_cur[idx].hit.hitDist        = hitDist;
}

void CCamOcclusionTracker::Dispatch()
{
    for (auto& ePrev : m_prev)
    {
        if (FindIdx(m_cur, ePrev.handle) >= 0) continue;

        auto obj = ePrev.handle.Get();
        auto camCol = dynamic_cast<ICamCollidable*>(obj);
        if (!camCol) continue;

        CamOcclusionHit hit = ePrev.hit;
        hit.phase = CamOcclusionPhase::Exit;
        camCol->OnCameraCollision(hit);
    }

    for (auto& eCur : m_cur)
    {
        auto obj = eCur.handle.Get();
        auto camCol = dynamic_cast<ICamCollidable*>(obj);
        if (!camCol) continue;

        _int prevIdx = FindIdx(m_prev, eCur.handle);

        CamOcclusionHit hit = eCur.hit;
        hit.phase = (prevIdx < 0) ? CamOcclusionPhase::Enter : CamOcclusionPhase::Stay;
        camCol->OnCameraCollision(hit);
    }

    m_prev.swap(m_cur);
    m_cur.clear();
}