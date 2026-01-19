#pragma once

#include "ZoomControllerData.h"

NS_BEGIN(Engine)

class ZoomController
{
public:
    void   SetPunch(_float amountDeg, _float attackSec, _float releaseSec);
    void   AddPunch(_float amountDeg, _float attackSec, _float releaseSec);

    void   Set(CamZoomType type, _float strength = 1.f);
    void   Add(CamZoomType type, _float strength = 1.f);

    void   Clear(_float fadeOutSec);
    _float Apply(_float dt);
    void   Reset();

private:
    void                AddPreset(const CamZoomPreset& p, _float strength);
    const CamZoomPreset& GetPreset(CamZoomType type) const;

private:
    vector<CamZoomInstance> m_instances{};
};

NS_END