#include "pch.h"
#include "MiasmaSpawner.h"
#include "GameInstance.h"
#include "CharacterController.h"
#include "BattleSystem.h"
#include "Defiler.h"
#include "MiasmaBlade.h"
#include "MiasmaSpawnBall.h"
#include "DefilerWeapon.h"

_float3 CMiasmaSpawner::ComputeArcSpawnPos(
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

    const float u = Helper::Get_Random_Float(0.f, 1.f); /*배율 0~1*/
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

_float3 CMiasmaSpawner::ComputeParabolarPos(const _float3& ownerPos, const _float3& targetPos)
{
    _vector3 forward = _vector3(targetPos) - _vector3(ownerPos);
    _float length = forward.Length();
    forward.y = 0.f;
    if (forward.LengthSquared() < 1e-6f)
        forward = { 0.f, 0.f, 1.f };
    else
        forward.Normalize();

    const _vector3 up = { 0.f, 1.f, 0.f };

    _vector3 right = up.Cross(forward);
    right.Normalize();

    const _float sideSign = m_parabolLeft ? -1.f : +1.f; // 왼쪽이면 -1
    m_parabolLeft = !m_parabolLeft;

    const _float sideOffset = 3.0f;   
    const _float forwardOffset = length * 0.4f;
    _vector3 offset = right * (sideOffset * sideSign) + forward * forwardOffset;

    _vector3 pos = _vector3(ownerPos) + offset;
    return _float3{ pos.x, targetPos.y, pos.z };
}

_float3 CMiasmaSpawner::ComputeCircular(const _float3& centerPos, _float radius)
{
    if (radius <= 0.f)
        return centerPos;

    const float rand01 = Helper::Get_Random_Float(0.f, 1.f);
    const float angle = Helper::Get_Random_Float(0.f, XM_2PI);

    const float dist = radius * sqrtf(rand01);

    const float offsetX = cosf(angle) * dist;
    const float offsetZ = sinf(angle) * dist;

    return _float3(
        centerPos.x + offsetX,
        centerPos.y,
        centerPos.z + offsetZ
    );
}
void CMiasmaSpawner::Spawn(MiasmaType type, _int count, _float3 targetPos, _float3 ownerPos, _float y, CDefiler* pDefiler)
{
    switch (type)
    {
    case Client::MiasmaType::Heavy:
        SpawnHeavy(targetPos,ownerPos);
        break;
    case Client::MiasmaType::Grandier:
        SpawnGrandier(count,targetPos,ownerPos, y);
        break;
    case Client::MiasmaType::Blade:
        SpawnBlade(targetPos,ownerPos, pDefiler);
        break;
    case Client::MiasmaType::Weapon:
        SpawnWeapon(targetPos, ownerPos, pDefiler);
        break;
    default:
        break;
    }
}

void CMiasmaSpawner::SpawnGrandier(_int count, _float3 targetPos, _float3 ownerPos, _float y)
{
    const string levelKey = LevelManager()->Get_NowLevelKey();

    const float minRadius = 1.0f;
    float maxRadius = (_vector3(ownerPos)- _vector3(targetPos)).Length() + 5.f;
    const float arcDegrees = 60.f;

    for (int spawnIndex = 0; spawnIndex < count; ++spawnIndex)
    {
        _float3 spawnPos = ComputeCircular({0,0,0},15);

        spawnPos.y = y;
        CCT_DESC MonsterCCT;
        MonsterCCT.eGroup = COLLISION_GROUP::MONSTER;
        MonsterCCT.iCollisionMask = ENUM(COLLISION_GROUP::PLAYER) | ENUM(COLLISION_GROUP::GROUND) | ENUM(COLLISION_GROUP::COMMON) | ENUM(COLLISION_GROUP::PLAYER_ATTACK);
        MonsterCCT.bAutoFit = false;
        MonsterCCT.fHeight = 1.28f;
        MonsterCCT.fRadius = 0.55f;
        MonsterCCT.vPos = spawnPos;
        MonsterCCT.vPos.y += MonsterCCT.fHeight;

        auto jaeger = Builder::Create_Object({ "Zero_Level", "Proto_GameObject_MiasmaGrandierJaeger" })
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
    .Position(Owner)
    .Add_ObjDesc(desc)
    .Build("MiasmaBlade");
    ObjectManager()->Add_Object(pBlade, { levelKey ,"Enemy_Layer" });
    BattleSystem()->EnterBattleObject(BATTLE_OBJ_TYPE::MONSTER, pBlade->Get_Handle());
}

void CMiasmaSpawner::SpawnHeavy(_float3 Target, _float3 Owner)
{
    const string levelKey = LevelManager()->Get_NowLevelKey();
    auto desc = new CMiasmaSpawnBall::SpawnParbolar;
    desc->startPos = Owner;
    desc->targetPos = ComputeParabolarPos(Owner,Target);

    COLLIDER_DESC ColDesc = {};
    ColDesc.eGroup = COLLISION_GROUP::COMMON;
    ColDesc.iCollisionMask = ENUM(COLLISION_GROUP::GROUND);
    ColDesc.bTrigger = false;
    ColDesc.bAutoFit = true;
    ColDesc.eType = COLLIDER_TYPE::SPHERE;

    auto Ball = Builder::Create_Object({ "Zero_Level", "Proto_GameObject_MiasmaSpawnBall" })
        .Position(Owner)
        .Add_ObjDesc(desc)
        .Collider(ColDesc)
        .Build("MiasmaSpawn");
    ObjectManager()->Add_Object(Ball, { levelKey, "Enemy_Layer" });
}

void CMiasmaSpawner::SpawnWeapon(_float3 Target, _float3 Owner, class CDefiler* pDefiler)
{
    ++m_WeaponThrowCount;

    const string levelKey = LevelManager()->Get_NowLevelKey();
    auto desc = new CDefilerWeapon::DefilerWeaponDesc;
    desc->vTargetPos = { 0, Target.y,3 };
    desc->isFinal = m_WeaponThrowCount == 3;

    auto pBlade =
        Builder::Create_Object({ "Zero_Level","Proto_GameObject_DefilerWeapon" })
        .Position(Owner)
        .Add_ObjDesc(desc)
        .Build("DefilerWeapon");
    ObjectManager()->Add_Object(pBlade, { levelKey ,"Enemy_Layer" });
    BattleSystem()->EnterBattleObject(BATTLE_OBJ_TYPE::MONSTER, pBlade->Get_Handle());

    if (m_WeaponThrowCount >= 3)
        m_WeaponThrowCount = 0;
}
