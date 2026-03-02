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
#include "ObjectContainer.h"
// Client
#include "BattleSystem.h"
#include "BattlePlayer.h"
#include "Player.h"
#include "MaterialInstance.h"
#include "Enemy.h"

namespace
{
    Vector3 DirFromYaw(float yawRad)
    {
        return Vector3(sinf(yawRad), 0.f, cosf(yawRad));
    }
}

HRESULT CUI_MeshPyramid::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	ResourceManager()->Add_ResourcePath("pyramid.model", "../bin/Resources/Global/UI/Model/UI_3DPyramid/pyramid.model");
	ResourceManager()->Add_ResourcePath("pyramid.mat",   "../bin/Resources/Global/UI/Model/UI_3DPyramid/pyramid.mat");
	Add_Component<CStaticModel>()->Link_Model(G_GlobalLevelKey, "pyramid.model");
	Add_Component<CMaterial>()->Link_Material(G_GlobalLevelKey, "pyramid.mat");
	return S_OK;
}

HRESULT CUI_MeshPyramid::Initialize(INIT_DESC* arg)
{
    __super::Initialize(arg);

    m_alpha = m_cfg.baseColorAlpha.w;
    m_color = m_cfg.gray;

    static Vector3 fillDark(0.35f, 0.35f, 0.38f);
    static Vector3 fillLight(0.90f, 0.90f, 0.92f);
    static Vector3 outlineColor(0.03f, 0.03f, 0.03f);

    static _float rimStart = 0.55f;
    static _float rimEnd = 0.92f;
    static _float rimStrength = 0.25f;
    static _float edgeHardness = 1.5f;

    m_topRed = Vector3(0.55f, 0.08f, 0.10f);
    m_topWhite = Vector3(1.f, 1.f, 1.f);
    m_topBlink = -1.f;

    static _float topNStart = 0.85f;
    static _float topNEnd = 0.98f;
    static _float topNPow = 6.0f;

    auto mtrl = Get_Component<CMaterial>();
    auto mtrlInsts = mtrl->Get_MaterialInstances();
    for (auto& inst : mtrlInsts)
    {
        inst->Set_Param("color", {&m_color, "float3", sizeof(_float3)});
        inst->Set_Param("alpha", {&m_alpha, "float", sizeof(_float)});

        inst->Set_Param("fillDark", {&fillDark, "float3", sizeof(_float3)});
        inst->Set_Param("fillLight", {&fillLight, "float3", sizeof(_float3)});
        inst->Set_Param("outlineColor", {&outlineColor, "float3", sizeof(_float3)});

        inst->Set_Param("rimStart", {&rimStart, "float", sizeof(_float)});
        inst->Set_Param("rimEnd", {&rimEnd, "float", sizeof(_float)});
        inst->Set_Param("rimStrength", {&rimStrength, "float", sizeof(_float)});
        inst->Set_Param("edgeHardness", {&edgeHardness, "float", sizeof(_float)});

        inst->Set_Param("topBlink", {&m_topBlink, "float", sizeof(_float)});
        inst->Set_Param("topRed", {&m_topRed, "float3", sizeof(_float3)});
        inst->Set_Param("topWhite", {&m_topWhite, "float3", sizeof(_float3)});
        inst->Set_Param("topNStart", {&topNStart, "float", sizeof(_float)});
        inst->Set_Param("topNEnd", {&topNEnd, "float", sizeof(_float)});
        inst->Set_Param("topNPow", {&topNPow, "float", sizeof(_float)});
    }

    static constexpr _float width = 0.03f;

    m_pTransform->Scale({width, width * 2.f, width});

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

    m_alpha = m_cfg.baseColorAlpha.w * Math::ApplyEase(EaseType::InOutSine, m_rt.fadeT);

    auto child = Get_Component<CChild>();
    if (!child) return;

    auto parentObj = child->Get_Parent();
    if (!parentObj) return;

    const bool isAlert = static_cast<CEnemy*>(parentObj)->IsOnAttack();
    m_rt.isAlert = isAlert;

    m_color = m_cfg.gray;

    if (!isAlert)
    {
        m_rt.alertBlinkT = 0.f;
        m_topBlink = -1.f;
    }
    else
    {
        m_rt.alertBlinkT += dt;
        const float period = m_cfg.blinkSec > 0.f ? m_cfg.blinkSec : 0.0001f;
        const float w = 6.2831853f / period;
        m_topBlink = 0.5f + 0.5f * sinf(m_rt.alertBlinkT * w);
    }

    OBJECT_HANDLE hChar = BattleSystem()->GetCurCharacterHandle();
    auto charObj = ObjectManager()->Request_Object(hChar);
    auto charCC = charObj->Get_Component<CCharacterController>();

    const Vector3 foot = charCC->Get_FootPosition();

    const Vector3 targetPos = parentObj->Get_WorldPos();
    const Vector3 charPos = charObj->Get_WorldPos();

    const float anchorY = foot.y + charCC->Get_HalfSize() * m_cfg.anchorUpRatio + m_cfg.anchorUpBias;

    Vector3 base(charPos.x, anchorY, charPos.z);

    Vector3 dir = targetPos - base;
    dir.y = 0.f;

    const float dirLenSq = dir.LengthSquared();
    if (dirLenSq <= 1e-10f) dir = Vector3(0.f, 0.f, 1.f);
    else dir /= sqrtf(dirLenSq);

    const float yawRad = atan2f(dir.x, dir.z);

    const Vector3 pos = base + dir * m_cfg.ringRadius + Vector3(0.f, m_cfg.yOffset, 0.f);

    auto tf = Get_Component<CTransform>();
    tf->Set_WorldPos(Vector4(pos.x, pos.y, pos.z, 1.f));

    const Quaternion q = Quaternion::CreateFromYawPitchRoll(yawRad, m_cfg.basePitchRad, 0.f);
    tf->Set_WorldQuaternion(q);
}

_bool CUI_MeshPyramid::IsOnScreen(_float marginPx)
{
    auto child = Get_Component<CChild>();
    if (!child) return false;

    auto parent = child->Get_Parent();
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
    screen.x = viewport.x + ( ndcX * 0.5f + 0.5f) * viewport.z;
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
	auto inst = new CUI_MeshPyramid;
	if (FAILED(inst->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUI_MeshPyramid");
		Safe_Release(inst);
	}
	return inst;
}

CGameObject* CUI_MeshPyramid::Clone(INIT_DESC* pArg)
{
	auto inst = new CUI_MeshPyramid(*this);
	if (FAILED(inst->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CUI_MeshPyramid");
		Safe_Release(inst);
	}
	return inst;
}