#include "pch.h"
#include "ParticleNode_Edit.h"
#include "GameInstance.h"
#include "ParticleSystem.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "MaterialData.h"
#include "Texture.h"
#include "Helper_Func.h"

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
	pMaterial->Insert_MaterialInstance(customInstance, nullptr);

	auto MaterialDat = customInstance->Get_MaterialData();
	if (MaterialDat)
		MaterialDat->Link_Shader(G_GlobalLevelKey, "VTX_InstancePoint.hlsl");

	m_pTransform->Initialize(nullptr);
	m_InstanceName = "ParticleNode";
	m_IsEffectActive = false;

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
	if(m_IsReady)
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
	CGameObject::Render_GUI();

	ImGui::PopID();
}

void CParticleNode_Edit::Play()
{
	m_isAlive = true;
	if (!m_IsLoop)
		m_IsEffectActive = false;
	else
		m_IsEffectActive = true;

	m_fElpasedTime = 0.f;

	PARTICLE_NODE node{};

	node.vPivot = m_vPivot;
	node.iRGBMaskMode = m_iRGBMaskMode;
	node.iColorMode = ENUM(m_eColorMode);
	node.isWorld = m_IsWorld;
	node.isLoop = m_IsLoop;
	node.iBurstCount = m_iBurstCount;
	node.fSpawnPerSec = m_fSpawnPerSec;
	node.iMaxSpawnParticleCount = m_iMaxSpawnParticleCount;
	node.vStartSpeed = m_vStartSpeed;
	node.vStartLifeTime = m_vStartLifeTime;
	node.vStartSize = m_vStartSize;

	node.SpawnShape = ENUM(m_eSpawnShape);
	node.vCenter = m_vCenter;
	node.vHalfBox = m_vHalfBox;
	node.fRadius = m_fRaidus;
	node.useGravity = m_UseGravity;
	node.fGravityScale = m_fGravityScale;

	node.fDampScale = m_fDampScale;

	node.vStartScale = m_vStartScale;
	node.vEndScale = m_vEndScale;

	node.vStartColor = m_vStartColor;
	node.vEndColor = m_vEndColor;

	node.vAlphaKey = m_vAlphaKey;
	node.vRatio = m_vRatio;

	node.isRandomFrameIndex = m_IsRandomFrameIndex;
	node.isParticleAnimated = m_IsParticleAnimated;
	node.iCol = m_iCol;
	node.iRow = m_iRow;
	node.iMaxFrameIndex = m_iMaxFrameIndex;

	node.vStrength = m_vStrength;
	node.vFrequency = m_vFrequency;
	node.vScrollSpeed = m_vScrollSpeed;

	auto pParticle = Get_Component<CParticleSystem>();
	pParticle->SetParticleParams(node);
	pParticle->Reset();
	
}

void CParticleNode_Edit::Import(nlohmann::ordered_json& json)
{
	m_IsReady = true;

	m_TextureKey = json.value("texture_key", m_TextureKey);
	m_TexturePath = json.value("texture_path", m_TexturePath);

	/* Offset Transform */
	auto vOffsetPosition = json.value("offset_position", json::array({ 0.f,0.f,0.f }));
	auto vOffsetQuaternion = json.value("offset_quaternion", json::array({ 0.f,0.f,0.f,1.f }));

	auto pivot = json.value("pivot", json::array({ 0.5f,0.5f }));
	m_vPivot = _float2(pivot[0], pivot[1]);
	m_iRGBMaskMode = json.value("rgb_mask", m_iRGBMaskMode);
	m_eColorMode = static_cast<CParticleSystem::COLOR_MODE>(json.at("color_mode").get<_uint>());
	m_fDelayTime = json.value("delay_time", m_fDelayTime);
	m_fDuration = json.value("duration", m_fDuration);
	m_IsLoop = json.value("is_loop", m_IsLoop);
	m_IsWorld = json.value("is_world", m_IsWorld);
	m_iBurstCount = json.value("burst_count", m_iBurstCount);
	m_fSpawnPerSec = json.value("spawn_per_sec", m_fSpawnPerSec);
	m_iMaxSpawnParticleCount = json.value("max_spawn_particle_count", m_iMaxSpawnParticleCount);

	m_vStartSpeed.x = json.at("start_speed").at("x").get<_float>();
	m_vStartSpeed.y = json.at("start_speed").at("y").get<_float>();
	m_vStartLifeTime.x = json.at("start_life_time").at("x").get<_float>();
	m_vStartLifeTime.y = json.at("start_life_time").at("y").get<_float>();
	m_vStartSize.x = json.at("start_size").at("x").get<_float>();
	m_vStartSize.y = json.at("start_size").at("y").get<_float>();

	m_vCenter.x = json.at("center").at("x").get<_float>();
	m_vCenter.y = json.at("center").at("y").get<_float>();
	m_vCenter.z = json.at("center").at("z").get<_float>();

	m_eSpawnShape = static_cast<CParticleSystem::SPAWN_SHAPE>(json.at("spawn_shape").get<_uint>());
	m_vHalfBox.x = json.at("half_box").at("x").get<_float>();
	m_vHalfBox.y = json.at("half_box").at("y").get<_float>();
	m_vHalfBox.z = json.at("half_box").at("z").get<_float>();
	m_fRaidus = json.at("radius").get<_float>();

	m_UseGravity = json.value("use_gravity", m_UseGravity);
	m_fGravityScale = json.value("gravity_scale", m_fGravityScale);

	m_fDampScale = json.value("damp_scale", m_fDampScale);

	m_vStartScale.x = json.at("start_scale").at("x").get<_float>();
	m_vStartScale.y = json.at("start_scale").at("y").get<_float>();
	m_vEndScale.x = json.at("end_scale").at("x").get<_float>();
	m_vEndScale.y = json.at("end_scale").at("y").get<_float>();

	m_vStartColor.x = json.at("start_color").at("x").get<_float>();
	m_vStartColor.y = json.at("start_color").at("y").get<_float>();
	m_vStartColor.z = json.at("start_color").at("z").get<_float>();
	m_vStartColor.w = json.at("start_color").at("w").get<_float>();

	m_vAlphaKey.x = json.at("alpha_key").at("x").get<_float>();
	m_vAlphaKey.y = json.at("alpha_key").at("y").get<_float>();
	m_vAlphaKey.z = json.at("alpha_key").at("z").get<_float>();
	m_vAlphaKey.w = json.at("alpha_key").at("w").get<_float>();
	
	m_vRatio.x = json.at("ratio").at("x").get<_float>();
	m_vRatio.y = json.at("ratio").at("y").get<_float>();

	m_vEndColor.x = json.at("end_color").at("x").get<_float>();
	m_vEndColor.y = json.at("end_color").at("y").get<_float>();
	m_vEndColor.z = json.at("end_color").at("z").get<_float>();
	m_vEndColor.w = json.at("end_color").at("w").get<_float>();

	m_IsParticleAnimated = json.value("particle_animated", m_IsParticleAnimated);
	m_IsRandomFrameIndex = json.value("random_frame_index", m_IsRandomFrameIndex);
	m_iCol = json.value("col", m_iCol);
	m_iRow = json.value("row", m_iRow);
	m_iMaxFrameIndex = json.value("max_frame_index", m_iMaxFrameIndex);

	m_vStrength.x = json.at("strength").at("x").get<_float>();
	m_vStrength.y = json.at("strength").at("y").get<_float>();
	m_vStrength.z = json.at("strength").at("z").get<_float>();

	m_vFrequency.x = json.at("frequency").at("x").get<_float>();
	m_vFrequency.y = json.at("frequency").at("y").get<_float>();
	m_vFrequency.z = json.at("frequency").at("z").get<_float>();
	
	m_vScrollSpeed.x = json.at("scroll_speed").at("x").get<_float>();
	m_vScrollSpeed.y = json.at("scroll_speed").at("y").get<_float>();
	m_vScrollSpeed.z = json.at("scroll_speed").at("z").get<_float>();

	/* Set Texture */
	{
		auto pTexture = ResourceManager()->Load_Texture(G_GlobalLevelKey, m_TextureKey, true);

		auto pMaterialInstance = Get_Component<CMaterial>()->Get_MaterialInstance(0);
		pMaterialInstance->Set_Param("DiffuseTexture", { pTexture->Get_SRV(),"Texture2D",0 });

		//auto pMaterialData = Get_Component<CMaterial>()->Get_MaterialInstance(0)->Get_MaterialData();
		//pMaterialData->Link_Texture(G_GlobalLevelKey, m_TextureKey, TEXTURE_TYPE::DIFFUSE);
		//
		//Get_Component<CMaterial>()->Get_MaterialInstance(0)->ChangeTexture(TEXTURE_TYPE::DIFFUSE, 0);
	}

	_vector3 vPosition(vOffsetPosition[0], vOffsetPosition[1], vOffsetPosition[2]);
	_quaternion vQuaternion(vOffsetQuaternion[0], vOffsetQuaternion[1], vOffsetQuaternion[2], vOffsetQuaternion[3]);

	m_pTransform->Set_Pos(vPosition);
	m_pTransform->Set_Quaternion(vQuaternion);
}

void CParticleNode_Edit::Export(nlohmann::ordered_json& json)
{
	_vector3 vOffsetPosition = m_pTransform->Get_Pos();
	_vector4 vOffsetQuaternion = m_pTransform->Get_QuaternionRotate();

	json =
	{
		{"effect_type", ENUM(EFFECT_TYPE::PARTICLE)},
		{"texture_key", m_TextureKey},
		{"texture_path",m_TexturePath},

		/* Offset Transform */
		{"offset_position",json::array({vOffsetPosition.x,vOffsetPosition.y,vOffsetPosition.z})},
		{"offset_quaternion",json::array({vOffsetQuaternion.x,vOffsetQuaternion.y,vOffsetQuaternion.z,vOffsetQuaternion.w})},

		{"pivot",json::array({m_vPivot.x,m_vPivot.y})},
		{"rgb_mask",m_iRGBMaskMode},
		{"color_mode",ENUM(m_eColorMode)},
		{"delay_time",m_fDelayTime},
		{"duration", m_fDuration},
		{"is_loop",m_IsLoop},
		{"is_world", m_IsWorld},
		{"burst_count",m_iBurstCount},
		{"spawn_per_sec",m_fSpawnPerSec},
		{"max_spawn_particle_count",m_iMaxSpawnParticleCount},
		{"start_speed",{{"x",m_vStartSpeed.x},{"y",m_vStartSpeed.y}}},
		{"start_life_time",{{"x",m_vStartLifeTime.x},{"y",m_vStartLifeTime.y}}},
		{"start_size",{{"x",m_vStartSize.x},{"y",m_vStartSize.y}}},

		{"spawn_shape",ENUM(m_eSpawnShape)},
		{"center",{{"x",m_vCenter.x},{"y",m_vCenter.y},{"z",m_vCenter.z}}},
		{"half_box",{{"x",m_vHalfBox.x},{"y",m_vHalfBox.y},{"z",m_vHalfBox.z}}},
		{"radius",m_fRaidus},

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

		/* Life Time Alpha */
		{"alpha_key",{{"x",m_vAlphaKey.x},{"y",m_vAlphaKey.y},{"z",m_vAlphaKey.z},{"w",m_vAlphaKey.w}}},
		{"ratio",{{"x",m_vRatio.x},{"y",m_vRatio.y}}},

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
			m_TextureKey = m_pContext->TextureTags[0];

			auto pTexture = ResourceManager()->Load_Texture(G_GlobalLevelKey, m_pContext->TextureTags[0], true);
			auto pMaterialInstance = Get_Component<CMaterial>()->Get_MaterialInstance(0);
			pMaterialInstance->Set_Param("DiffuseTexture", { pTexture->Get_SRV(),"Texture2D",0 });
		}
	}
}

void CParticleNode_Edit::SetUp_ParticleEffect()
{
	_bool isDirty = false;

	ImGui::SeparatorText("ParticleEffect Setting");

	ImGui::DragFloat("Delay Time", &m_fDelayTime);
	ImGui::DragFloat("Duration", &m_fDuration);

	{
		_float rimLightColor[3] = { m_vRimLightColor.x,m_vRimLightColor.y,m_vRimLightColor.z };

		if (ImGui::ColorEdit3("RimLight Color", rimLightColor))
		{
			m_vRimLightColor = _float3(rimLightColor[0], rimLightColor[1], rimLightColor[2]);
			isDirty = true;
		}
	}
	isDirty |= ImGui::DragFloat2("Pivot", &m_vPivot.x);
	isDirty |= ImGui::DragInt("RGB Mask Mode", reinterpret_cast<_int*>(&m_iRGBMaskMode));
	isDirty |= Helper::DrawEnumCombo("Color Mode", m_eColorMode, 100.f);
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

	isDirty |= Helper::DrawEnumCombo("Spawn Shape", m_eSpawnShape, 100.f);
	isDirty |= ImGui::DragFloat3("Center", &m_vCenter.x);
	isDirty |= ImGui::DragFloat3("Half Box", &m_vHalfBox.x);
	isDirty |= ImGui::DragFloat("Radius", &m_fRaidus);

	if (ImGui::CollapsingHeader("Life Time Velocity"))
	{
		isDirty |= ImGui::DragFloat("Damp Scale", &m_fDampScale);
	}

	if (ImGui::CollapsingHeader("Life Time Size"))
	{
		isDirty |= ImGui::DragFloat2("Start Scale", &m_vStartScale.x);
		isDirty |= ImGui::DragFloat2("End Scale", &m_vEndScale.x);
	}

	if(ImGui::CollapsingHeader("Life Time Color"))
	{
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
	}

	if (ImGui::CollapsingHeader("Life Time Alpha"))
	{
		isDirty |= ImGui::DragFloat("Alpha 0", &m_vAlphaKey.x, 1.f, 0.f, 1.f);
		isDirty |= ImGui::DragFloat("Alpha 1", &m_vAlphaKey.y, 1.f, 0.f, 1.f);
		isDirty |= ImGui::DragFloat("Alpha 2", &m_vAlphaKey.z, 1.f, 0.f, 1.f);
		isDirty |= ImGui::DragFloat("Alpha 3", &m_vAlphaKey.w, 1.f, 0.f, 1.f);

		isDirty |= ImGui::DragFloat("Ratio 0", &m_vRatio.x, 1.f, 0.f, 1.f);
		isDirty |= ImGui::DragFloat("Ratio 1", &m_vRatio.y, 1.f, 0.f, 1.f);
	}

	if (ImGui::CollapsingHeader("Texture Sheet Animation"))
	{
		isDirty |= ImGui::Checkbox("Is Random Frame Index", &m_IsRandomFrameIndex);
		isDirty |= ImGui::Checkbox("Is Particle Animated", &m_IsParticleAnimated);
		isDirty |= ImGui::DragInt("Texture Col", reinterpret_cast<_int*>(&m_iCol));
		isDirty |= ImGui::DragInt("Texture Row", reinterpret_cast<_int*>(&m_iRow));
		isDirty |= ImGui::DragInt("Max FrameIndex", reinterpret_cast<_int*>(&m_iMaxFrameIndex));
	}

	if (ImGui::CollapsingHeader("Noise"))
	{
		isDirty |= ImGui::DragFloat3("Strength", &m_vStrength.x);
		isDirty |= ImGui::DragFloat3("Frequency", &m_vFrequency.x);
		isDirty |= ImGui::DragFloat3("Scroll Speed", &m_vScrollSpeed.x);
	}

	if (isDirty)
	{
		m_IsReady = true;

		PARTICLE_NODE node{};

		node.vRimLightColor = m_vRimLightColor;
		node.vPivot = m_vPivot;
		node.iRGBMaskMode = m_iRGBMaskMode;
		node.SpawnShape = ENUM(m_eSpawnShape);
		node.iColorMode = ENUM(m_eColorMode);
		node.isWorld = m_IsWorld;
		node.isLoop = m_IsLoop;
		node.iBurstCount = m_iBurstCount;
		node.fSpawnPerSec = m_fSpawnPerSec;
		node.iMaxSpawnParticleCount = m_iMaxSpawnParticleCount;

		if (m_vStartSpeed.x <= m_vStartSpeed.y)
			node.vStartSpeed = m_vStartSpeed;

		if(m_vStartLifeTime.x<=m_vStartLifeTime.y)
			node.vStartLifeTime = m_vStartLifeTime;

		node.vStartSize = m_vStartSize;

		node.SpawnShape = ENUM(m_eSpawnShape);
		node.vCenter = m_vCenter;
		node.vHalfBox = m_vHalfBox;
		node.fRadius = m_fRaidus;

		node.useGravity = m_UseGravity;
		node.fGravityScale = m_fGravityScale;

		node.fDampScale = m_fDampScale;

		node.vStartScale = m_vStartScale;
		node.vEndScale = m_vEndScale;
		
		node.vStartColor = m_vStartColor;
		node.vEndColor = m_vEndColor;

		node.vAlphaKey = m_vAlphaKey;

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
