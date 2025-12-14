#include "pch.h"
#include "ToolLight.h"
#include "Light.h"

CToolLight::CToolLight()
	:CGameObject()
{
}

CToolLight::CToolLight(const CToolLight& rhs)
	:CGameObject(rhs)
{
}

HRESULT CToolLight::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CLight>();
	return S_OK;
}

HRESULT CToolLight::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	return S_OK;
}

void CToolLight::Priority_Update(_float dt)
{
}

void CToolLight::Update(_float dt)
{
}

void CToolLight::Late_Update(_float dt)
{
}

CToolLight* CToolLight::Create()
{
	CToolLight* instance = new CToolLight();

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Clone Failed : CToolLight");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CToolLight::Clone(INIT_DESC* pArg)
{
	CToolLight* instance = new CToolLight(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CToolLight");
		Safe_Release(instance);
	}

	return instance;
}

void CToolLight::Free()
{
	__super::Free();
}
