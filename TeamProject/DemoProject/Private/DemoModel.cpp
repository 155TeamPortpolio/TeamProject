#include "pch.h"
#include "DemoModel.h"
#include "SkeletalModel.h"
#include "Animator3D.h"
#include "Material.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

#include "RigidBody.h"
CDemoModel::CDemoModel()
{
}

CDemoModel::CDemoModel(const CDemoModel& rhs)
	:CGameObject(rhs) 
{
}

HRESULT CDemoModel::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CSkeletalModel>();
	Add_Component<CAnimator3D>();
	Add_Component<CMaterial>();
	Add_Component<CObjectContainer>();
	Add_Component<CRigidBody>();
	Add_Component<CCollider>();

	auto pRcsMgr = CGameInstance::GetInstance()->Get_ResourceMgr();

	/*파일명과 키값은 일치*/
	pRcsMgr->Add_ResourcePath("Bangboo_Sharkboo_NPC (merge).model",
		"../../DemoResource/new/Bangboo_Sharkboo_NPC (merge).model");
	pRcsMgr->Add_ResourcePath("Bangboo_Sharkboo_NPC (merge).mat",
		"../../DemoResource/new/Bangboo_Sharkboo_NPC (merge).mat");
	pRcsMgr->Add_ResourcePath("Bangboo_Sharkboo_Meta.json",
		"../../DemoResource/new/Anim/Bangboo_Sharkboo_Meta.json");

	Get_Component<CModel>()->Link_Model("Demo_Level", "Bangboo_Sharkboo_NPC (merge).model");
	Get_Component<CMaterial>()->Link_Material("Demo_Level", "Bangboo_Sharkboo_NPC (merge).mat");

	return S_OK;
}

HRESULT CDemoModel::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	GAMEOBJECT_DESC* pObjDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

	return S_OK;
}

void CDemoModel::Awake()
{
	Get_Component<CAnimator3D>()->LinkAnimate_Model("Demo_Level", "Bangboo_Sharkboo_NPC (merge).model");
	Get_Component<CAnimator3D>()->Link_MetaData("Demo_Level", "Bangboo_Sharkboo_Meta.json");
	Get_Component<CAnimator3D>()->Set_Animation(0, 3);
}

void CDemoModel::Priority_Update(_float dt)
{
}

void CDemoModel::Update(_float dt)
{
	Get_Component<CAnimator3D>()->Update_Animation(dt);
}

void CDemoModel::Late_Update(_float dt)
{
	Get_Component<CRigidBody>()->Late_Update(dt);
}

void CDemoModel::OnCollisionEnter()
{

}

void CDemoModel::OnCollisionStay()
{
}

void CDemoModel::OnCollisionExit()
{
}

void CDemoModel::Render_GUI()
{
	__super::Render_GUI();

	if (ImGui::Button("Add")) {
		CGameObject* DemoModel = Builder::Create_Object({ "Demo_Level" ,"Proto_GameObject_DemoModel" })
			.Position({ 0,0,0 })
			.Build("Demo_Model");
		Get_Component<CObjectContainer>()->Add_Child(DemoModel,false);
	}
	_bool isLayer = Get_Layer();
	ImGui::Checkbox("InLayer",&isLayer);
}

CDemoModel* CDemoModel::Create()
{
	CDemoModel* instance = new CDemoModel();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CDemoModel");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CDemoModel::Clone(INIT_DESC* pArg)
{
	CDemoModel* instance = new CDemoModel(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CDemoModel");
		Safe_Release(instance);
	}

	return instance;
}

void CDemoModel::Free()
{
	__super::Free();
}