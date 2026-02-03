#include "pch.h"
#include "ToolMap.h"
#include "StaticModel.h"
#include "Material.h"
#include "GameInstance.h"

CToolMap::CToolMap()
	:CGameObject()
{
}

CToolMap::CToolMap(const CToolMap& rhg)
	:CGameObject(rhg)
{
}

HRESULT CToolMap::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	auto pModel = Add_Component<CStaticModel>();
	auto pMaterial = Add_Component<CMaterial>();
	
	ResourceManager()->Add_ResourcePath("Meshout.model", "../Bin/Resources/Map/Meshout.model");
	ResourceManager()->Add_ResourcePath("Meshout.mat", "../Bin/Resources/Map/Meshout.mat");

	pModel->Link_Model(G_GlobalLevelKey, "Meshout.model");
	pMaterial->Link_Material(G_GlobalLevelKey, "Meshout.mat");

	return S_OK;
}

HRESULT CToolMap::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	m_pTransform->Set_Pos(_vector3(0.f, -3.f, 0.f));
	return S_OK;
}

void CToolMap::Priority_Update(_float dt)
{
}

void CToolMap::Update(_float dt)
{
}

void CToolMap::Late_Update(_float dt)
{
}

CToolMap* CToolMap::Create()
{
	CToolMap* instance = new CToolMap();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CToolMap");
		Safe_Release(instance);
	}

	return instance;
}

void CToolMap::Free()
{
	__super::Free();
}

CGameObject* CToolMap::Clone(INIT_DESC* pArg)
{
	CToolMap* instance = new CToolMap(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CToolMap");
		Safe_Release(instance);
	}

	return instance;
}
