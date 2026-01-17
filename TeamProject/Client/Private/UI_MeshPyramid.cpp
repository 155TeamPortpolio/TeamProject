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

    auto mtrl = Get_Component<CMaterial>();
    auto mtrlInsts = mtrl->Get_MaterialInstances();
    for (auto& inst : mtrlInsts)
    {
        inst->Set_Param("alpha",     {&alpha,     "float",  sizeof(_float)});
        inst->Set_Param("color",     {&color,     "float3", sizeof(_float3)});
        inst->Set_Param("alphaTest", {&alphaTest, "float",  sizeof(_float)});
    }

    m_pTransform->Scale({0.05f, 0.1f, 0.05f});

    auto model = Get_Component<CStaticModel>();
    model->Set_RenderType(RENDER_PASS_TYPE::RENDER_OPAQUE);
    model->ShadowCast(false);

    return S_OK;
}

void CUI_MeshPyramid::Update(_float dt)
{
    const _bool isVisible = IsOnScreen(20.f);

    if (isVisible)
    {
        fadeT += dt / fadeInDur;
        if (fadeT > 1.f) fadeT = 1.f;
    }
    else
    {
        fadeT -= dt / fadeOutDur;
        if (fadeT < 0.f) fadeT = 0.f;
    }

    alpha = Math::ApplyEase(EaseType::InOutSine, fadeT);

    color = {0.38f, 0.38f, 0.38f};

    auto parentObj = Get_Component<CChild>()->Get_Parent();

    OBJECT_HANDLE hChar = BattleSystem()->GetCurCharacterHandle();

    auto charObj = ObjectManager()->Request_Object(hChar);
    auto charCC = charObj->Get_Component<CCharacterController>();

    const Vector4 foot4 = charCC->Get_FootPosition();
    Vector3 foot(foot4.x, foot4.y, foot4.z);

    foot.y += charCC->Get_HalfSize() * 0.5f;

    const Vector3 targetPos = parentObj->Get_WorldPos();

    Vector3 dir = targetPos - foot;
    dir.y = 0.f;

    const float len = dir.Length();
    if (len == 0.f) return;

    dir /= len;

    const float ringRadius = 1.35f;
    const float yOffset = 0.03f;

    const Vector3 pos = foot + dir * ringRadius + Vector3(0.f, yOffset, 0.f);

    auto tf = Get_Component<CTransform>();
    tf->Set_WorldPos(Vector4(pos.x, pos.y, pos.z, 1.f));

    const float yawRad = atan2f(dir.x, dir.z);

    const float basePitchRad = XMConvertToRadians(-90.f);
    const float finalYawRad = yawRad + XM_PI;

    const Quaternion q = Quaternion::CreateFromYawPitchRoll(finalYawRad, basePitchRad, 0.f);

    tf->Set_Quaternion(XMVectorSet(q.x, q.y, q.z, q.w));
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

_bool CUI_MeshPyramid::IsAlert()
{
    return _bool();
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