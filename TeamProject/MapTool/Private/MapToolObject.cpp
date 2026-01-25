#include "pch.h"
#include "PlacedObject.h"
#include "RayReceiver.h"

#include "GameInstance.h"


CMapToolObject::CMapToolObject()
{
}

CMapToolObject::CMapToolObject(const CMapToolObject& rhs)
	:CGameObject(rhs)
{
}

HRESULT CMapToolObject::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	//Add_Component<CRayReceiver>();

	return S_OK;
}

HRESULT CMapToolObject::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	return S_OK;
}

void CMapToolObject::Awake()
{
	// (!!포인터 접근해서 값 변경) 만들어 지면 무조건 Inspector창에 띄움.
	CGameInstance::GetInstance()->Get_GUISystem()->Get_Context()->pSelectedObject = this;
}

void CMapToolObject::Priority_Update(_float dt)
{
}

void CMapToolObject::Update(_float dt)
{
}

void CMapToolObject::Late_Update(_float dt)
{
}

void CMapToolObject::Render_GUI()
{
	if (ImGui::TreeNode("Component Inspector")) {
		__super::Render_GUI();
		ImGui::TreePop();
	}
	ImGui::Separator();

	string TagID = "Object ID : " + to_string(m_iObjectIndex);
	ImGui::Text(TagID.c_str());
}

void CMapToolObject::Delete_Object()
{
	CGameInstance::GetInstance()->Get_ObjectMgr()->Remove_Object(this);
	CGameInstance::GetInstance()->Get_GUISystem()->Get_Context()->pSelectedObject = nullptr;
}

void CMapToolObject::Free()
{
	__super::Free();
}