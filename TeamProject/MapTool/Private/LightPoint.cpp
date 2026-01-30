#include "pch.h"
#include "LightPoint.h"
#include "GameInstance.h"

#include "Collider.h"
#include "StaticModel.h"
#include "Material.h"
#include "Light.h"

CLightPoint::CLightPoint()
	: CMapToolObject()
{
}

CLightPoint::CLightPoint(const CLightPoint& rhs)
	: CMapToolObject(rhs)
{
}

HRESULT CLightPoint::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CCollider>();
	Add_Component<CStaticModel>();
	Add_Component<CMaterial>();
	Add_Component<CLight>();

	auto pModel = Get_Component<CStaticModel>();
	pModel->Link_Model(G_GlobalLevelKey, "Default.model");
	auto pMaterial = Get_Component<CMaterial>();
	pMaterial->Link_Material(G_GlobalLevelKey, "Default.mat");

	m_LightDesc.eType = LIGHT_TYPE::POINT;

	return S_OK;
}

HRESULT CLightPoint::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	Get_Component<CCollider>()->Set_MapToolMode(true);
	Get_Component<CTransform>()->Scale({ 0.3f, 0.3f, 0.3f });
	Get_Component<CCollider>()->Set_Size({ 0.3f, 0.3f, 0.3f });
	Get_Component<CCollider>()->Set_ColliderColor({ 1.f, 1.f, 1.f, 1.f });

	if (LIGHT_INIT_DESC* pDesc = static_cast<LIGHT_INIT_DESC*>(pArg)) {
		m_LightDesc.vOffsetPosition = pDesc->DescJson.vOffsetPosition;
		m_LightDesc.vLightDiffuse = pDesc->DescJson.vLightDiffuse;
		m_LightDesc.vLightAmbient = pDesc->DescJson.vLightAmbient;
		m_LightDesc.vLightSpecular = pDesc->DescJson.vLightSpecular;
		m_LightDesc.fLightRange = pDesc->DescJson.fLightRange;
		m_LightDesc.fLightIntensity = pDesc->DescJson.fLightIntensity;
	}


	Get_Component<CLight>()->Set_Desc(m_LightDesc, m_LightDesc.eType);

	return S_OK;
}

void CLightPoint::Awake()
{
	__super::Awake();
}

void CLightPoint::Priority_Update(_float dt)
{
}

void CLightPoint::Update(_float dt)
{
	Get_Component<CCollider>()->Update(dt);
}

void CLightPoint::Late_Update(_float dt)
{
}

void CLightPoint::Export_ObjectData(void* pDesc)
{
	MAP_LIGHT* pMapLightDesc = static_cast<MAP_LIGHT*>(pDesc);

	_vector3 Pos = Get_WorldPos();
	pMapLightDesc->vTranslation = { Pos.x, Pos.y ,Pos.z };
	pMapLightDesc->LightDesc.vOffsetPosition = m_LightDesc.vOffsetPosition;
	pMapLightDesc->LightDesc.vLightDiffuse = m_LightDesc.vLightDiffuse;
	pMapLightDesc->LightDesc.vLightAmbient = m_LightDesc.vLightAmbient;
	pMapLightDesc->LightDesc.vLightSpecular = m_LightDesc.vLightSpecular;
	pMapLightDesc->LightDesc.fLightRange = m_LightDesc.fLightRange;
	pMapLightDesc->LightDesc.fLightIntensity = m_LightDesc.fLightIntensity;
}

void CLightPoint::Render_GUI()
{
	ImGui::PushID(this);

	__super::Render_GUI();

	_vector3 Pos = Get_Component<CTransform>()->Get_Pos();
	_float fPos[3] = { Pos.x,Pos.y,Pos.z };
	ImGui::DragFloat3("Pos", fPos, 0.01f, -100.f, 100.f, "%.2f");
	Get_Component<CTransform>()->Set_Pos({ fPos[0], fPos[1] ,fPos[2] });

	LIGHT_TYPE eType = m_LightDesc.eType;

	const char* typeNames[] = { "Directional", "Point", "Spot" };
	ImGui::Text("Type : %s", typeNames[static_cast<_int>(eType)]);

	ImGui::Text("Range");
	ImGui::DragFloat("##Range", &m_LightDesc.fLightRange, 1.0f, 0.0f, 1000.0f, "%.1f");
	Get_Component<CCollider>()->Set_Size({ m_LightDesc.fLightRange, m_LightDesc.fLightRange, m_LightDesc.fLightRange });

	ImGui::Text("Intensity");
	ImGui::DragFloat("##Intensity", &m_LightDesc.fLightIntensity, 0.5f, 0.0f, 10.0f, "%.1f");

	ImGui::Text("Diffuse");
	ImGui::ColorEdit3("Diffuse", &m_LightDesc.vLightDiffuse.x);
	ImGui::Text("Ambient");
	ImGui::ColorEdit3("Ambient", &m_LightDesc.vLightAmbient.x);
	ImGui::Text("Specular");
	ImGui::ColorEdit3("Specular", &m_LightDesc.vLightSpecular.x);

	ImGui::DragFloat3("Offset", &m_LightDesc.vOffsetPosition.x, 0.1f);
	Get_Component<CCollider>()->Set_Center({ m_LightDesc.vOffsetPosition.x, m_LightDesc.vOffsetPosition.y, m_LightDesc.vOffsetPosition.z });

	Get_Component<CLight>()->Set_Desc(m_LightDesc, m_LightDesc.eType);
	
	ImGui::PopID();
}

CLightPoint* CLightPoint::Create()
{
	CLightPoint* instance = new CLightPoint();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CLightPoint");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CLightPoint::Clone(INIT_DESC* pArg)
{
	CLightPoint* instance = new CLightPoint(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CLightPoint");
		Safe_Release(instance);
	}

	return instance;
}

void CLightPoint::Free()
{
	__super::Free();
}