#include "pch.h"
#include "PedestrianNpc.h"
#include "SkeletalModel.h"
#include "Material.h"
#include "Helper_Func.h"
#include "Animator3D.h"

static vector<vector<_vector3>> dstPoint =
{
    {
        { 55.f,  0.f,  6.5f },
        { 55.f,  0.f, -0.8f },
        { 40.f,  0.f, -4.5f },
        { -18.f, 0.f, -4.5f },
        { -27.f, 0.f, -3.8f },
        { -30.f, 0.f, -1.f },
        { -30.f, 0.f,  7.f },
        { -50.f, 0.f,  7.f },
        { -57.f, 0.f, 17.f },
        { -57.f, 0.f, 29.f },
    },
    {
        { -57.f, 0.f, 29.f },
        { -57.f, 0.f, 17.f },
        { -52.f, 0.f,  7.f },
        { -30.f, 0.f,  7.f },
        { -30.f, 0.f, -1.f },
        { -27.f, 0.f, -3.8f },
        { -18.f, 0.f, -4.5f },
        {  40.f, 0.f, -4.5f },
        {  55.f, 0.f, -0.8f },
        {  55.f, 0.f,  6.5f },
    }
};

CPedestrianNpc::CPedestrianNpc()
{
}

CPedestrianNpc::CPedestrianNpc(const CPedestrianNpc& rhs)
    : CNpc(rhs)
{
}

HRESULT CPedestrianNpc::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CSkeletalModel>();
    Add_Component<CMaterial>();
    Add_Component<CAnimator3D>();

    return S_OK;
}

HRESULT CPedestrianNpc::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Get_Component<CModel>()->Link_Model("MainCity_Level", "NPC_21.model");
    Get_Component<CMaterial>()->Link_Material("MainCity_Level", "NPC_21.mat");

    const _uint routeCount = (_uint)dstPoint.size();
    if (routeCount == 0)
        return E_FAIL;

    m_RouteIdx = (_uint)Helper::Get_Random_Int(0, (int)routeCount - 1);

    const vector<_vector3>& route = dstPoint[m_RouteIdx];
    if (route.size() < 2)
        return E_FAIL;

    m_CurPointIdx = 0;

    m_MoveSpeed = Helper::Get_Random_Int(2, 4);
    m_PathOffset = Helper::Get_Random_Float(-2.2f, 2.2f);

    _vector3 point0 = route[0];
    _vector3 point1 = route[1];

    _vector3 dir01 = Math::NormalizeSafeXZ(point1 - point0);
    _vector3 right01 = Rotate90ByCw(dir01, true);

    _vector3 startPos = point0 + right01 * m_PathOffset;
    m_pTransform->Set_Pos(startPos);
    m_pTransform->Set_Look(dir01);

    m_Vel = dir01 * (float)m_MoveSpeed;
    m_Vel.y = 0.f;

    Get_Component<CAnimator3D>()->LinkAnimate_Model("MainCity_Level", "NPC_21.model");
    Get_Component<CAnimator3D>()->Link_MetaData("MainCity_Level", "NPC_21_Meta.json");
    Get_Component<CAnimator3D>()
        ->Set_Animation("NPC_Female_Size01_Ani_MainCity_Walk_Lively_030").Loop(true).Apply();

    return S_OK;
}

void CPedestrianNpc::Awake()
{
}

void CPedestrianNpc::Priority_Update(_float dt)
{
}

void CPedestrianNpc::Update(_float dt)
{
    Get_Component<CAnimator3D>()->Update_Animation(dt);
    Calc_Destination(dt);
}

void CPedestrianNpc::Late_Update(_float dt)
{
}

_vector3 CPedestrianNpc::Rotate90ByCw(const _vector3& v, bool cw)
{
    if (cw)
        return _vector3(v.z, 0.f, -v.x);
    return _vector3(-v.z, 0.f, v.x);
}

void CPedestrianNpc::Calc_Destination(_float dt)
{
    if (dt <= 0.f)
        return;

    if (dstPoint.empty())
        return;

    if (m_RouteIdx >= (_uint)dstPoint.size())
        m_RouteIdx = 0;

    const vector<_vector3>& route = dstPoint[m_RouteIdx];
    const _uint pointCount = (_uint)route.size();
    if (pointCount < 2)
        return;

    const _uint lastIndex = pointCount - 1;

    if (m_CurPointIdx > lastIndex)
        m_CurPointIdx = 0;

    const _uint indexA = m_CurPointIdx;
    const _uint indexB = (indexA + 1 <= lastIndex) ? (indexA + 1) : 0;
    const _uint indexC = (indexB + 1 <= lastIndex) ? (indexB + 1) : 0;

    const _vector3 pointA = route[indexA];
    const _vector3 pointB = route[indexB];
    const _vector3 pointC = route[indexC];

    _vector3 pos = m_pTransform->Get_WorldPos();
    pos.y = 0.f;

    const float arriveRadius = 1.0f;
    const float blendStartDist = 6.0f;
    const float lanePull = 2.5f;
    const float maxAccel = 10.0f;
    const float passEps = 0.05f;

    _vector3 segAB = pointB - pointA;
    segAB.y = 0.f;

    float lenAB = segAB.Length();
    if (lenAB <= 1e-5f)
    {
        m_CurPointIdx = indexB;
        return;
    }

    _vector3 dirAB = segAB / lenAB;

    _vector3 segBC = pointC - pointB;
    segBC.y = 0.f;

    float lenBC = segBC.Length();
    _vector3 dirBC = (lenBC > 1e-5f) ? (segBC / lenBC) : dirAB;

    _vector3 toB = pointB - pos;
    toB.y = 0.f;

    float distToB = toB.Length();

    float turnWeight = 0.f;
    if (distToB < blendStartDist)
    {
        float near01 = 1.f - Math::Clamp01(distToB / blendStartDist);
        turnWeight = Math::SmoothStep01(near01);
    }

    _vector3 guideDir = Math::NormalizeSafeXZ(dirAB * (1.f - turnWeight) + dirBC * turnWeight);

    _vector3 fromA = pos - pointA;
    fromA.y = 0.f;

    float tAB = fromA.Dot(dirAB);
    tAB = clamp(tAB, 0.f, lenAB);

    _vector3 railAB = pointA + dirAB * tAB;

    _vector3 fromB = pos - pointB;
    fromB.y = 0.f;

    float tBC = fromB.Dot(dirBC);
    tBC = clamp(tBC, 0.f, (lenBC > 1e-5f ? lenBC : 0.f));

    _vector3 railBC = pointB + dirBC * tBC;

    _vector3 railPos = railAB * (1.f - turnWeight) + railBC * turnWeight;

    _vector3 rightAB = Rotate90ByCw(dirAB, true);
    _vector3 rightBC = Rotate90ByCw(dirBC, true);
    _vector3 rightBlend = Math::NormalizeSafeXZ(rightAB * (1.f - turnWeight) + rightBC * turnWeight);

    _vector3 lanePos = railPos + rightBlend * m_PathOffset;

    _vector3 toLane = lanePos - pos;
    toLane.y = 0.f;

    _vector3 lateral = toLane - guideDir * toLane.Dot(guideDir);

    float speed = (float)m_MoveSpeed;
    _vector3 desiredVel = guideDir * speed + lateral * lanePull;

    _vector3 dv = desiredVel - m_Vel;
    dv.y = 0.f;

    float dvLen = dv.Length();
    float maxDv = maxAccel * dt;

    if (dvLen > maxDv && dvLen > 1e-6f)
        dv *= (maxDv / dvLen);

    m_Vel += dv;
    m_Vel.y = 0.f;

    m_pTransform->Translate(m_Vel * dt);

    _vector3 lookDir = Math::NormalizeSafeXZ(m_Vel);
    m_pTransform->Set_Look(lookDir);

    _vector3 posAfter = m_pTransform->Get_WorldPos();
    posAfter.y = 0.f;

    _vector3 toBAfter = pointB - posAfter;
    toBAfter.y = 0.f;

    _vector3 fromAAfter = posAfter - pointA;
    fromAAfter.y = 0.f;

    float tABAfter = fromAAfter.Dot(dirAB);

    bool passedB =
        (toBAfter.LengthSquared() <= arriveRadius * arriveRadius) ||
        (tABAfter >= (lenAB - passEps));

    if (passedB)
    {
        if (indexB == 0)
        {
            SnapToStart();
            return;
        }

        m_CurPointIdx = indexB;
        m_Vel = Math::NormalizeSafeXZ(m_Vel) * speed;
        m_Vel.y = 0.f;
    }
}

void CPedestrianNpc::SnapToStart()
{
    if (dstPoint.empty())
        return;

    if (m_RouteIdx >= (_uint)dstPoint.size())
        m_RouteIdx = 0;

    const vector<_vector3>& route = dstPoint[m_RouteIdx];
    if (route.size() < 2)
        return;

    m_CurPointIdx = 0;

    _vector3 point0 = route[0];
    _vector3 point1 = route[1];

    _vector3 dir01 = Math::NormalizeSafeXZ(point1 - point0);
    _vector3 right01 = Rotate90ByCw(dir01, true);

    _vector3 startPos = point0 + right01 * m_PathOffset;

    m_pTransform->Set_Pos(startPos);
    m_pTransform->Set_Look(dir01);

    m_Vel = dir01 * (float)m_MoveSpeed;
    m_Vel.y = 0.f;
}

_vector3 CPedestrianNpc::RotateTowardsXZ(_vector3 curDir, _vector3 targetDir, float maxRadDelta)
{
    curDir.y = 0.f;
    targetDir.y = 0.f;

    curDir = Math::NormalizeSafeXZ(curDir);
    targetDir = Math::NormalizeSafeXZ(targetDir);

    _float dotVal = clamp(curDir.Dot(targetDir), -1.f, 1.f);
    _float angle = acosf(dotVal);

    if (angle <= 1e-6f || angle <= maxRadDelta)
        return targetDir;

    _float weight = maxRadDelta / angle;
    _vector3 mixed = Math::NormalizeSafeXZ(curDir * (1.f - weight) + targetDir * weight);
    return mixed;
}

CPedestrianNpc* CPedestrianNpc::Create()
{
    CPedestrianNpc* instance = new CPedestrianNpc();

    if (FAILED(instance->Initialize_Prototype()))
    {
        Safe_Release(instance);
        MSG_BOX("Failed to create : CPedestrianNpc");
    }

    return instance;
}

CGameObject* CPedestrianNpc::Clone(INIT_DESC* pArg)
{
    CPedestrianNpc* instance = new CPedestrianNpc(*this);
    if (FAILED(instance->Initialize(pArg)))
    {
        Safe_Release(instance);
        return nullptr;
    }
    return instance;
}

void CPedestrianNpc::Free()
{
    __super::Free();
}