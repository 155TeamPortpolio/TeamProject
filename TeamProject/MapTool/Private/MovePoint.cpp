#include "pch.h"
#include "MovePoint.h"

#include "ModelData.h"
#include "StaticModel.h"
#include "SkeletalModel.h"
#include "Material.h"
#include "Collider.h"
#include "MaterialInstance.h"

#include "RayReceiver.h"

#include "GameInstance.h"
#include "MapToolGui.h"


CMovePoint::CMovePoint()
	: CMapToolObject()
{
}

CMovePoint::CMovePoint(const CMovePoint& rhs)
	: CMapToolObject(rhs)
{
}

HRESULT CMovePoint::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CStaticModel>();
	Add_Component<CMaterial>();

	auto pModel = Get_Component<CStaticModel>();
	pModel->Link_Model(G_GlobalLevelKey, "Default.model");
	auto pMaterial = Get_Component<CMaterial>();
	pMaterial->Link_Material(G_GlobalLevelKey, "Default.mat");

	return S_OK;
}

HRESULT CMovePoint::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);
	return S_OK;
}

void CMovePoint::Awake()
{
	__super::Awake();
}

void CMovePoint::Priority_Update(_float dt)
{
}

void CMovePoint::Update(_float dt)
{
	m_InstanceName = "MovePoint_" + to_string(m_iPathIndex) + "_" + to_string(m_iOrderIndex);
}

void CMovePoint::Late_Update(_float dt)
{
	m_pGUi->Update_MovePoint(m_iPathIndex, m_iOrderIndex, Get_WorldPos());
}

void CMovePoint::Export_ObjectData(void* pDesc)
{
	MapData_Object* pObjectDesc = static_cast<MapData_Object*>(pDesc);

	pObjectDesc->TagModelResourceKey = m_TagModelKey;
	pObjectDesc->TagMaterialResourceKey = m_TagMaterialKey;

	_float4x4 matWorld = Get_Component<CTransform>()->Get_WorldMatrix();

	pObjectDesc->vRight = { matWorld._11,matWorld._12, matWorld._13, matWorld._14 };
	pObjectDesc->vUp = { matWorld._21,matWorld._22, matWorld._23, matWorld._24 };
	pObjectDesc->vLook = { matWorld._31,matWorld._32, matWorld._33, matWorld._34 };
	pObjectDesc->vPos = { matWorld._41,matWorld._42, matWorld._43, matWorld._44 };
}

void CMovePoint::Render_GUI()
{
	ImGui::PushID(this);

	__super::Render_GUI();

	_vector3 Pos = Get_Component<CTransform>()->Get_Pos();
	_float fPos[3] = { Pos.x,Pos.y,Pos.z };
	ImGui::DragFloat3("Pos", fPos, 0.01f, -100.f, 100.f, "%.2f");
	Get_Component<CTransform>()->Set_Pos({ fPos[0], fPos[1] ,fPos[2] });


	ImGui::PopID();
}

CMovePoint* CMovePoint::Create()
{
	CMovePoint* instance = new CMovePoint();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CMovePoint");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CMovePoint::Clone(INIT_DESC* pArg)
{
	CMovePoint* instance = new CMovePoint(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CMovePoint");
		Safe_Release(instance);
	}

	return instance;
}

void CMovePoint::Free()
{
	__super::Free();
}