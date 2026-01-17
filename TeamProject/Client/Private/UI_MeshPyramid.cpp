#include "pch.h"
#include "UI_MeshPyramid.h"
#include "GameInstance.h"
// Component
#include "StaticModel.h"
#include "CharacterController.h"
#include "Material.h"
// Client
#include "BattleSystem.h"
#include "BattlePlayer.h"
#include "Player.h"

HRESULT CUI_MeshPyramid::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	ResourceManager()->Add_ResourcePath("pyramid.model", "../bin/Resources/UI/Model/UI_3DPyramid/pyramid.model");
	ResourceManager()->Add_ResourcePath("pyramid.mat", "../bin/Resources/UI/Model/UI_3DPyramid/pyramid.mat");
	Add_Component<CStaticModel>()->Link_Model(G_GlobalLevelKey, "pyramid.model");
	Add_Component<CMaterial>()->Link_Material(G_GlobalLevelKey, "pyramid.mat");
	return S_OK;
}

HRESULT CUI_MeshPyramid::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	auto model = Get_Component<CStaticModel>();
	model->Set_RenderType(RENDER_PASS_TYPE::RENDER_OPAQUE);

	return S_OK;
}

void CUI_MeshPyramid::Update(_float dt)
{
	auto battle = BattleSystem();
	auto player = battle->GetBattlePlayer();

	OBJECT_HANDLE hChar = battle->GetCurCharacterHandle();
	OBJECT_HANDLE hTarget = player->GetTargetHandle();

	if (!hChar.isValid() || !hTarget.isValid()) return;

	auto charObj = ObjectManager()->Request_Object(hChar);
	auto charCC = charObj->Get_Component<CCharacterController>();

	const Vector4 foot4 = charCC->Get_FootPosition();
	const Vector3 foot(foot4.x, foot4.y, foot4.z);

	auto targetObj = ObjectManager()->Request_Object(hTarget);
	const Vector3 targetPos = targetObj->Get_WorldPos();

	Vector3 dir = targetPos - foot;
	dir.y = 0.f;

	const float len = dir.Length();
	if (len == 0.f) return;

	dir /= len;

	const float ringRadius = 0.9f;
	const float yOffset = 0.03f;

	const Vector3 pos = foot + dir * ringRadius + Vector3(0.f, yOffset, 0.f);

	auto tf = Get_Component<CTransform>();
	tf->Set_WorldPos(Vector4(pos.x, pos.y, pos.z, 1.f));

	const Vector3 lookAt(targetPos.x, pos.y, targetPos.z);
	tf->LookAt(Vector4(lookAt.x, lookAt.y, lookAt.z, 1.f));
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