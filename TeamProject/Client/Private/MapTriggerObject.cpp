#include "pch.h"
#include "MapTriggerObject.h"
#include "GameInstance.h"

#include "Collider.h"
#include "MapLoader_Helper.h"

//포탈을 알고있어야 만드는디
#include "Portal.h"

CMapTriggerObject::CMapTriggerObject()
	: CMapObject()
{
}

CMapTriggerObject::CMapTriggerObject(const CMapTriggerObject& rhs)
	: CMapObject(rhs)
{
}

HRESULT CMapTriggerObject::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CCollider>();

	return S_OK;
}

HRESULT CMapTriggerObject::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	MAPOBJ_DESC* pObjDesc = static_cast<MAPOBJ_DESC*>(pArg);
	
	Ready_PlaneUI(pObjDesc);
	Ready_MeshUI(pObjDesc);
	Ready_Interactable(pObjDesc);

	return S_OK;
}

void CMapTriggerObject::Awake()
{
	__super::Awake();
}

void CMapTriggerObject::Priority_Update(_float dt)
{
}

void CMapTriggerObject::Update(_float dt)
{
	Get_Component<CCollider>()->Update(dt);
}

void CMapTriggerObject::Late_Update(_float dt)
{
}

void CMapTriggerObject::OnTriggerEnter()
{
	//MSG_BOX("부딪힘");
}

void CMapTriggerObject::OnTriggerExit()
{
}

void CMapTriggerObject::OnCollisionEnter()
{
	//MSG_BOX("콜리젼들어옴");
}

void CMapTriggerObject::Export_ObjectData(void* pDesc)
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

void CMapTriggerObject::Render_GUI()
{
	ImGui::PushID(this);

	__super::Render_GUI();

	string TagModelKey = "Trigger Key : " + m_InstanceName;
	ImGui::Text(TagModelKey.c_str());


	ImGui::PopID();
}


void CMapTriggerObject::Ready_PlaneUI(const MAPOBJ_DESC* pObjDesc)
{
	// ---------- Plane UI ----------
	auto iter = pObjDesc->SlotDataValues.find("PlaneUI");
	if (iter != pObjDesc->SlotDataValues.end()) {
		string PrototypeTag = {};
		string AssetKey = {};
		_float3 vOffset = {};
		for (auto& tFieldData : iter->second) 
		{
			if (tFieldData.TagName == "PrototypeTag")
			{
				auto tagValue = GetSlotValue<string>(tFieldData.defaultvalue);
				PrototypeTag = *tagValue;
			}
			else if (tFieldData.TagName == "AssetKey") 
			{
				auto tagValue = GetSlotValue<string>(tFieldData.defaultvalue);
				AssetKey = *tagValue;
			}
			else if (tFieldData.TagName == "Offset") 
			{
				auto tabValue = GetSlotValue<_float3>(tFieldData.defaultvalue);
				vOffset = *tabValue;
			}
		}
		_float3 vPos = {};
		XMStoreFloat3(&vPos, m_pTransform->Get_Pos());

		auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, PrototypeTag })
			.Asset(AssetKey)
			.WorldPos(vPos + vOffset)
			.Build("planeUI");

		CGameInstance::GetInstance()->Get_UIMgr()->Add_UIObject(pObj, pObjDesc->TagLevel);
	}
}

void CMapTriggerObject::Ready_MeshUI(const MAPOBJ_DESC* pObjDesc)
{
	// ---------- Mesh UI ----------
	auto iter = pObjDesc->SlotDataValues.find("MeshUI");
	if (iter != pObjDesc->SlotDataValues.end()) {
		string PrototypeTag = {};
		string AssetKey = {};
		_float3 vOffset = {};
		_float3 vScale = { 1.f, 1.f, 1.f };
		for (auto& tFieldData : iter->second) 
		{
			if (tFieldData.TagName == "PrototypeTag")
			{
				auto tagValue = GetSlotValue<string>(tFieldData.defaultvalue);
				PrototypeTag = *tagValue;
			}
			else if (tFieldData.TagName == "AssetKey")
			{
				auto tagValue = GetSlotValue<string>(tFieldData.defaultvalue);
				AssetKey = *tagValue;
			}
			else if (tFieldData.TagName == "Offset")
			{
				auto tabValue = GetSlotValue<_float3>(tFieldData.defaultvalue);
				vOffset = *tabValue;
			}
			else if (tFieldData.TagName == "Scale")
			{
				auto tabValue = GetSlotValue<_float3>(tFieldData.defaultvalue);
				vScale = *tabValue;
			}
		}
		_float3 vPos = {};
		XMStoreFloat3(&vPos, m_pTransform->Get_Pos());

		MODEL_INIT_DESC modelDesc =  MODEL_INIT_DESC(G_GlobalLevelKey, AssetKey + ".model");
		MATERIAL_INIT_DESC matDesc = MATERIAL_INIT_DESC(G_GlobalLevelKey, AssetKey + ".mat");
		
		auto pObj = Builder::Create_Object({ G_GlobalLevelKey, PrototypeTag })
			.Position(vPos + vOffset)
			.Scale(vScale)
			.Model_Link(modelDesc, MESH_TYPE::NONANIM)
			.Material_Link(matDesc)
			.Build("meshUI");

		CGameInstance::GetInstance()->Get_ObjectMgr()->Add_Object(pObj, { pObjDesc->TagLevel, "Layer_UI" });
	}
}

void CMapTriggerObject::Ready_Interactable(const MAPOBJ_DESC* pObjDesc)
{
	auto iter = pObjDesc->SlotDataValues.find("Portal");

	if (iter != pObjDesc->SlotDataValues.end()) {
		string PrototypeTag = "Proto_GameObject_Portal";
		
		for (auto& tFieldData : iter->second)
		{
			if (tFieldData.TagName == "NextLevel")
			{
				_float3 vPos = {};
				XMStoreFloat3(&vPos, m_pTransform->Get_Pos());
				_vector3 vScale = m_pTransform->Get_Scale();

				string NextLevelTag = *GetSlotValue<string>(tFieldData.defaultvalue);

				CPortal::PORTAL_DESC* pPortalDesc = new CPortal::PORTAL_DESC;
				pPortalDesc->InstanceName = "Portal" + NextLevelTag;
				pPortalDesc->NextNameTag = NextLevelTag;

				COLLIDER_DESC ColDesc = {};
				ColDesc.iCollisionMask = ENUM(COLLISION_GROUP::PLAYER);
				ColDesc.eType = Get_Component<CCollider>()->Get_Type();
				ColDesc.bTrigger = true; // 충돌 박스 생성하는 트리거
				ColDesc.vCenter = Get_Component<CCollider>()->Get_Center();
				ColDesc.vSize = Get_Component<CCollider>()->Get_Size();
				ColDesc.vRotation = Get_Component<CCollider>()->Get_Rotation();

				auto pObj = Builder::Create_Object({ pObjDesc->TagLevel, PrototypeTag })
					.Add_ObjDesc(pPortalDesc)
					.Position(vPos)
					.Collider(ColDesc)
					.Scale(vScale)
					.Build("Portal");

				CGameInstance::GetInstance()->Get_ObjectMgr()->Add_Object(pObj, { pObjDesc->TagLevel, "InteractableObject_Layer" });
			}
		}
	}
}

CMapTriggerObject* CMapTriggerObject::Create()
{
	CMapTriggerObject* instance = new CMapTriggerObject();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CMapTriggerObject");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CMapTriggerObject::Clone(INIT_DESC* pArg)
{
	CMapTriggerObject* instance = new CMapTriggerObject(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CMapTriggerObject");
		Safe_Release(instance);
	}

	return instance;
}

void CMapTriggerObject::Free()
{
	__super::Free();
}