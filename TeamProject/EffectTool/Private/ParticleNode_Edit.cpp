#include "pch.h"
#include "ParticleNode_Edit.h"
#include "GameInstance.h"
#include "ParticleSystem.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "MaterialData.h"

CParticleNode_Edit::CParticleNode_Edit()
	:CParticleNode()
{
}

CParticleNode_Edit::CParticleNode_Edit(const CParticleNode_Edit& rhs)
	:CParticleNode(rhs)
{
}

HRESULT CParticleNode_Edit::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CParticleNode_Edit::Initialize(INIT_DESC* pArg)
{
	PARTICLE_NODE_EDIT_DESC* pDesc = static_cast<PARTICLE_NODE_EDIT_DESC*>(pArg);
	m_pContext = pDesc->pContext;

	CParticleSystem* pParticle = Get_Component<CParticleSystem>();
	pParticle->Link_Model(G_GlobalLevelKey, "Engine_Default_InstancePoint");
	pParticle->Initialize(nullptr);

	ID3D11Device* pDevice = CGameInstance::GetInstance()->Get_Device();
	CMaterial* pMaterial = Get_Component<CMaterial>();
	CMaterialInstance* customInstance = CMaterialInstance::Create_Handle("Point_Effect_Base", "Default", pDevice);
	customInstance->ChangeTexture(TEXTURE_TYPE::DIFFUSE, 0);
	customInstance->Set_Blended(true);

	pMaterial->Insert_MaterialInstance(customInstance, nullptr);

	auto MaterialDat = customInstance->Get_MaterialData();
	if (MaterialDat)
		MaterialDat->Link_Shader(G_GlobalLevelKey, "VTX_InstancePoint.hlsl");

	m_InstanceName = "ParticleNode";
	return S_OK;
}

void CParticleNode_Edit::Awake()
{
}

void CParticleNode_Edit::Priority_Update(_float dt)
{
}

void CParticleNode_Edit::Update(_float dt)
{
	__super::Update(dt);
}

void CParticleNode_Edit::Late_Update(_float dt)
{
}

void CParticleNode_Edit::Render_GUI()
{
	static _bool isOpen = true;

	ImGui::PushID(this);
	//ImGui::Begin("SpriteNode", &isOpen);
	AddTextures();
	SetUp_ParticleEffect();

	ImGui::PopID();
}

void CParticleNode_Edit::Play()
{
	PARTICLE_NODE node{};
	Get_Component<CParticleSystem>()->SetParticleParams(node);
}

CParticleNode_Edit* CParticleNode_Edit::Create()
{
	CParticleNode_Edit* instance = new CParticleNode_Edit();

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Clone Failed : CParticleNode_Edit");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CParticleNode_Edit::Clone(INIT_DESC* pArg)
{
	CParticleNode_Edit* instance = new CParticleNode_Edit(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CParticleNode_Edit");
		Safe_Release(instance);
	}

	return instance;
}

void CParticleNode_Edit::Free()
{
	__super::Free();
}

void CParticleNode_Edit::AddTextures()
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

void CParticleNode_Edit::SetUp_ParticleEffect()
{
	_bool isDirty = false;

	ImGui::SeparatorText("ParticleEffect Setting");

	isDirty |= ImGui::Checkbox("Is World", &m_IsWorld);
	isDirty |= ImGui::Checkbox("Is Loop", &m_IsLoop);
	isDirty |= ImGui::Checkbox("Use Gravity", &m_UseGravity);
	isDirty |= ImGui::DragFloat("Gravity Scale", &m_fGravityScale);
	isDirty |= ImGui::DragInt("Burst Count", reinterpret_cast<_int*>(&m_iBurstCount));
	isDirty |= ImGui::DragFloat("Spawn Per Sec", &m_fSpawnPerSec);
	isDirty |= ImGui::DragInt("Max Particle", reinterpret_cast<_int*>(&m_iMaxSpawnParticleCount));

	isDirty |= ImGui::DragFloat2("Start Speed Min,Max", &m_vStartSpeed.x);
	isDirty |= ImGui::DragFloat2("Start Life Time Min, Max", &m_vStartLifeTime.x);
	isDirty |= ImGui::DragFloat2("Start Size", &m_vStartSize.x);

	isDirty |= ImGui::DragFloat3("Spawn Area Min", &m_vSpawnAreaMin.x);
	isDirty |= ImGui::DragFloat3("Spawn Area Max", &m_vSpawnAreaMax.x);

	ImGui::SeparatorText("Life Time Velocity");
	isDirty |= ImGui::DragFloat("Damp Scale", &m_fDampScale);

	ImGui::SeparatorText("Life Time Size");
	isDirty |= ImGui::DragFloat2("Start Scale", &m_vStartScale.x);
	isDirty |= ImGui::DragFloat2("End Scale", &m_vEndScale.x);

	if (isDirty)
	{
		PARTICLE_NODE node{};

		node.isWorld = m_IsWorld;
		node.isLoop = m_IsLoop;
		node.iBurstCount = m_iBurstCount;
		node.fSpawnPerSec = m_fSpawnPerSec;
		node.iMaxSpawnParticleCount = m_iMaxSpawnParticleCount;
		node.vStartSpeed = m_vStartSpeed;
		node.vStartLifeTime = m_vStartLifeTime;
		node.vStartSize = m_vStartSize;
		node.vSpawnAreaMin = m_vSpawnAreaMin;
		node.vSpawnAreaMax = m_vSpawnAreaMax;
		node.useGravity = m_UseGravity;
		node.fGravityScale = m_fGravityScale;

		node.fDampScale = m_fDampScale;

		node.vStartScale = m_vStartScale;
		node.vEndScale = m_vEndScale;

		Get_Component<CParticleSystem>()->SetParticleParams(node);
	}
}
