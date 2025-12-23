#include "pch.h"
#include "MeshNode_Edit.h"
#include "GameInstance.h"
#include "StaticModel.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "MaterialData.h"
#include "Helper_Func.h"

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

	auto pModel = Get_Component<CStaticModel>();
	pModel->Set_RenderType(RENDER_PASS_TYPE::RENDER_EFFECT);
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
	if (m_SetMaterial && m_SetMesh)
	{
		__super::Update(dt);
	}
}

void CMeshNode_Edit::Late_Update(_float dt)
{
}

void CMeshNode_Edit::Render_GUI()
{
	static _bool isOpen = true;
	ImGui::PushID(this);
	ImGui::SeparatorText("MeshEffect");
	SetMesh();
	SetMaterial();
	if (m_SetMesh && m_SetMaterial)
	{
		SetUp_MeshEffect();
		CGameObject::Render_GUI();
	}
	ImGui::PopID();
}

void CMeshNode_Edit::Play()
{
	m_isAlive = true;

	if (!m_IsLoop)
		m_IsEffectActive = false;

	m_fElpasedTime = 0.f;
}

void CMeshNode_Edit::Import(nlohmann::ordered_json& json)
{
	m_ModelKey = json.value("model_key", m_ModelKey);
	m_MaterialKey = json.value("material_key", m_MaterialKey);

	m_fDelayTime = json.value("delay_time", m_fDelayTime);
	m_fDuration = json.value("duration", m_fDuration);
	m_IsLoop = json.value("is_loop", m_IsLoop);

	/* Texture Slot Module */
	m_TextureSlotModule.eSamplerMode = static_cast<TEXTURE_SLOT_MODULE::SAMPLER_MODE>(json.value("sampler_mode", 0));
	m_TextureSlotModule.eMainUsage = static_cast<TEXTURE_SLOT_MODULE::MAIN_USAGE>(json.value("main_usage", 0));
	m_TextureSlotModule.eRed = static_cast<TEXTURE_SLOT_MODULE::CHANNEL_USAGE>(json.at("channel_usage").at("x").get<_uint>());
	m_TextureSlotModule.eGreen = static_cast<TEXTURE_SLOT_MODULE::CHANNEL_USAGE>(json.at("channel_usage").at("y").get<_uint>());
	m_TextureSlotModule.eBlue = static_cast<TEXTURE_SLOT_MODULE::CHANNEL_USAGE>(json.at("channel_usage").at("z").get<_uint>());
	m_TextureSlotModule.eAlpha = static_cast<TEXTURE_SLOT_MODULE::CHANNEL_USAGE>(json.at("channel_usage").at("w").get<_uint>());

	/* Color Module */
	m_ColorModule.eEaseType = static_cast<EaseType>(json.value("color_ease_type", 0));
	m_ColorModule.vStartColor.x = json.at("start_color").at("x").get<_float>();
	m_ColorModule.vStartColor.y = json.at("start_color").at("y").get<_float>();
	m_ColorModule.vStartColor.z = json.at("start_color").at("z").get<_float>();
	m_ColorModule.vStartColor.w = json.at("start_color").at("w").get<_float>();

	m_ColorModule.vEndColor.x = json.at("end_color").at("x").get<_float>();
	m_ColorModule.vEndColor.y = json.at("end_color").at("y").get<_float>();
	m_ColorModule.vEndColor.z = json.at("end_color").at("z").get<_float>();
	m_ColorModule.vEndColor.w = json.at("end_color").at("w").get<_float>();

	/* Scale Module */
	m_ScaleModule.eEaseType = static_cast<EaseType>(json.value("scale_ease_type", 0));
	m_ScaleModule.vStartScale.x = json.at("start_scale").at("x").get<_float>();
	m_ScaleModule.vStartScale.y = json.at("start_scale").at("y").get<_float>();
	m_ScaleModule.vStartScale.z = json.at("start_scale").at("z").get<_float>();

	m_ScaleModule.vEndScale.x = json.at("end_scale").at("x").get<_float>();
	m_ScaleModule.vEndScale.y = json.at("end_scale").at("y").get<_float>();
	m_ScaleModule.vEndScale.z = json.at("end_scale").at("z").get<_float>();

	/* UV Animation Module */
	m_UVAnimaitonModule.eEaseType = static_cast<EaseType>(json.value("uv_ease_type", 0));
	m_UVAnimaitonModule.vStartUVOffset.x = json.at("start_uv_offset").at("x").get<_float>();
	m_UVAnimaitonModule.vStartUVOffset.y = json.at("start_uv_offset").at("y").get<_float>();

	m_UVAnimaitonModule.vEndUVOffset.x = json.at("end_uv_offset").at("x").get<_float>();
	m_UVAnimaitonModule.vEndUVOffset.y = json.at("end_uv_offset").at("y").get<_float>();

	/* Sprite Anim Module */
	m_SpriteAnimationModule.iCol = json.value("col", 1);
	m_SpriteAnimationModule.iRow = json.value("row", 1);
	m_SpriteAnimationModule.iMaxFrameIndex = json.value("max_frame_index", 1);

	/* Dissolve Module */
	m_DissolveModule.eEaseType = static_cast<EaseType>(json.value("dissolve_ease_type", 0));
	m_DissolveModule.fStartProgress = json.value("dissolve_start_progress", 1.f);
	m_DissolveModule.fEndProgress = json.value("dissolve_end_progress", 1.f);

	/* Bloom Module */
	m_BloomModule.fIntensity = json.value("bloom_intensity", 1.f);

	{
		m_SetMaterial = true;

		if (FAILED(Get_Component<CMaterial>()->Link_Material(G_GlobalLevelKey, m_MaterialKey)))
			MSG_BOX("Link Failed - Material");

		m_SetMaterial = true;
	}

	{
		if (FAILED(Get_Component<CStaticModel>()->Link_Model(G_GlobalLevelKey, m_ModelKey)))
			MSG_BOX("Link Failed - Mesh");

		m_SetMesh = true;
	}
}

void CMeshNode_Edit::Export(nlohmann::ordered_json& json)
{
	json =
	{
		{"effect_type", static_cast<_uint>(EFFECT_TYPE::MESH)},
		{"model_key",m_ModelKey},
		{"material_key",m_MaterialKey},

		{"delay_time", m_fDelayTime},
		{"duration", m_fDuration},
		{"is_loop",m_IsLoop},

		/* Texture Module */
		{"sampler_mode",m_TextureSlotModule.iSamplerModeParam},
		{"main_usage",m_TextureSlotModule.iMainUsageParam},
		{"channel_usage",
		{{"x",m_TextureSlotModule.vChannelUsageParam.x},
		{"y",m_TextureSlotModule.vChannelUsageParam.y},
		{"z",m_TextureSlotModule.vChannelUsageParam.z},
		{"w",m_TextureSlotModule.vChannelUsageParam.w}}},

		/* Color Module*/
		{"color_ease_type",ENUM(m_ColorModule.eEaseType)},
		{"start_color",
		{{"x",m_ColorModule.vStartColor.x},
		{"y",m_ColorModule.vStartColor.y},
		{"z",m_ColorModule.vStartColor.z},
		{"w",m_ColorModule.vStartColor.w}}},
		{"end_color",
		{{"x",m_ColorModule.vEndColor.x},
		{"y",m_ColorModule.vEndColor.y},
		{"z",m_ColorModule.vEndColor.z},
		{"w",m_ColorModule.vEndColor.w}}},

		/* Scale Module */
		{"scale_ease_type",ENUM(m_ScaleModule.eEaseType)},
		{"start_scale",{{"x",m_ScaleModule.vStartScale.x},{"y",m_ScaleModule.vStartScale.y},{"z",m_ScaleModule.vStartScale.z}}},
		{"end_scale",{{"x",m_ScaleModule.vEndScale.x},{"y",m_ScaleModule.vEndScale.y},{"z",m_ScaleModule.vEndScale.z}}},

		/* UV Anim Module */
		{"uv_ease_type",ENUM(m_UVAnimaitonModule.eEaseType)},
		{"start_uv_offset",{{"x",m_UVAnimaitonModule.vStartUVOffset.x},{"y",m_UVAnimaitonModule.vStartUVOffset.y}}},
		{"end_uv_offset",{{"x",m_UVAnimaitonModule.vEndUVOffset.x},{"y",m_UVAnimaitonModule.vEndUVOffset.y}}},

		/* Sprite Anim Module */
		{"col",m_SpriteAnimationModule.iCol},
		{"row",m_SpriteAnimationModule.iRow},
		{"max_frame_index",m_SpriteAnimationModule.iMaxFrameIndex},

		/* Dissolve */
		{"dissolve_ease_type",{ENUM(m_DissolveModule.eEaseType)}},
		{"dissolve_start_progress",m_DissolveModule.fStartProgress},
		{"dissolve_end_progress",m_DissolveModule.fEndProgress},

		/* Bloom */
		{"bloom_intensity",m_BloomModule.fIntensity}
	};
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

			//Get_Component<CMaterial>()->Get_MaterialInstance(0)->Override_Pass("UVAnimation");

			m_SetMaterial = true;
			m_MaterialKey = MaterialTag;
		}
	}
}

void CMeshNode_Edit::SetMesh()
{
	if (-1 != m_pContext->iSelectModelIndex)
	{
		if (ImGui::Button("Set Select Model#mesh"))
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
			m_ModelKey = ModelTag;
		}
	}
}

_bool CMeshNode_Edit::ChangeEaseType(EaseType& ioValue, EaseType shownValue)
{
	_bool changed = false;

	auto Pick = [&](EaseType v)
		{
			const bool selected = (shownValue == v);
			if (ImGui::Selectable(Helper::EnumLabel<EaseType>(v), selected))
			{
				ioValue = v;
				changed = true;
			}
			if (selected) ImGui::SetItemDefaultFocus();
		};

	Pick(EaseType::None);

	ImGui::SeparatorText("A. Stable");
	Pick(EaseType::InOutSine);
	Pick(EaseType::OutCubic);
	Pick(EaseType::InOutCubic);
	Pick(EaseType::OutSine);
	Pick(EaseType::InOutQuad);

	ImGui::SeparatorText("B. Ease In");
	Pick(EaseType::InSine);
	Pick(EaseType::InCubic);
	Pick(EaseType::InQuad);
	Pick(EaseType::InCirc);

	ImGui::SeparatorText("C. Settle / Stop");
	Pick(EaseType::InOutCirc);
	Pick(EaseType::OutCirc);
	Pick(EaseType::OutQuad);

	ImGui::SeparatorText("D. Strong");
	Pick(EaseType::InQuart);
	Pick(EaseType::InQuint);
	Pick(EaseType::InOutQuart);
	Pick(EaseType::OutQuart);
	Pick(EaseType::InOutQuint);
	Pick(EaseType::OutQuint);

	ImGui::SeparatorText("E. Extreme");
	Pick(EaseType::InOutExpo);
	Pick(EaseType::OutExpo);
	Pick(EaseType::InExpo);

	ImGui::SeparatorText("F. Overshoot");
	Pick(EaseType::OutBack);
	Pick(EaseType::InOutBack);
	Pick(EaseType::InBack);

	ImGui::SeparatorText("G. Special");
	Pick(EaseType::OutElastic);
	Pick(EaseType::InOutElastic);
	Pick(EaseType::InElastic);
	Pick(EaseType::OutBounce);
	Pick(EaseType::InOutBounce);
	Pick(EaseType::InBounce);

	return changed;
}

void CMeshNode_Edit::SetUp_MeshEffect()
{
	_bool isDirty = false;
	auto pMaterialInstance = Get_Component<CMaterial>()->Get_MaterialInstance(0);

	ImGui::SeparatorText("MeshEffect Setting");

	ImGui::DragFloat("Delay Time", &m_fDelayTime);
	ImGui::DragFloat("Duration", &m_fDuration);

	if (ImGui::CollapsingHeader("Texture Slot Module"))
	{
		if (Helper::DrawEnumCombo("Sampler Mode", m_TextureSlotModule.eSamplerMode, 100.f))
			m_TextureSlotModule.iSamplerModeParam = ENUM(m_TextureSlotModule.eSamplerMode);
		if (Helper::DrawEnumCombo("Main Usage", m_TextureSlotModule.eMainUsage, 100.f))
			m_TextureSlotModule.iMainUsageParam = ENUM(m_TextureSlotModule.iMainUsageParam);
		if (Helper::DrawEnumCombo("Red", m_TextureSlotModule.eRed, 100.f))
			m_TextureSlotModule.vChannelUsageParam.x = ENUM(m_TextureSlotModule.eRed);
		if (Helper::DrawEnumCombo("Green", m_TextureSlotModule.eGreen, 100.f))
			m_TextureSlotModule.vChannelUsageParam.y = ENUM(m_TextureSlotModule.eGreen);
		if (Helper::DrawEnumCombo("Blue", m_TextureSlotModule.eBlue, 100.f))
			m_TextureSlotModule.vChannelUsageParam.z = ENUM(m_TextureSlotModule.eBlue);
		if (Helper::DrawEnumCombo("Alpha", m_TextureSlotModule.eAlpha, 100.f))
			m_TextureSlotModule.vChannelUsageParam.w = ENUM(m_TextureSlotModule.eAlpha);
	}

	if (ImGui::CollapsingHeader("Color Module"))
	{
		Helper::DrawEnumCombo("Color Ease Type", m_ColorModule.eEaseType, 100.f);

		_float startColor[4] = { m_ColorModule.vStartColor.x,m_ColorModule.vStartColor.y,m_ColorModule.vStartColor.z,m_ColorModule.vStartColor.w };
		_float endColor[4] = { m_ColorModule.vEndColor.x,m_ColorModule.vEndColor.y,m_ColorModule.vEndColor.z,m_ColorModule.vEndColor.w };

		if (ImGui::ColorEdit4("Start Color", startColor))
		{
			m_ColorModule.vStartColor = _float4(startColor[0], startColor[1], startColor[2], startColor[3]);
		}
		if (ImGui::ColorEdit4("End Color", endColor))
		{
			m_ColorModule.vEndColor = _float4(endColor[0], endColor[1], endColor[2], endColor[3]);
		}
	}

	if (ImGui::CollapsingHeader("Scale Module"))
	{
		Helper::DrawEnumCombo("Scale Ease Type", m_ScaleModule.eEaseType, 100.f);

		ImGui::DragFloat3("Start Scale", &m_ScaleModule.vStartScale.x);
		ImGui::DragFloat3("End Scale", &m_ScaleModule.vEndScale.x);
	}

	if (ImGui::CollapsingHeader("UV Animation Module"))
	{
		Helper::DrawEnumCombo("UV Ease Type", m_UVAnimaitonModule.eEaseType, 100.f);

		ImGui::DragFloat2("Start UV Offset", &m_UVAnimaitonModule.vStartUVOffset.x);
		ImGui::DragFloat2("End UV Offset", &m_UVAnimaitonModule.vEndUVOffset.x);
	}
	
	if (ImGui::CollapsingHeader("Sprite Animation Module"))
	{
		ImGui::DragInt("Col", reinterpret_cast<_int*>(&m_SpriteAnimationModule.iCol));
		ImGui::DragInt("Row", reinterpret_cast<_int*>(&m_SpriteAnimationModule.iRow));
		ImGui::DragInt("Max Frame Index", reinterpret_cast<_int*>(&m_SpriteAnimationModule.iMaxFrameIndex));
	}

	if (ImGui::CollapsingHeader("Dissolve Module"))
	{
		Helper::DrawEnumCombo("Dissolve Ease Type", m_DissolveModule.eEaseType, 100.f);

		ImGui::DragFloat("Start Progress", &m_DissolveModule.fStartProgress);
		ImGui::DragFloat("End Progress", &m_DissolveModule.fEndProgress);
	}

	if (ImGui::CollapsingHeader("Bloom Module"))
	{
		ImGui::DragFloat("Bloom Intensity", &m_BloomModule.fIntensity);
	}
}
