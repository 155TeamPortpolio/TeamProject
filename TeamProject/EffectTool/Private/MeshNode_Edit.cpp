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
	MESH_NODE_EDIT_DESC* pDesc = static_cast<MESH_NODE_EDIT_DESC*>(pArg);
	m_pContext = pDesc->pContext;

	/*ID3D11Device* pDevice = CGameInstance::GetInstance()->Get_Device();
	CMaterial* pMaterial = Get_Component<CMaterial>();
	CMaterialInstance* customInstance = CMaterialInstance::Create_Handle("Effect_Mesh_Base", "Default", pDevice);
	customInstance->ChangeTexture(TEXTURE_TYPE::DIFFUSE, 0);
	customInstance->Set_Blended(true);

	pMaterial->Insert_MaterialInstance(customInstance, nullptr);
	
	auto MaterialDat = customInstance->Get_MaterialData();
	if (MaterialDat)
		MaterialDat->Link_Shader(G_GlobalLevelKey, "VTX_EffectMesh.hlsl");*/
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
	if(m_SetMaterial && m_SetMesh)
		__super::Update(dt);
}

void CMeshNode_Edit::Late_Update(_float dt)
{
}

void CMeshNode_Edit::Render_GUI()
{
	SetMesh();
	SetMaterial();
	SetUp_MeshEffect();
}

void CMeshNode_Edit::Play()
{
	m_IsLoop = false;
	m_fAlpha = 1.f;
	m_fElpasedTime = 0.f;
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

void CMeshNode_Edit::SetMaterial()
{
	if (-1 != m_pContext->iSelectMaterialIndex)
	{
		if (ImGui::Button("Set Select Material"))
		{
			string MaterialTag = m_pContext->MaterialTags[m_pContext->iSelectMaterialIndex];

			if (FAILED(Get_Component<CMaterial>()->Link_Material(G_GlobalLevelKey, MaterialTag)))
				MSG_BOX("Link Failed - Material");

			Get_Component<CMaterial>()->Get_MaterialInstance(0)->Set_Blended(true);
			Get_Component<CMaterial>()->Get_MaterialInstance(0)->Override_Pass("UVAnimation");
			m_SetMaterial = true;
		}
	}
}

void CMeshNode_Edit::SetMesh()
{
	if (-1 != m_pContext->iSelectModelIndex)
	{
		if (ImGui::Button("Set Select Model"))
		{
			if (!m_SetMaterial)
			{
				MSG_BOX("Set up material fist");
				return;
			}

			string ModelTag = m_pContext->ModelTags[m_pContext->iSelectModelIndex];

			if (FAILED(Get_Component<CStaticModel>()->Link_Model(G_GlobalLevelKey, ModelTag)))
				MSG_BOX("Link Failed - Mesh");

			m_SetMesh = true;
		}
	}
}

void CMeshNode_Edit::SetUp_MeshEffect()
{
	_bool isDirty = false;

	ImGui::SeparatorText("MeshEffect Setting");
	ImGui::DragFloat("Duration", &m_fDuration);
	ImGui::DragFloat2("Alpha Fade", &m_vAlphaFade.x);
	ImGui::DragFloat2("UVSpeed", &m_vUVSpeed.x);
}
