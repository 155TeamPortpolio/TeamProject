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

	node.vStartColor = m_vStartColor;
	node.vEndColor = m_vEndColor;

	node.isRandomFrameIndex = m_IsRandomFrameIndex;
	node.isParticleAnimated = m_IsParticleAnimated;
	node.iCol = m_iCol;
	node.iRow = m_iRow;
	node.iMaxFrameIndex = m_iMaxFrameIndex;

	node.vStrength = m_vStrength;
	node.vFrequency = m_vFrequency;
	node.vScrollSpeed = m_vScrollSpeed;

	Get_Component<CParticleSystem>()->SetParticleParams(node);
}

void CParticleNode_Edit::Import(nlohmann::ordered_json& json)
{
}

void CParticleNode_Edit::Export(nlohmann::ordered_json& json)
{
	json =
	{
		{"texture_key",m_TextureKey},
		{"texture_path",m_TexturePath},
		{"is_world", m_IsWorld},
		{"is_loop",m_IsLoop},
		{"burst_count",m_iBurstCount},
		{"spawn_per_sec",m_fSpawnPerSec},
		{"max_spawn_particle_count",m_iMaxSpawnParticleCount},
		{"start_speed",{{"x",m_vStartSpeed.x},{"y",m_vStartSpeed.y}}},
		{"start_life_time",{{"x",m_vStartLifeTime.x},{"y",m_vStartLifeTime.y}}},
		{"start_size",{{"x",m_vStartSize.x},{"y",m_vStartSize.y}}},
		{"spawn_area_min",{{"x",m_vSpawnAreaMin.x},{"y",m_vSpawnAreaMin.y},{"z",m_vSpawnAreaMin.z}}},
		{"spawn_area_max",{{"x",m_vSpawnAreaMax.x},{"y",m_vSpawnAreaMax.y},{"z",m_vSpawnAreaMax.z}}},
		{"use_gravity",m_UseGravity},
		{"gravity_scale",m_fGravityScale},

		/* Life Time Speed */
		{"damp_scale",m_fDampScale},

		/* Life Time Scale */
		{"start_scale",{{"x",m_vStartScale.x},{"y",m_vStartScale.y}}},
		{"end_scale",{{"x",m_vEndScale.x},{"y",m_vEndScale.y}}},

		/* Life Time Color */
		{"start_color",{{"x",m_vStartColor.x},{"y",m_vStartColor.y},{"z",m_vStartColor.z},{"w",m_vStartColor.w}}},
		{"end_color",{{"x",m_vEndColor.x},{"y",m_vEndColor.y},{"z",m_vEndColor.z},{"w",m_vEndColor.w}}},

		/* Texture Sheet Animation */
		{"particle_animated",m_IsParticleAnimated},
		{"random_frame_index",m_IsRandomFrameIndex},
		{"col",m_iCol},
		{"row",m_iRow},
		{"max_frame_index",m_iMaxFrameIndex},

		/* Noise */
		{"strength",{{"x",m_vStrength.x},{"y",m_vStrength.y},{"z",m_vStrength.z}}},
		{"frequency",{{"x",m_vFrequency.x},{"y",m_vFrequency.y},{"z",m_vFrequency.z}}},
		{"scroll_speed",{{"x",m_vScrollSpeed.x},{"y",m_vScrollSpeed.y},{"z",m_vScrollSpeed.z}}}
	};
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

			m_TextureKey = m_pContext->TextureTags[0];
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

	ImGui::SeparatorText("Life Time Color");
	_float startColor[4] = { m_vStartColor.x,m_vStartColor.y,m_vStartColor.z,m_vStartColor.w };
	_float endColor[4] = { m_vEndColor.x,m_vEndColor.y,m_vEndColor.z,m_vEndColor.w };

	if (ImGui::ColorEdit4("Start Color", startColor))
	{
		m_vStartColor = _float4(startColor[0], startColor[1], startColor[2], startColor[3]);
		isDirty = true;
	}
	if (ImGui::ColorEdit4("End Color", endColor))
	{
		m_vEndColor = _float4(endColor[0], endColor[1], endColor[2], endColor[3]);
		isDirty = true;
	}

	ImGui::SeparatorText("Texture Sheet Animation");
	isDirty |= ImGui::Checkbox("Is Random Frame Index", &m_IsRandomFrameIndex);
	isDirty |= ImGui::Checkbox("Is Particle Animated", &m_IsParticleAnimated);
	isDirty |= ImGui::DragInt("Texture Col", reinterpret_cast<_int*>(&m_iCol));
	isDirty |= ImGui::DragInt("Texture Row", reinterpret_cast<_int*>(&m_iRow));
	isDirty |= ImGui::DragInt("Max FrameIndex", reinterpret_cast<_int*>(&m_iMaxFrameIndex));

	ImGui::SeparatorText("Noise");
	isDirty |= ImGui::DragFloat3("Strength", &m_vStrength.x);
	isDirty |= ImGui::DragFloat3("Frequency", &m_vFrequency.x);
	isDirty |= ImGui::DragFloat3("Scroll Speed", &m_vScrollSpeed.x);

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
		
		node.vStartColor = m_vStartColor;
		node.vEndColor = m_vEndColor;

		node.isRandomFrameIndex = m_IsRandomFrameIndex;
		node.isParticleAnimated = m_IsParticleAnimated;
		node.iCol = m_iCol;
		node.iRow = m_iRow;
		node.iMaxFrameIndex = m_iMaxFrameIndex;

		node.vStrength = m_vStrength;
		node.vFrequency = m_vFrequency;
		node.vScrollSpeed = m_vScrollSpeed;

		Get_Component<CParticleSystem>()->SetParticleParams(node);
	}
}
