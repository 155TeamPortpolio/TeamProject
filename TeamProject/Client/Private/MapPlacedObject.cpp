#include "pch.h"
#include "MapPlacedObject.h"
#include "GameInstance.h"
#include "MapData_Defines.h"
#include "MapLoader_Helper.h"

#include "ModelData.h"
#include "StaticModel.h"
#include "SkeletalModel.h"
#include "Material.h"

#include "EffectContainer.h"



CMapPlacedObject::CMapPlacedObject()
	: CMapObject()
{
}

CMapPlacedObject::CMapPlacedObject(const CMapPlacedObject& rhs)
	: CMapObject(rhs)
{
}

HRESULT CMapPlacedObject::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CMaterial>();

	return S_OK;
}

HRESULT CMapPlacedObject::Initialize(INIT_DESC* pArg)
{
#pragma region Model Type Check
	MAPOBJ_DESC* pObjDesc = static_cast<MAPOBJ_DESC*>(pArg);

	m_TagModelKey = pObjDesc->TagModelKey;
	m_TagMaterialKey = pObjDesc->TagMaterialKey;

	if (true == m_TagModelKey.empty() ||
		true == m_TagMaterialKey.empty())
		return E_FAIL;

	if (true == pObjDesc->bCooking)
		Add_Component<CCollider>();

	CModelData* pData = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_ModelData(pObjDesc->TagLevel, pObjDesc->TagModelKey);
	if (nullptr == pData)
		return E_FAIL;

	_bool isSkinned = pData->isSkinned();

	if (true == isSkinned) {
		Add_Component<CSkeletalModel>();
		Get_Component<CSkeletalModel>()->Link_Model(pObjDesc->TagLevel, m_TagModelKey);
	}
	else {
		Add_Component<CStaticModel>();
		Get_Component<CStaticModel>()->Link_Model(pObjDesc->TagLevel, m_TagModelKey);
	}

	Get_Component<CMaterial>()->Link_Material(pObjDesc->TagLevel, m_TagMaterialKey);
#pragma endregion
	__super::Initialize(pArg);

	ColliderGroup_SlotData(pObjDesc);
	Effect_SlotData(pObjDesc);
	Rotate_SlotData(pObjDesc);

	return S_OK;
}

void CMapPlacedObject::Awake()
{
}

void CMapPlacedObject::Priority_Update(_float dt)
{
}

void CMapPlacedObject::Update(_float dt)
{
	RotatePerSec(dt);
}

void CMapPlacedObject::Late_Update(_float dt)
{
 }

void CMapPlacedObject::Export_ObjectData(void* pDesc)
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

#pragma region SlotData
void CMapPlacedObject::ColliderGroup_SlotData(MAPOBJ_DESC* pObjDesc)
{
	if (!pObjDesc) return;

	auto ColGroup_iter = pObjDesc->SlotDataValues.find("ColliderGroup");
	if (ColGroup_iter != pObjDesc->SlotDataValues.end()) {
		for (auto& tFieldData : ColGroup_iter->second) {
			if (tFieldData.TagName == "Ground") {
				_bool isGround = *GetSlotValue<_bool>(tFieldData.defaultvalue);
				if (isGround)
					Get_Component<CCollider>()->Set_CollisionGroup(COLLISION_GROUP::GROUND);
			}
		}
	}
}

void CMapPlacedObject::Effect_SlotData(MAPOBJ_DESC* pObjDesc)
{
	if (!pObjDesc) return;

	auto Eff_iter = pObjDesc->SlotDataValues.find("Effect");
	if (Eff_iter != pObjDesc->SlotDataValues.end()) {
		string TagAsset = {};
		_float3 vPosition = {};
		for (auto& tFieldData : Eff_iter->second) {
			if (tFieldData.TagName == "AssetKey") {
				auto TagValueAssetKey = GetSlotValue<string>(tFieldData.defaultvalue);
				TagAsset = *TagValueAssetKey;
			}
			else if (tFieldData.TagName == "Position") {
				auto vValuePos = GetSlotValue<_float3>(tFieldData.defaultvalue);
				vPosition = *vValuePos;
			}
		}
		auto effect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset(TagAsset)
			.Position(vPosition)
			.Build("Test_Effect");
		CGameInstance::GetInstance()->Get_ObjectMgr()->Add_Object(effect, { "Test_Level","Effect_Layer" });
	}
}

void CMapPlacedObject::Rotate_SlotData(MAPOBJ_DESC* pObjDesc)
{
	if (!pObjDesc) return;

	auto ColGroup_iter = pObjDesc->SlotDataValues.find("Rotation");
	if (ColGroup_iter != pObjDesc->SlotDataValues.end()) {
		for (auto& tFieldData : ColGroup_iter->second) {
			if (tFieldData.TagName == "DegreePerSec") {
				m_vDegreePerSec = *GetSlotValue<_float3>(tFieldData.defaultvalue);
			}
		}
	}
}

#pragma endregion

void CMapPlacedObject::RotatePerSec(_float dt)
{
	if (m_vDegreePerSec == _vector3::Zero)
		return;

	_quaternion curQuat = Get_Component<CTransform>()->Get_QuaternionRotate();

	_vector3 vDeltaDegree = m_vDegreePerSec * dt;
	_vector3 vDeltaRadians = {
		XMConvertToRadians(vDeltaDegree.x),
		XMConvertToRadians(vDeltaDegree.y),
		XMConvertToRadians(vDeltaDegree.z)
	};

	_quaternion deltaQuat = _quaternion::CreateFromYawPitchRoll(
		vDeltaRadians.y, vDeltaRadians.x, vDeltaRadians.z
	);

	_quaternion nextQuat = deltaQuat * curQuat;
	nextQuat.Normalize();

	Get_Component<CTransform>()->Set_Quaternion(nextQuat);
}

void CMapPlacedObject::Render_GUI()
{
	ImGui::PushID(this);

	__super::Render_GUI();

	ImGui::PopID();
}

CMapPlacedObject* CMapPlacedObject::Create()
{
	CMapPlacedObject* instance = new CMapPlacedObject();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CMapPlacedObject");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CMapPlacedObject::Clone(INIT_DESC* pArg)
{
	CMapPlacedObject* instance = new CMapPlacedObject(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CMapPlacedObject");
		Safe_Release(instance);
	}

	return instance;
}

void CMapPlacedObject::Free()
{
	__super::Free();
}