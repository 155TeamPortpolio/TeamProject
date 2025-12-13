#include "pch.h"
#include "DummyModel.h"
#include "StaticModel.h"
#include "RectModel.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "MaterialData.h"

#include "GameInstance.h"

CDummyModel::CDummyModel()
{
}

CDummyModel::CDummyModel(const CDummyModel& rhs)
	:CGameObject(rhs)
{
}

HRESULT CDummyModel::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CRectModel>();
	Add_Component<CMaterial>();

	return S_OK;
}

HRESULT CDummyModel::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);



	return S_OK;
}

void CDummyModel::Awake()
{
	//Get_Component<CModel>()->Link_Model("Demo_Level", "");

	ID3D11Device* pDevice = CGameInstance::GetInstance()->Get_Device();
	CMaterial* pMaterial = Get_Component<CMaterial>();
	//쓰고싶은이름
	CMaterialInstance* customInstance = CMaterialInstance::Create_Handle("Ma_Test", "Opaque", pDevice);
	pMaterial->Insert_MaterialInstance(customInstance, nullptr);
	auto MaterialDat = customInstance->Get_MaterialData();
	if (MaterialDat)
		MaterialDat->Link_Shader(G_GlobalLevelKey, "VTX_TexPos.hlsl");
	if (FAILED(customInstance->Get_MaterialData()->Link_Texture(G_GlobalLevelKey, "Test.dds", TEXTURE_TYPE::DIFFUSE)))
		return;
	
}

void CDummyModel::Priority_Update(_float dt)
{
}

void CDummyModel::Update(_float dt)
{
}

void CDummyModel::Late_Update(_float dt)
{
}

void CDummyModel::Render_GUI()
{
	__super::Render_GUI();
}

CDummyModel* CDummyModel::Create()
{
	CDummyModel* instance = new CDummyModel();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CDummyModel");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CDummyModel::Clone(INIT_DESC* pArg)
{
	CDummyModel* instance = new CDummyModel(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CDummyModel");
		Safe_Release(instance);
	}

	return instance;
}

void CDummyModel::Free()
{
	__super::Free();
}