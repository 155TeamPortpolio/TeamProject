#include "pch.h"
#include "MapInvisibleWall.h"
#include "GameInstance.h"
#include "MapData_Defines.h"
#include "MapLoader_Helper.h"

#include "Material.h"
#include "EffectContainer.h"

CMapInvisibleWall::CMapInvisibleWall()
	:CMapObject()
{
}

CMapInvisibleWall::CMapInvisibleWall(const CMapInvisibleWall& rhs)
	:CMapObject(rhs)
{
}

HRESULT CMapInvisibleWall::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CCollider>();
	return S_OK;
}

HRESULT CMapInvisibleWall::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

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
	Get_Component<CCollider>()->Update(dt);
}

void CMapInvisibleWall::Late_Update(_float dt)
{
}

void CMapInvisibleWall::Export_ObjectData(void* pDesc)
{
	MapData_Object* pObjectDesc = static_cast<MapData_Object*>(pDesc);

	pObjectDesc->TagModelResourceKey = m_InstanceName;
	pObjectDesc->TagMaterialResourceKey = "";

	_float4x4 matWorld = Get_Component<CTransform>()->Get_WorldMatrix();

	pObjectDesc->vRight = { matWorld._11,matWorld._12, matWorld._13, matWorld._14 };
	pObjectDesc->vUp = { matWorld._21,matWorld._22, matWorld._23, matWorld._24 };
	pObjectDesc->vLook = { matWorld._31,matWorld._32, matWorld._33, matWorld._34 };
	pObjectDesc->vPos = { matWorld._41,matWorld._42, matWorld._43, matWorld._44 };
}

void CMapInvisibleWall::Render_GUI()
{
}

CMapInvisibleWall* CMapInvisibleWall::Create()
{
	CMapInvisibleWall* pInstance = new CMapInvisibleWall();
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}


CGameObject* CMapInvisibleWall::Clone(INIT_DESC* pArg)
{
	CMapInvisibleWall* instance = new CMapInvisibleWall(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CMapInvisibleWall");
		Safe_Release(instance);
	}

	return instance;
}

void CMapInvisibleWall::Free()
{
	__super::Free();
}
