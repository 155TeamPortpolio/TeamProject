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
    float ExpAlpha(float speed, float dt)
    {
        float a = 1.f - expf(-speed * dt);
        return clamp(a, 0.f, 1.f);
    }

    float WrapRad(float rad)
    {
        while (rad > XM_PI) rad -= XM_2PI;
        while (rad < -XM_PI) rad += XM_2PI;
        return rad;
    }

    Vector2 MoveTowardsVec2(const Vector2& cur, const Vector2& target, float maxDelta)
    {
        Vector2 d = target - cur;
        const float len = d.Length();
        if (len <= maxDelta || len == 0.f) return target;
        d /= len;
        return cur + d * maxDelta;
    }

    Vector3 MoveTowardsVec3(const Vector3& cur, const Vector3& target, float maxDelta)
    {
        Vector3 d = target - cur;
        const float len = d.Length();
        if (len <= maxDelta || len == 0.f) return target;
        d /= len;
        return cur + d * maxDelta;
    }

    Vector3 DirFromYaw(float yawRad)
    {
        return Vector3(sinf(yawRad), 0.f, cosf(yawRad));
    }
}

HRESULT CUI_MeshPyramid::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	ResourceManager()->Add_ResourcePath("pyramid.model", "../bin/Resources/Global/UI/Model/UI_3DPyramid/pyramid.model");
	ResourceManager()->Add_ResourcePath("pyramid.mat", "../bin/Resources/Global/UI/Model/UI_3DPyramid/pyramid.mat");
	Add_Component<CStaticModel>()->Link_Model(G_GlobalLevelKey, "pyramid.model");
	Add_Component<CMaterial>()->Link_Material(G_GlobalLevelKey, "pyramid.mat");

	return S_OK;
}

HRESULT CUI_MeshPyramid::Initialize(INIT_DESC* arg)
{
    __super::Initialize(arg);

    m_alpha = m_cfg.baseColorAlpha.w;
    m_color = Vector3(m_cfg.baseColorAlpha.x, m_cfg.baseColorAlpha.y, m_cfg.baseColorAlpha.z);

    auto mtrl = Get_Component<CMaterial>();
    auto mtrlInsts = mtrl->Get_MaterialInstances();
    for (auto& inst : mtrlInsts)
    {
        inst->Set_Param("color", {&m_color, "float3", sizeof(_float3)});
        inst->Set_Param("alpha", {&m_alpha, "float", sizeof(_float)});
    }

    m_pTransform->Scale({0.05f, 0.1f, 0.05f});

    auto model = Get_Component<CStaticModel>();
    model->Set_RenderType(RENDER_PASS_TYPE::RENDER_3DUI);
    model->ShadowCast(false);

    return S_OK;
}

void CUI_MeshPyramid::Update(_float dt)
{
    if (!IsOnScreen(m_cfg.onScreenMarginPx))
    {
        m_rt.fadeT += dt / m_cfg.fadeInDur;
        if (m_rt.fadeT > 1.f) m_rt.fadeT = 1.f;
    }
    else
    {
        m_rt.fadeT -= dt / m_cfg.fadeOutDur;
        if (m_rt.fadeT < 0.f) m_rt.fadeT = 0.f;
    }

    const float eased = Math::ApplyEase(EaseType::InOutSine, m_rt.fadeT);
    m_alpha = m_cfg.baseColorAlpha.w * eased;

    //rt.isAlert = IsAlert();

    if (!m_rt.isAlert)
    {
        m_color = m_cfg.gray;
        m_rt.alertBlinkT = 0.f;
    }
    else
    {
        m_rt.alertBlinkT += dt;
        const float period = m_cfg.blinkSec > 0.f ? m_cfg.blinkSec : 0.0001f;
        const int phase = (int)(m_rt.alertBlinkT / period);
        m_color = (phase & 1) ? m_cfg.red : m_cfg.gray;
    }
    auto child = Get_Component<CChild>();
    if (!child)
        return;
    auto parentObj = child->Get_Parent();

    OBJECT_HANDLE hChar = BattleSystem()->GetCurCharacterHandle();
    auto charObj = ObjectManager()->Request_Object(hChar);
    auto charCC = charObj->Get_Component<CCharacterController>();

    const Vector4 foot4 = charCC->Get_FootPosition();
    Vector3 foot(foot4.x, foot4.y, foot4.z);
    foot.y += charCC->Get_HalfSize() * 0.5f;

    const Vector3 targetPos3 = parentObj->Get_WorldPos();

    Vector2 footXZ(foot.x, foot.z);
    Vector2 targetXZ(targetPos3.x, targetPos3.z);

    if (!m_rt.hasLastFootXZ)
    {
        m_rt.lastFootXZ = footXZ;
        m_rt.hasLastFootXZ = true;
    }
    else
    {
        m_rt.lastFootXZ = MoveTowardsVec2(m_rt.lastFootXZ, footXZ, m_cfg.maxFootStepPerFrame);
    }

    if (!m_rt.hasLastTargetXZ)
    {
        m_rt.lastTargetXZ = targetXZ;
        m_rt.hasLastTargetXZ = true;
    }
    else
    {
        m_rt.lastTargetXZ = MoveTowardsVec2(m_rt.lastTargetXZ, targetXZ, m_cfg.maxTargetStepPerFrame);
    }

    const Vector3 stableFoot(m_rt.lastFootXZ.x, foot.y, m_rt.lastFootXZ.y);
    const Vector3 stableTarget(m_rt.lastTargetXZ.x, targetPos3.y, m_rt.lastTargetXZ.y);

    Vector3 dir = stableTarget - stableFoot;
    dir.y = 0.f;

    const float dirLen = dir.Length();

    if (dirLen >= m_cfg.minDirLen)
    {
        dir /= dirLen;
        m_rt.lastDirXZ = dir;
        m_rt.hasLastDir = true;
    }
    else
    {
        if (m_rt.hasLastDir) dir = m_rt.lastDirXZ;
        else dir = DirFromYaw(m_rt.hasLastYaw ? m_rt.lastYawRad : 0.f);
    }

    const Vector3 rawPos = stableFoot + dir * m_cfg.ringRadius + Vector3(0.f, m_cfg.yOffset, 0.f);
    const float rawYawRad = atan2f(dir.x, dir.z);

    if (!m_rt.hasLastYaw)
    {
        m_rt.lastYawRad = rawYawRad;
        m_rt.hasLastYaw = true;
    }
    else
    {
        const float a = ExpAlpha(m_cfg.yawSmoothSpeed, dt);
        const float delta = WrapRad(rawYawRad - m_rt.lastYawRad);
        m_rt.lastYawRad = WrapRad(m_rt.lastYawRad + delta * a);
    }

    if (!m_rt.hasSmoothPos)
    {
        m_rt.smoothPos = rawPos;
        m_rt.hasSmoothPos = true;
    }
    else
    {
        const float a = ExpAlpha(m_cfg.posSmoothSpeed, dt);
        Vector3 nextPos = Vector3::Lerp(m_rt.smoothPos, rawPos, a);
        m_rt.smoothPos = MoveTowardsVec3(m_rt.smoothPos, nextPos, m_cfg.maxPosStepPerFrame);
    }

    auto tf = Get_Component<CTransform>();
    tf->Set_WorldPos(Vector4(m_rt.smoothPos.x, m_rt.smoothPos.y, m_rt.smoothPos.z, 1.f));

    const Quaternion q = Quaternion::CreateFromYawPitchRoll(m_rt.lastYawRad, m_cfg.basePitchRad, 0.f);
    tf->Set_WorldQuaternion(Vector4(q.x, q.y, q.z, q.w));
}

_bool CUI_MeshPyramid::IsOnScreen(_float marginPx)
{
    if (!Get_Component<CChild>())
        return false;

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