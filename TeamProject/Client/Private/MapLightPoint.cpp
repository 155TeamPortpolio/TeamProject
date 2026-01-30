#include "pch.h"
#include "MapLightPoint.h"
#include "Light.h"

CMapLightPoint::CMapLightPoint()
{
}

CMapLightPoint::CMapLightPoint(const CMapLightPoint& rhs)
{
}

HRESULT CMapLightPoint::Initialize_Prototype()
{
	Add_Component<CCollider>();
	Add_Component<CLight>();
	return S_OK;
}

HRESULT CMapLightPoint::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	Get_Component<CCollider>()->Set_MapToolMode(true);
	Get_Component<CTransform>()->Scale({ 0.3f, 0.3f, 0.3f });
	Get_Component<CCollider>()->Set_Size({ 0.3f, 0.3f, 0.3f });
	Get_Component<CCollider>()->Set_ColliderColor({ 1.f, 1.f, 1.f, 1.f });

	LIGHT_DESC LightDesc{};
	if (MAP_LIGHTPOINT_DESC* pDesc = static_cast<MAP_LIGHTPOINT_DESC*>(pArg)) {
		LightDesc.vOffsetPosition = pDesc->DescJson.vOffsetPosition;
		LightDesc.vLightDiffuse = pDesc->DescJson.vLightDiffuse;
		LightDesc.vLightAmbient = pDesc->DescJson.vLightAmbient;
		LightDesc.vLightSpecular = pDesc->DescJson.vLightSpecular;
		LightDesc.fLightRange = pDesc->DescJson.fLightRange;
		LightDesc.fLightIntensity = pDesc->DescJson.fLightIntensity;
	}

	Get_Component<CLight>()->Set_Desc(LightDesc, LIGHT_TYPE::POINT);

	return S_OK;
}

void CMapLightPoint::Awake()
{
	__super::Awake();
}

void CMapLightPoint::Priority_Update(_float dt)
{
}

void CMapLightPoint::Update(_float dt)
{
}

void CMapLightPoint::Late_Update(_float dt)
{
}

void CMapLightPoint::Export_ObjectData(void* pDesc)
{
}

void CMapLightPoint::Render_GUI()
{
}

CMapLightPoint* CMapLightPoint::Create()

{
	CMapLightPoint* instance = new CMapLightPoint();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CMapLightPoint");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CMapLightPoint::Clone(INIT_DESC* pArg)
{
	CMapLightPoint* instance = new CMapLightPoint(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CMapLightPoint");
		Safe_Release(instance);
	}

	return instance;
}

void CMapLightPoint::Free()
{
	__super::Free();
}