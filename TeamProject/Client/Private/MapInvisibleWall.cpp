#include "pch.h"
#include "MapInvisibleWall.h"
#include "GameInstance.h"
#include "MapData_Defines.h"
#include "MapLoader_Helper.h"

#include "Material.h"
#include "EffectContainer.h"

CMapInvisibleWall::CMapInvisibleWall()
{
}

CMapInvisibleWall::CMapInvisibleWall(const CMapInvisibleWall& rhs)
{
}

HRESULT CMapInvisibleWall::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CMaterial>();

	return S_OK;
}

HRESULT CMapInvisibleWall::Initialize(INIT_DESC* pArg)
{
	return S_OK;
}

void CMapInvisibleWall::Awake()
{
}

void CMapInvisibleWall::Priority_Update(_float dt)
{
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

void CMapInvisibleWall::Free()
{
	__super::Free();
}
