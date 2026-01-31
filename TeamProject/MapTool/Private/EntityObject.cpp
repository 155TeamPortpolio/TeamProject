#include "pch.h"
#include "EntityObject.h"
#include "GameInstance.h"
#include "MapToolCore.h"
#include "MapToolGui.h"

#include "Collider.h"
#include "ModelData.h"
#include "StaticModel.h"
#include "SkeletalModel.h"
#include "Material.h"

CEntityObject::CEntityObject()
	: CMapToolObject()
{
}

CEntityObject::CEntityObject(const CEntityObject& rhs)
	: CMapToolObject(rhs)
	, m_tCurModel{rhs.m_tCurModel}
	, m_iType{ rhs.m_iType }
{
}

HRESULT CEntityObject::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CCollider>();
	Add_Component<CStaticModel>();
	Add_Component<CMaterial>();

	m_iType = -1;
	m_tCurModel = { false, "None", "Default.model", "Default.mat" };

	auto pModel = Get_Component<CStaticModel>();
	pModel->Link_Model(G_GlobalLevelKey, m_tCurModel.ModelKey);
	auto pMaterial = Get_Component<CMaterial>();
	pMaterial->Link_Material(G_GlobalLevelKey, m_tCurModel.MaterialKey);

	return S_OK;
}

HRESULT CEntityObject::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);
	auto pDesc = static_cast<ENTITY_INIT_DESC*>(pArg);

	if (pDesc)
		m_iType = pDesc->iType;
	else
		m_iType = -1;

	Get_Component<CCollider>()->Set_MapToolMode(true);
	Get_Component<CCollider>()->Set_ColliderColor(Get_TypeColor());

	return S_OK;
}

void CEntityObject::Awake()
{
	__super::Awake();
}

void CEntityObject::Priority_Update(_float dt)
{
}

void CEntityObject::Update(_float dt)
{
	Get_Component<CCollider>()->Update(dt);
}

void CEntityObject::Late_Update(_float dt)
{
}

void CEntityObject::Export_ObjectData(void* pDesc)
{
	ENTITY* pEntityDesc = static_cast<ENTITY*>(pDesc);

	pEntityDesc->tagName = m_InstanceName;
	pEntityDesc->iType = m_iType;

	_float3 vScale; XMStoreFloat3(&vScale, Get_Component<CTransform>()->Get_Pos());
	_float4 qRotation; XMStoreFloat4(&qRotation, Get_Component<CTransform>()->Get_QuaternionRotate());
	_vector3 vEulerRotation = _quaternion(qRotation).ToEuler();
	_float3 vPosition; XMStoreFloat3(&vPosition, Get_Component<CTransform>()->Get_Pos());
	_float3 vColSize = Get_Component<CCollider>()->Get_Size();

	pEntityDesc->vScale = { vScale.x, vScale.y, vScale.z };
	pEntityDesc->vRotation = { vEulerRotation.x, vEulerRotation.y, vEulerRotation.z };
	pEntityDesc->vTranslation = { vPosition.x, vPosition.y, vPosition.z };
	pEntityDesc->vColSize = { vColSize.x, vColSize.y, vColSize.z };
}

void CEntityObject::Set_EntityModel(const string& ModelTag, const string& ModelKeyTag, const string& MaterialKeyTag)
{
	CModelData* pData = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_ModelData(g_TagMapToolLevel, ModelKeyTag);
	if (nullptr == pData)
		return;

	_bool isSkinned = pData->isSkinned();
	
	if (true == isSkinned) {
		if (false == m_tCurModel.IsSkinned) {
			Remove_Component<CStaticModel>();
			Add_Component<CSkeletalModel>();
		}
		Get_Component<CSkeletalModel>()->Link_Model(g_TagMapToolLevel, ModelKeyTag);
	}
	else {
		if (true == m_tCurModel.IsSkinned) {
			Remove_Component<CSkeletalModel>();
			Add_Component<CStaticModel>();
		}

		Get_Component<CStaticModel>()->Link_Model(g_TagMapToolLevel, ModelKeyTag);
	}

	Get_Component<CMaterial>()->Link_Material(g_TagMapToolLevel, MaterialKeyTag);

	m_tCurModel = { isSkinned, ModelTag, ModelKeyTag, MaterialKeyTag };
}

string CEntityObject::Get_TypeName()
{
	string Typename;

	switch (m_iType)
	{
	case 0: Typename = "NPC";		break;
	case 1: Typename = "Interact";	break;
	case 2: Typename = "ETC";		break;
	case 3: Typename = "Invwall";	break;
	default:
		break;
	}

	return Typename;
}

_vector4 CEntityObject::Get_TypeColor()
{
	_vector4 TypeColor;
	
	switch (m_iType)
	{
	case 0: TypeColor = _vector4{ 1.f, 1.f, 0.f, 1.f };		break; // NPC : Yellow
	case 1: TypeColor = _vector4{ 0.f, 1.f, 1.f, 1.f };		break; // INTERACT  : Cyan
	case 2: TypeColor = _vector4{ 0.5f, 0.f, 0.5f, 1.f };	break; // ETC : Purple
	case 3: TypeColor = _vector4{ 0.8f, 0.8f, 0.8f, 1.f };	break; // INVWALL : LightGray
	default:TypeColor = _vector4{ 0.f, 1.f, 0.f, 1.f };		break; // DEF : Green 
	}

	return TypeColor;
}


void CEntityObject::Render_GUI()
{
	ImGui::PushID(this);

	__super::Render_GUI();
	
	ImGui::InputText("##TriggerName", &m_InstanceName);
	ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 1.f), "Client Type");

	if (ImGui::InputInt("##Version", &m_iType)) {
		Get_Component<CCollider>()->Set_ColliderColor(Get_TypeColor());
	};

	const auto EntityList = CMapToolCore::GetInstance()->Get_MapToolGui()->Get_EntityModelNames();
	const auto ModelPaths = CMapToolCore::GetInstance()->Get_MapToolGui()->Get_ModelPathPack();

	if (!EntityList.empty()) {
		if (ImGui::BeginCombo("Model", m_tCurModel.ModelKey.c_str()))
		{
			for (int i = 0; i < EntityList.size(); ++i)
			{
				if (ImGui::Selectable(EntityList[i].c_str()))
				{
					for (auto Model : EntityList)
					{
						for (auto Path : ModelPaths)
						{
							if (Path.TagName == EntityList[i]) {
								Set_EntityModel(Path.TagName, Path.TagModelKey, Path.TagMaterialKey);
							}
						}
					}
				}
			}
			ImGui::EndCombo();
		}
	}

	_vector3 Scale = Get_Component<CTransform>()->Get_Scale();
	_float fScale[3] = { Scale.x,Scale.y,Scale.z };
	ImGui::DragFloat3("Scale", fScale, 0.01f, -100.f, 100.f, "%.2f");
	Get_Component<CTransform>()->Scale({ fScale[0], fScale[1] ,fScale[2] });

	_vector3 ColSize = Get_Component<CCollider>()->Get_Size();
	_float fColSize[3] = { ColSize.x,ColSize.y,ColSize.z };
	ImGui::DragFloat3("ColSize", fColSize, 0.01f, -100.f, 100.f, "%.2f");
	Get_Component<CCollider>()->Set_Size({ fColSize[0], fColSize[1] ,fColSize[2] });

	string ModelType = " ModelType : " + Get_TypeName();
	ImGui::Text(ModelType.c_str());

	ImGui::PopID();
}

CEntityObject* CEntityObject::Create()
{
	CEntityObject* instance = new CEntityObject();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CEntityObject");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CEntityObject::Clone(INIT_DESC* pArg)
{
	CEntityObject* instance = new CEntityObject(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CEntityObject");
		Safe_Release(instance);
	}

	return instance;
}

void CEntityObject::Free()
{
	__super::Free();
}

