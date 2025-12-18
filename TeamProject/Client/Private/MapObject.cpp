#include "pch.h"
#include "MapObject.h"
#include "RayReceiver.h"

#include "GameInstance.h"


CMapObject::CMapObject()
{
}

CMapObject::CMapObject(const CMapObject& rhs)
	:CGameObject(rhs)
{
}

HRESULT CMapObject::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	//Add_Component<CRayReceiver>();

	return S_OK;
}

HRESULT CMapObject::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	return S_OK;
}

void CMapObject::Awake()
{
	// (!!포인터 접근해서 값 변경) 만들어 지면 무조건 Inspector창에 띄움.
	//CGameInstance::GetInstance()->Get_GUISystem()->Get_Context()->pSelectedObject = this;
}

void CMapObject::Priority_Update(_float dt)
{
}

void CMapObject::Update(_float dt)
{
}

void CMapObject::Late_Update(_float dt)
{
}

void CMapObject::Render_GUI()
{

	__super::Render_GUI();

	if (ImGui::Button("Delete")) {
		Delete_Object();
	}


}

void CMapObject::Delete_Object()
{
	CGameInstance::GetInstance()->Get_ObjectMgr()->Remove_Object(this);
	CGameInstance::GetInstance()->Get_GUISystem()->Get_Context()->pSelectedObject = nullptr;
}

void CMapObject::Free()
{
	__super::Free();
}