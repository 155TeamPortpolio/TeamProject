#pragma once

#include "OrbitCamTypes.h"
#include "ICollisionService.h"

NS_BEGIN(Engine)
class CCharacterController; 
NS_END

NS_BEGIN(Client)

class COrbitCollisionDistController
{
public:
    void Reset() {}

    OrbitCollisionDistEvalResult Evaluate(_float dt, const OrbitCamProfile& profile, PxScene* scene, CCharacterController* camCC, 
        const Vector3& pivotWorld, _float wantDist, const Vector2& curRotDeg, const Vector2& targetRotDeg, _float curGoalDist);

private:
    _float ComputeAllowedDist(const OrbitCamProfile& profile, PxScene* scene, CCharacterController* camCC, const Vector3& pivotWorld,
        _float wantDist, const Vector2& curRotDeg, const Vector2& targetRotDeg);
};

NS_END