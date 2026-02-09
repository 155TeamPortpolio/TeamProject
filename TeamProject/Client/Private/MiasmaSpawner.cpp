#include "pch.h"
#include "MiasmaSpawner.h"
#include "GameInstance.h"
#include "CharacterController.h"
#include "BattleSystem.h"
#include "Defiler.h"
#include "MiasmaBlade.h"

static _float3 ComputeArcSpawnPos(
    const _float3& ownerPos,
    const _float3& targetPos,
    _float minRadius,
    _float maxRadius,
    _float arcDegrees,
    _float yFixed
)
{
    _float3 toTarget = { targetPos.x - ownerPos.x, 0.f, targetPos.z - ownerPos.z };
    _float3 centerDir = Math::NormalizeSafeXZ(toTarget);

    const float halfArcDeg = arcDegrees * 0.5f;
    const float yawDeg = Helper::Get_Random_Float(-halfArcDeg, +halfArcDeg);
    const float yawRad = yawDeg * (3.1415926535f / 180.f);

    const float u = Helper::Get_Random_Float(0.f, 1.f); /*¹èÀ² 0~1*/
    const float minR2 = minRadius * minRadius;
    const float maxR2 = maxRadius * maxRadius;
    const float radius = sqrt(minR2 + (maxR2 - minR2) * u);

    _float3 dir = Math::RotateYawXZ(centerDir, yawRad);
    _float3 spawnPos;
    spawnPos.x = ownerPos.x + dir.x * radius;
    spawnPos.y = yFixed;
    spawnPos.z = ownerPos.z + dir.z * radius;
    return spawnPos;
}

void CMiasmaSpawner::Spawn(MiasmaType type, _int count, _float3 targetPos, _float3 ownerPos, _float y, CDefiler* pDefiler)
{
    switch (type)
    {
    case Client::MiasmaType::Heavy:
        SpawnGrandier(count,targetPos,ownerPos, y);
        break;
    case Client::MiasmaType::Grandier:
        SpawnGrandier(count,targetPos,ownerPos, y);
        break;
    case Client::MiasmaType::Blade:
        SpawnBlade(targetPos,ownerPos, pDefiler);
        break;
    default:
        break;
    }
}

void CMiasmaSpawner::SpawnGrandier(_int count, _float3 targetPos, _float3 ownerPos, _float y)
{
    const string levelKey = LevelManager()->Get_NowLevelKey();

    const float minRadius = 1.0f;
    float maxRadius = (ownerPos - targetPos).Length() + 5.f;
    const float arcDegrees = 60.f;

    for (int spawnIndex = 0; spawnIndex < count; ++spawnIndex)
    {
        _float3 spawnPos = ComputeArcSpawnPos(
            ownerPos, targetPos,
            minRadius, maxRadius,
            arcDegrees,
            ownerPos.y
        );

        spawnPos.y = y;

        CCT_DESC MonsterCCT;
        MonsterCCT.eGroup = COLLISION_GROUP::MONSTER;
        MonsterCCT.iCollisionMask = ENUM(COLLISION_GROUP::PLAYER) | ENUM(COLLISION_GROUP::COMMON) | ENUM(COLLISION_GROUP::PLAYER_ATTACK);
        MonsterCCT.bAutoFit = false;
        MonsterCCT.fHeight = 1.28f;
        MonsterCCT.fRadius = 0.55f;
        MonsterCCT.vPos = spawnPos;
        MonsterCCT.vPos.y += MonsterCCT.fHeight;

        auto jaeger = Builder::Create_Object({ "Zero_Level", "Proto_GameObject_MiasmaJaeger" })
            .Position(spawnPos)
            .CharacterController(MonsterCCT)
            .Build("MiasmaUnit");
        ObjectManager()->Add_Object(jaeger, { levelKey, "Enemy_Layer" });
        BattleSystem()->EnterBattleObject(BATTLE_OBJ_TYPE::MONSTER, jaeger->Get_Handle());
    }
}

void CMiasmaSpawner::SpawnBlade(_float3 Target, _float3 Owner, CDefiler* pDefiler)
{
    const string levelKey = LevelManager()->Get_NowLevelKey();
    auto desc = new CMiasmaBlade::BladeDesc;
    desc->pOwner = pDefiler;
    desc->vTargetPos = Target;
    auto pBlade =
    Builder::Create_Object({ "Zero_Level","Proto_GameObject_MiasmaBlade" })
    .FromPool()
    .Position(Owner)
    .Add_ObjDesc(desc)
    .Build("MiasmaBlade");
    ObjectManager()->Add_Object(pBlade, { levelKey ,"Enemy_Layer" });
}
