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


	return S_OK;
}

HRESULT CMapObject::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	return S_OK;
}

void CMapObject::Awake()
{
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
}

void CMapObject::Free()
{
	__super::Free();
}