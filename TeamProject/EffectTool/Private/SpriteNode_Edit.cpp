#include "pch.h"
#include "SpriteNode_Edit.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "MaterialData.h"
#include "PointModel.h"
#include "GameInstance.h"

CSpriteNode_Edit::CSpriteNode_Edit()
	:CSpriteNode()
{
}

CSpriteNode_Edit::CSpriteNode_Edit(const CSpriteNode_Edit& rhs)
	:CSpriteNode(rhs)
{
}

HRESULT CSpriteNode_Edit::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CSpriteNode_Edit::Initialize(INIT_DESC* pArg)
{
	//__super::Initialize(pArg);
	SPRITE_NODE_EDIT_DESC* pDesc = static_cast<SPRITE_NODE_EDIT_DESC*>(pArg);
	m_pContext = pDesc->pContext;

	Get_Component<CPointModel>()->Link_Model(G_GlobalLevelKey, "Engine_Default_Point");

	ID3D11Device* pDevice = CGameInstance::GetInstance()->Get_Device();
	CMaterial* pMaterial = Get_Component<CMaterial>();
	CMaterialInstance* customInstance = CMaterialInstance::Create_Handle("Point_Effect_Base", "Opaque", pDevice);
	customInstance->ChangeTexture(TEXTURE_TYPE::DIFFUSE, 0);
	pMaterial->Insert_MaterialInstance(customInstance, nullptr);

	auto MaterialDat = customInstance->Get_MaterialData();
	if (MaterialDat)
		MaterialDat->Link_Shader(G_GlobalLevelKey, "VTX_Point.hlsl");

	m_pTransform->Scale(_float3{ 50.f,50.f,1.f });

	return S_OK;
}

void CSpriteNode_Edit::Awake()
{
}

void CSpriteNode_Edit::Priority_Update(_float dt)
{
}

void CSpriteNode_Edit::Update(_float dt)
{
	__super::Update(dt);
}

void CSpriteNode_Edit::Late_Update(_float dt)
{
}

void CSpriteNode_Edit::Render_GUI()
{
	static _bool isOpen = true;

	ImGui::PushID(this);
	ImGui::Begin("SpriteNode", &isOpen);
	AddTextures();

	ImGui::End();
	ImGui::PopID();
}

CSpriteNode_Edit* CSpriteNode_Edit::Create()
{
	CSpriteNode_Edit* instance = new CSpriteNode_Edit();

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Clone Failed : CSpriteNode_Edit");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CSpriteNode_Edit::Clone(INIT_DESC* pArg)
{
	CSpriteNode_Edit* instance = new CSpriteNode_Edit(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CSpriteNode_Edit");
		Safe_Release(instance);
	}

	return instance;
}

void CSpriteNode_Edit::Free()
{
	__super::Free();
}

void CSpriteNode_Edit::AddTextures()
{
	if (ImGui::Button("Add Textures"))
	{
		if (!m_pContext->Textures.empty())
		{
			auto pMaterialData = Get_Component<CMaterial>()->Get_MaterialInstance(0)->Get_MaterialData();
			for (_uint i = 0; i < m_pContext->Textures.size(); ++i)
			{
				pMaterialData->Link_Texture("EffectEdit_Level", m_pContext->TextureTags[i], TEXTURE_TYPE::DIFFUSE);
				Get_Component<CMaterial>()->Get_MaterialInstance(0)->ChangeTexture(TEXTURE_TYPE::DIFFUSE, 0);
			}
		}
	}
}	
