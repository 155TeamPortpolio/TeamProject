#include "pch.h"
#include "UI_MeshPyramid.h"
#include "GameInstance.h"
#include "Helper_Func.h"
// Component
#include "StaticModel.h"
#include "CharacterController.h"
#include "Material.h"
#include "Child.h"
#include "Camera.h"
// Client
#include "BattleSystem.h"
#include "BattlePlayer.h"
#include "Player.h"
#include "MaterialInstance.h"

namespace
{
    float WrapRad(float a)
    {
        constexpr float kPi = 3.14159265359f;
        constexpr float kTwoPi = 6.28318530718f;

        while (a > kPi) a -= kTwoPi;
        while (a < -kPi) a += kTwoPi;
        return a;
    }

    float LerpAngleRad(float from, float to, float t)
    {
        float d = WrapRad(to - from);
        return WrapRad(from + d * t);
    }

    float ExpSmoothingFactor(float dt, float speed)
    {
        return 1.f - expf(-speed * dt);
    }
}

HRESULT CUI_MeshPyramid::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    ResourceManager()->Add_ResourcePath("pyramid.model", "../bin/Resources/UI/Model/UI_3DPyramid/pyramid.model");
    ResourceManager()->Add_ResourcePath("pyramid.mat", "../bin/Resources/UI/Model/UI_3DPyramid/pyramid.mat");
    Add_Component<CStaticModel>()->Link_Model(G_GlobalLevelKey, "pyramid.model");
    Add_Component<CMaterial>()->Link_Material(G_GlobalLevelKey, "pyramid.mat");

    return S_OK;
}

HRESULT CUI_MeshPyramid::Initialize(INIT_DESC* arg)
{
    __super::Initialize(arg);

    rt.colorAlpha = cfg.baseColorAlpha;

    auto mtrl = Get_Component<CMaterial>();
    auto mtrlInsts = mtrl->Get_MaterialInstances();
    for (auto& inst : mtrlInsts) inst->Set_Param("colorAlpha", {&rt.colorAlpha, "float4", sizeof(Vector4)});

    m_pTransform->Scale({0.05f, 0.1f, 0.05f});

    auto model = Get_Component<CStaticModel>();
    model->Set_RenderType(RENDER_PASS_TYPE::RENDER_3DUI);
    model->ShadowCast(false);

    return S_OK;
}

void CUI_MeshPyramid::Update(_float dt)
{
    if (InputDevice()->Mouse_Tap(MOUSE_BTN::LB)) rt.isAlert = !rt.isAlert;

    UpdateFade(dt);

    const Vector3 baseColor = UpdateAlertBlink(dt);

    ApplyColorAlpha(baseColor);

    UpdateFollow(dt);
}

void CUI_MeshPyramid::UpdateFade(_float dt)
{
    const _bool isVisible = !IsOnScreen(cfg.onScreenMarginPx);

    if (isVisible)
    {
        rt.fadeT += dt / cfg.fadeInDur;
        if (rt.fadeT > 1.f) rt.fadeT = 1.f;
    }
    else
    {
        rt.fadeT -= dt / cfg.fadeOutDur;
        if (rt.fadeT < 0.f) rt.fadeT = 0.f;
    }
}

Vector3 CUI_MeshPyramid::UpdateAlertBlink(_float dt)
{
    if (!rt.isAlert)
    {
        rt.alertBlinkT = 0.f;
        return cfg.gray;
    }

    rt.alertBlinkT += dt;

    const float cycle = cfg.blinkSec * 2.f;
    while (rt.alertBlinkT >= cycle) rt.alertBlinkT -= cycle;

    const _bool flashRed = rt.alertBlinkT < cfg.blinkSec;
    return flashRed ? cfg.red : cfg.gray;
}

void CUI_MeshPyramid::ApplyColorAlpha(const Vector3& baseColor)
{
    const float easedA = Math::ApplyEase(EaseType::InOutSine, rt.fadeT);

    rt.colorAlpha.x = baseColor.x;
    rt.colorAlpha.y = baseColor.y;
    rt.colorAlpha.z = baseColor.z;
    rt.colorAlpha.w = easedA;
}

void CUI_MeshPyramid::UpdateFollow(_float dt)
{
    auto parentObj = Get_Component<CChild>()->Get_Parent();

    OBJECT_HANDLE hChar = BattleSystem()->GetCurCharacterHandle();

    auto charObj = ObjectManager()->Request_Object(hChar);
    auto charCC = charObj->Get_Component<CCharacterController>();

    const Vector4 foot4 = charCC->Get_FootPosition();
    Vector3 foot(foot4.x, foot4.y, foot4.z);

    const float yFixed = foot.y + charCC->Get_HalfSize() * 0.5f + cfg.yOffset;

    const Vector3 targetPos3 = parentObj->Get_WorldPos();

    Vector2 footXZ(foot.x, foot.z);
    Vector2 targetXZ(targetPos3.x, targetPos3.z);

    const float aPos = ExpSmoothingFactor(dt, cfg.posSmoothSpeed);

    if (!rt.hasLastFootXZ)
    {
        rt.lastFootXZ = footXZ;
        rt.hasLastFootXZ = true;
    }
    else rt.lastFootXZ = rt.lastFootXZ + (footXZ - rt.lastFootXZ) * aPos;

    if (!rt.hasLastTargetXZ)
    {
        rt.lastTargetXZ = targetXZ;
        rt.hasLastTargetXZ = true;
    }
    else rt.lastTargetXZ = rt.lastTargetXZ + (targetXZ - rt.lastTargetXZ) * aPos;

    Vector2 dir2 = rt.lastTargetXZ - rt.lastFootXZ;

    const float len2 = dir2.Length();
    if (len2 <= cfg.minDirLen) return;

    dir2 /= len2;

    const Vector3 pos(rt.lastFootXZ.x + dir2.x * cfg.ringRadius, yFixed, rt.lastFootXZ.y + dir2.y * cfg.ringRadius);

    auto tf = Get_Component<CTransform>();
    tf->Set_WorldPos(Vector4(pos.x, pos.y, pos.z, 1.f));

    float yawRad = atan2f(dir2.x, dir2.y);

    if (!rt.hasLastYaw)
    {
        rt.lastYawRad = yawRad;
        rt.hasLastYaw = true;
    }
    else
    {
        const float aYaw = ExpSmoothingFactor(dt, cfg.yawSmoothSpeed);
        rt.lastYawRad = LerpAngleRad(rt.lastYawRad, yawRad, aYaw);
    }

    const Quaternion q = Quaternion::CreateFromYawPitchRoll(rt.lastYawRad, cfg.basePitchRad, 0.f);

    tf->Set_WorldQuaternion(Vector4(q.x, q.y, q.z, q.w));
}
_bool CUI_MeshPyramid::IsOnScreen(_float marginPx)
{
    auto parent = Get_Component<CChild>()->Get_Parent();
    if (!parent) return false;

    const Vector3 worldPos = parent->Get_WorldPos();

    const Matrix view = *CameraManager()->Get_ViewMatrix();
    const Matrix proj = *CameraManager()->Get_ProjMatrix();

    const Vector2 client = GameInstance()->Get_ClientSize();
    const Vector4 viewport(0.f, 0.f, client.x, client.y);

    const Matrix vp = view * proj;

    Vector4 clip(worldPos.x, worldPos.y, worldPos.z, 1.f);
    clip = Vector4::Transform(clip, vp);

    if (clip.w <= 0.f) return false;

    const float invW = 1.f / clip.w;
    const float ndcX = clip.x * invW;
    const float ndcY = clip.y * invW;
    const float ndcZ = clip.z * invW;

    if (ndcZ < 0.f) return false;
    if (ndcZ > 1.f) return false;

    Vector2 screen{};
    screen.x = viewport.x + (ndcX * 0.5f + 0.5f) * viewport.z;
    screen.y = viewport.y + (-ndcY * 0.5f + 0.5f) * viewport.w;

    const float minX = viewport.x - marginPx;
    const float maxX = viewport.x + viewport.z + marginPx;
    const float minY = viewport.y - marginPx;
    const float maxY = viewport.y + viewport.w + marginPx;

    if (screen.x < minX) return false;
    if (screen.x > maxX) return false;
    if (screen.y < minY) return false;
    if (screen.y > maxY) return false;

    return true;
}

CGameObject* CUI_MeshPyramid::Create()
{
    CUI_MeshPyramid* pInstance = new CUI_MeshPyramid;
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_MeshPyramid");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_MeshPyramid::Clone(INIT_DESC* pArg)
{
    CUI_MeshPyramid* pInstance = new CUI_MeshPyramid(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_MeshPyramid");
        Safe_Release(pInstance);
    }
    return pInstance;
}