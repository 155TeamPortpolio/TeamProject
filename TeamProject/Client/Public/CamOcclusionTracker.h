#pragma once

#include "ICamCollidable.h"
#include "GameObject.h"

NS_BEGIN(Client)

class CCamOcclusionTracker final
{
public:
    void Reset()      { m_cur.clear(); m_prev.clear();}
    void BeginFrame() { m_cur.clear(); }
    void AddHit(CGameObject* obj, const Vector3& hitPos, const Vector3& hitNormal, _float hitDist);
    void Dispatch();

private:
    struct Entry
    {
        OBJECT_HANDLE   handle{};
        CamOcclusionHit hit{};
    };

private:
    vector<Entry> m_cur;
    vector<Entry> m_prev;

private:
    _int FindIdx(const vector<Entry>& vec, const OBJECT_HANDLE& handle) const;
};

NS_END