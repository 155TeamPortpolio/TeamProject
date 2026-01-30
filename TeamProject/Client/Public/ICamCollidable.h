#pragma once

NS_BEGIN(Client)

enum class CamOcclusionPhase { Enter, Stay, Exit };

struct CamOcclusionHit
{
    OBJECT_HANDLE      other{};
    Vector3            hitWorldPos{};
    Vector3            hitWorldNormal{};
    _float             hitDist = FLT_MAX;
    CamOcclusionPhase  phase   = CamOcclusionPhase::Stay;
};

class ICamCollidable
{
protected:
    virtual ~ICamCollidable() DEFAULT;

public:
    virtual void OnCameraCollision(const CamOcclusionHit& hit) PURE;
};

NS_END