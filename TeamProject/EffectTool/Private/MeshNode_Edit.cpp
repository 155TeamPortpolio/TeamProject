#include "pch.h"
#include "MeshNode_Edit.h"
#include "GameInstance.h"
#include "StaticModel.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "MaterialData.h"

CMeshNode_Edit::CMeshNode_Edit()
	:CMeshNode()
{
}

CMeshNode_Edit::CMeshNode_Edit(const CMeshNode_Edit& rhs)
	:CMeshNode(rhs)
{
}

HRESULT CMeshNode_Edit::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CStaticModel>();
	Add_Component<CMaterial>();
	return S_OK;
}

HRESULT CMeshNode_Edit::Initialize(INIT_DESC* pArg)
{
	auto resource = CGameInstance::GetInstance()->Get_ResourceMgr();
	resource->Add_ResourcePath("Trail.model", "../Bin/Resource/Mesh/Trail.model");

	Get_Component<CStaticModel>()->Link_Model(G_GlobalLevelKey, "Trail.model");

	ID3D11Device* pDevice = CGameInstance::GetInstance()->Get_Device();
	CMaterial* pMaterial = Get_Component<CMaterial>();
	CMaterialInstance* customInstance = CMaterialInstance::Create_Handle("Effect_Mesh_Base", "Default", pDevice);
	customInstance->ChangeTexture(TEXTURE_TYPE::DIFFUSE, 0);
	customInstance->Set_Blended(true);

	pMaterial->Insert_MaterialInstance(customInstance, nullptr);

	auto MaterialDat = customInstance->Get_MaterialData();
	if (MaterialDat)
		MaterialDat->Link_Shader(G_GlobalLevelKey, "VTX_EffectMesh.hlsl");
	m_InstanceName = "MeshNode";

	return S_OK;
}

void CMeshNode_Edit::Awake()
{
}

void CMeshNode_Edit::Priority_Update(_float dt)
{
}

void CMeshNode_Edit::Update(_float dt)
{
}

void CMeshNode_Edit::Late_Update(_float dt)
{
}

void CMeshNode_Edit::Render_GUI()
{
}

void CMeshNode_Edit::Play()
{
}

CMeshNode_Edit* CMeshNode_Edit::Create()
{
	CMeshNode_Edit* instance = new CMeshNode_Edit();

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Clone Failed : CMeshNode_Edit");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CMeshNode_Edit::Clone(INIT_DESC* pArg)
{
	CMeshNode_Edit* instance = new CMeshNode_Edit(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CMeshNode_Edit");
		Safe_Release(instance);
	}

	return instance;
}

void CMeshNode_Edit::Free()
{
	__super::Free();
}

void CMeshNode_Edit::AddTexture()
{
	if (ImGui::Button("Add Textures"))
	{
		if (!m_pContext->Textures.empty())
		{
			auto pMaterialData = Get_Component<CMaterial>()->Get_MaterialInstance(0)->Get_MaterialData();
			pMaterialData->Link_Texture("EffectEdit_Level", m_pContext->TextureTags[0], TEXTURE_TYPE::DIFFUSE);
		}

		Get_Component<CMaterial>()->Get_MaterialInstance(0)->ChangeTexture(TEXTURE_TYPE::DIFFUSE, 0);
	}
}

void CMeshNode_Edit::SetMesh()
{
}

void CMeshNode_Edit::SetUp_MeshEffect()
{
}
