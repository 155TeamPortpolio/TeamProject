#include "pch.h"
#include "MapLightPoint.h"
#include "Collider.h"
#include "Light.h"

CMapLightPoint::CMapLightPoint()
	: CMapObject()
{
}

CMapLightPoint::CMapLightPoint(const CMapLightPoint& rhs)
	: CMapObject(rhs)
{
}

HRESULT CMapLightPoint::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CCollider>();
	Add_Component<CLight>();
	return S_OK;
}

HRESULT CMapLightPoint::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

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
	__super::Render_GUI();
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