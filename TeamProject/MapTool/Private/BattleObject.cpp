#include "pch.h"
#include "BattleObject.h"
#include "GameInstance.h"

#include "Collider.h"

CBattleObject::CBattleObject()
	: CMapToolObject()
{
}

CBattleObject::CBattleObject(const CBattleObject& rhs)
	: CMapToolObject(rhs)
{
}

HRESULT CBattleObject::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CCollider>();

	return S_OK;
}

HRESULT CBattleObject::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	Get_Component<CCollider>()->Set_MapToolMode(true);

	return S_OK;
}

void CBattleObject::Awake()
{
	__super::Awake();
}

void CBattleObject::Priority_Update(_float dt)
{
}

void CBattleObject::Update(_float dt)
{
	Get_Component<CCollider>()->Update(dt);
}

void CBattleObject::Late_Update(_float dt)
{
}

void CBattleObject::Render_GUI()
{
	ImGui::PushID(this);

	__super::Render_GUI();

	ImGui::PopID();
}

void CBattleObject::Free()
{
	__super::Free();
}
