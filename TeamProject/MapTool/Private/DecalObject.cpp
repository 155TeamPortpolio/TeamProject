#include "pch.h"
#include "DecalObject.h"

CDecalObject::CDecalObject()
	: CMapToolObject()
{
}

CDecalObject::CDecalObject(const CDecalObject& rhs)
	: CMapToolObject(rhs)
{
}

HRESULT CDecalObject::Initialize_Prototype()
{
	if(FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDecalObject::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CDecalObject::Awake()
{
	__super::Awake();
}

void CDecalObject::Priority_Update(_float dt)
{
}

void CDecalObject::Update(_float dt)
{
}

void CDecalObject::Late_Update(_float dt)
{
}

void CDecalObject::Export_ObjectData(void* pDesc)
{
}

void CDecalObject::Render_GUI()
{
	__super::Render_GUI();
}

CDecalObject* CDecalObject::Create()
{
	CDecalObject* instance = new CDecalObject();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CDecalObject");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CDecalObject::Clone(INIT_DESC* pArg)
{
	CDecalObject* instance = new CDecalObject(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CDecalObject");
		Safe_Release(instance);
	}

	return instance;
}

void CDecalObject::Free()
{
	__super::Free();
}