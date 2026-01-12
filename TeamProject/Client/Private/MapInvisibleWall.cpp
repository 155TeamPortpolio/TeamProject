#include "pch.h"
#include "MapInvisibleWall.h"
#include "GameInstance.h"
#include "MapData_Defines.h"
#include "MapLoader_Helper.h"

#include "ModelData.h"

#include "Material.h"

CMapInvisibleWall::CMapInvisibleWall()
{
}

CMapInvisibleWall::CMapInvisibleWall(const CMapInvisibleWall& rhs)
{
}

HRESULT CMapInvisibleWall::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CMapInvisibleWall::Initialize(INIT_DESC* pArg)
{
	return E_NOTIMPL;
}

void CMapInvisibleWall::Awake()
{
}

void CMapInvisibleWall::Priority_Update(_float dt)
{
	//Update Sprite Textrue
}

void CMapInvisibleWall::Update(_float dt)
{
}

void CMapInvisibleWall::Late_Update(_float dt)
{
}

void CMapInvisibleWall::Render_GUI()
{
}

CGameObject* CMapInvisibleWall::Clone(INIT_DESC* pArg)
{
	return nullptr;
}
