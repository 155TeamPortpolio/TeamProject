#include "pch.h"
#include "EditModel.h"
#include "StaticModel.h"
#include "Material.h"

#include "GameInstance.h"

CEditModel::CEditModel()
{
}

CEditModel::CEditModel(const CEditModel& rhs)
	:CGameObject(rhs)
{
}

HRESULT CEditModel::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	//Add_Component<CStaticModel>();
	//Add_Component<CMaterial>();

	return S_OK;
}

HRESULT CEditModel::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	//m_Components.emplace(type_index(typeid()))

	return S_OK;
}

void CEditModel::Awake()
{
	//CGameInstance::GetInstance()->Get_ResourceMgr()->Add_ResourcePath("", "");
	//Get_Component<CModel>()->Link_Model("Demo_Level", "");
}

void CEditModel::Priority_Update(_float dt)
{
}

void CEditModel::Update(_float dt)
{
}

void CEditModel::Late_Update(_float dt)
{
}

void CEditModel::Render_GUI()
{
	__super::Render_GUI();
}

CEditModel* CEditModel::Create()
{
	CEditModel* instance = new CEditModel();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CEditModel");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CEditModel::Clone(INIT_DESC* pArg)
{
	CEditModel* instance = new CEditModel(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CEditModel");
		Safe_Release(instance);
	}

	return instance;
}

void CEditModel::Free()
{
	__super::Free();
}