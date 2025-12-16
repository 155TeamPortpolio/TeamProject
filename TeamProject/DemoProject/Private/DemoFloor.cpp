#include "pch.h"
#include "DemoFloor.h"
#include "StaticModel.h"
#include "Material.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

#include "RigidBody.h"

CDemoFloor::CDemoFloor()
{
}

CDemoFloor::CDemoFloor(const CDemoFloor& rhs)
	:CGameObject(rhs)
{
}

HRESULT CDemoFloor::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CObjectContainer>();
	Add_Component<CCollider>();

	return S_OK;
}

HRESULT CDemoFloor::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	GAMEOBJECT_DESC* pObjDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

	return S_OK;
}

void CDemoFloor::Awake()
{
}

void CDemoFloor::Priority_Update(_float dt)
{
}

void CDemoFloor::Update(_float dt)
{
	Get_Component<CCollider>()->Update(dt);
}

void CDemoFloor::Late_Update(_float dt)
{
}

void CDemoFloor::OnCollisionEnter()
{
}

void CDemoFloor::OnCollisionStay()
{
}

void CDemoFloor::OnCollisionExit()
{
}

void CDemoFloor::Render_GUI()
{
	__super::Render_GUI();

	if (ImGui::Button("Add")) {
		CGameObject* DemoFloor = Builder::Create_Object({ "Physics_Level" ,"Proto_GameObject_DemoFloor" })
			.Position({ 0,0,0 })
			.Build("Demo_Floor");
		Get_Component<CObjectContainer>()->Add_Child(DemoFloor, false);
	}
	_bool isLayer = Get_Layer();
	ImGui::Checkbox("InLayer", &isLayer);
}

CDemoFloor* CDemoFloor::Create()
{
	CDemoFloor* instance = new CDemoFloor();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CDemoFloor");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CDemoFloor::Clone(INIT_DESC* pArg)
{
	CDemoFloor* instance = new CDemoFloor(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CDemoFloor");
		Safe_Release(instance);
	}

	return instance;
}

void CDemoFloor::Free()
{
	__super::Free();
}
