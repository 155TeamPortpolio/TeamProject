#include "pch.h"
#include "MapInvisibleWall.h"
#include "GameInstance.h"
#include "MapData_Defines.h"
#include "MapLoader_Helper.h"

#include "Material.h"
#include "EffectContainer.h"

#include "ObjectContainer.h"
#include "XWall.h"

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

	if(auto* pChild = Get_Component<CObjectContainer>())
		pChild->UpdateChild(dt);
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
	ImGui::PushID(this);

	__super::Render_GUI();

	ImGui::PopID();
}

void CMapInvisibleWall::CreateXWall(const _vector2& vCount, const _vector2& vOffset)
{
	Add_Component<CObjectContainer>();

	CXWall::XWALL_DESC* XWallDesc = new CXWall::XWALL_DESC;

	if (vOffset.x != 0.f || vOffset.y != 0.f)
		XWallDesc->vOffset = vOffset;

	XWallDesc->vCount = vCount;
	_vector3 vScale = Get_Component<CCollider>()->Get_Size();

	if (0 == vCount.x)
		XWallDesc->vCount.x = static_cast<_int>(vScale.x / XWallDesc->vOffset.x);

	if (0 == vCount.y)
		XWallDesc->vCount.y = 3;

	auto XWall = Builder::Create_Object({ G_GlobalLevelKey, "Proto_GameObject_XWall" })
		.Add_ObjDesc(XWallDesc)
		.Rotate(Get_Component<CCollider>()->Get_Rotation())
		.Build("XWall");

	if(XWall)
		Get_Component<CObjectContainer>()->Add_Child(XWall, true);
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
