#include "pch.h"
#include "MeshNode_Edit.h"
#include "GameInstance.h"
#include "StaticModel.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "MaterialData.h"
#include "Helper_Func.h"
#include "Texture.h"

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
	pModel->ShadowCast(false);

	m_pTransform->Initialize(nullptr);
	m_InstanceName = "MeshNode";

	_float2 screenSize = CGameInstance::GetInstance()->Get_ClientSize();
	m_fScreenWidth = screenSize.x;
	m_fScreenHeight = screenSize.y;

	return S_OK;
}

void CMeshNode_Edit::Post_EngineUpdate(_float dt)
{
	if (/*m_SetMesh && m_SetMaterial*/true)
		__super::Post_EngineUpdate(dt);
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

	m_DissolveModule.fProgress = 0.f;
	m_fElpasedTime = 0.f;
}

void CMeshNode_Edit::Import(nlohmann::ordered_json& json)
{

	m_ModelKey = json.value("model_key", m_ModelKey);
	m_MaterialKey = json.value("material_key", m_MaterialKey);

	m_fDelayTime = json.value("delay_time", m_fDelayTime);
	m_fDuration = json.value("duration", m_fDuration);
	m_IsLoop = json.value("is_loop", m_IsLoop);

	/* Offset Transform */
	auto vOffsetPosition = json.value("offset_position", json::array({ 0.f,0.f,0.f }));
	auto vOffsetQuaternion = json.value("offset_quaternion",json::array({0.f,0.f,0.f,1.f}));

	/* Texture Tags */
	m_DiffuseTextureTag = json.value("diffuse_texture_tag", "");
	m_DissolveTextureTag = json.value("dissolve_texture_tag", "");
	m_NoiseTextureTag = json.value("noise_texture_tag", "");
	m_MaskTextureTagA = json.value("mask_texture_tag", "");
	m_MaskTextureTagB = json.value("mask_texture_tagB", "");
	m_DistortionTextureTag = json.value("distortion_texture_tag", "");
	m_DistortionMaskTextureTag = json.value("distortion_mask_texture_tag", "");
	m_GradientTextureTag = json.value("gradient_texture_tag", "");

	/* Texture Slot Module */
	m_TextureSlotModule.eSamplerMode = static_cast<TEXTURE_SLOT_MODULE::SAMPLER_MODE>(json.value("sampler_mode", 0));
	m_TextureSlotModule.eMainUsage = static_cast<TEXTURE_SLOT_MODULE::MAIN_USAGE>(json.value("main_usage", 0));
	m_TextureSlotModule.eRed = static_cast<TEXTURE_SLOT_MODULE::CHANNEL_USAGE>(json.at("channel_usage").at("x").get<_uint>());
	m_TextureSlotModule.eGreen = static_cast<TEXTURE_SLOT_MODULE::CHANNEL_USAGE>(json.at("channel_usage").at("y").get<_uint>());
	m_TextureSlotModule.eBlue = static_cast<TEXTURE_SLOT_MODULE::CHANNEL_USAGE>(json.at("channel_usage").at("z").get<_uint>());
	m_TextureSlotModule.eAlpha = static_cast<TEXTURE_SLOT_MODULE::CHANNEL_USAGE>(json.at("channel_usage").at("w").get<_uint>());
	m_TextureSlotModule.iColorMode = json.value("color_mode", 0);
	m_TextureSlotModule.iRGBMask = json.value("rgb_mask", 0);

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
	m_DissolveModule.fEnableDissolve = json.value("enable_dissolve", 0.f);
	m_DissolveModule.eEaseType = static_cast<EaseType>(json.value("dissolve_ease_type", 0));
	m_DissolveModule.fDissolveSoftness = json.value("dissolve_softness", 0.f);
	m_DissolveModule.fStartProgress = json.value("dissolve_start_progress", 1.f);
	m_DissolveModule.fEndProgress = json.value("dissolve_end_progress", 1.f);

	/* Bloom Module */
	m_BloomModule.fIntensity = json.value("bloom_intensity", 1.f);
	m_BloomModule.fSoftness = json.value("bloom_softness", m_BloomModule.fSoftness);
	m_BloomModule.fThreshold = json.value("bloom_threshold", m_BloomModule.fThreshold);

	/* Noise Module */
	m_NoiseModule.fEnableNoise = json.value("enable_noise", 0.f);
	m_NoiseModule.fNoiseStrength = json.value("noise_strength", 0.f);
	m_NoiseModule.fNoiseTilling = json.value("noise_tilling", 0.f);
	m_NoiseModule.vNoiseUVSpeed.x = json.at("noise_uvspeed").at("x").get<_float>();
	m_NoiseModule.vNoiseUVSpeed.y = json.at("noise_uvspeed").at("y").get<_float>();

	/* Mask Module */
	m_MaskModule.fEnableMaskA = json.value("enable_mask", 0.f);
	m_MaskModule.fEnableMaskB = json.value("enable_maskB", 0.f);
	m_MaskModule.fMaskTilling = json.value("mask_tilling", 0.f);

	/* Distortion Module */
	m_DistortionModule.useDiffuseAlpha = json.value("use_diffuse_alpha", true);
	m_DistortionModule.useDistortionMask = json.value("use_distortion_mask", false);
	m_DistortionModule.fEnableDistortion = json.value("enable_distortion", 0.f);
	m_DistortionModule.fDistortionStrength = json.value("distortion_strength", 0.f);
	m_DistortionModule.fDistortionTilling = json.value("distortion_tilling", 0.f);
	auto distortionUVSpeed = json.value("distortion_uvspeed", json::array({ 0.f,0.f }));
	m_DistortionModule.vDistortionUVSpeed = _float2(distortionUVSpeed[0], distortionUVSpeed[1]);

	/* Gradient Module */
	m_GradientModule.fEnableGradient = json.value("enable_gradient", 0.f);
	m_GradientModule.eGradientMode = static_cast<GRADIENT_MODULE::GRADIENT_MODE>(json.value("gradient_mode", 0));

	{
		m_SetMaterial = true;

		if (FAILED(Get_Component<CMaterial>()->Link_Material(G_GlobalLevelKey, m_MaterialKey)))
			MSG_BOX("Link Failed - Material");

		auto pMaterialInstance = Get_Component<CMaterial>()->Get_MaterialInstance(0);

		if (!m_DiffuseTextureTag.empty())
		{
			auto pDiffuseTexture = ResourceManager()->Load_Texture(G_GlobalLevelKey, m_DiffuseTextureTag);
			pMaterialInstance->Set_Param("DiffuseTexture", { pDiffuseTexture->Get_SRV(),"Texture2D",0 });
		}

		if (!m_NoiseTextureTag.empty())
		{
			auto pNoiseTexture = ResourceManager()->Load_Texture(G_GlobalLevelKey, m_NoiseTextureTag);
			pMaterialInstance->Set_Param("NoiseTexture", { pNoiseTexture->Get_SRV(),"Texture2D",0 });
		}
		
		if (!m_DissolveTextureTag.empty())
		{
			auto pDissolveTexture = ResourceManager()->Load_Texture(G_GlobalLevelKey, m_DissolveTextureTag);
			pMaterialInstance->Set_Param("DissolveTexture", { pDissolveTexture->Get_SRV(),"Texture2D",0 });
		}

		if (!m_MaskTextureTagA.empty())
		{
			auto pMaskTexture = ResourceManager()->Load_Texture(G_GlobalLevelKey, m_MaskTextureTagA);
			pMaterialInstance->Set_Param("AlphaMaskTextureA", { pMaskTexture->Get_SRV(),"Texture2D",0 });
		}
		
		if (!m_MaskTextureTagB.empty())
		{
			auto pMaskTexture = ResourceManager()->Load_Texture(G_GlobalLevelKey, m_MaskTextureTagB);
			pMaterialInstance->Set_Param("AlphaMaskTextureB", { pMaskTexture->Get_SRV(),"Texture2D",0 });
		}

		if (!m_DistortionTextureTag.empty())
		{
			auto pDistortionTexture = ResourceManager()->Load_Texture(G_GlobalLevelKey, m_DistortionTextureTag);
			pMaterialInstance->Set_Param("DistortionTexture", { pDistortionTexture->Get_SRV(),"Texture2D",0 });
		}

		if (!m_DistortionMaskTextureTag.empty())
		{
			auto pDistortionMaskTexture = ResourceManager()->Load_Texture(G_GlobalLevelKey, m_DistortionMaskTextureTag);
			pMaterialInstance->Set_Param("DistortionMaskTexture", { pDistortionMaskTexture->Get_SRV(),"Texture2D",0 });
		}

		if (!m_GradientTextureTag.empty())
		{
			auto pGradientTexture = ResourceManager()->Load_Texture(G_GlobalLevelKey, m_GradientTextureTag);
			pMaterialInstance->Set_Param("GradientTexture", { pGradientTexture->Get_SRV(),"Texture2D",0 });
		}

		m_SetMaterial = true;
	}

	{
		if (FAILED(Get_Component<CStaticModel>()->Link_Model(G_GlobalLevelKey, m_ModelKey)))
			MSG_BOX("Link Failed - Mesh");

		m_SetMesh = true;
	}

	_vector3 vPosition(vOffsetPosition[0], vOffsetPosition[1], vOffsetPosition[2]);
	_quaternion vQuaternion(vOffsetQuaternion[0], vOffsetQuaternion[1], vOffsetQuaternion[2], vOffsetQuaternion[3]);

	m_pTransform->Set_Pos(vPosition);
	m_pTransform->Set_Quaternion(vQuaternion);
}

void CMeshNode_Edit::Export(nlohmann::ordered_json& json)
{
	_vector3 vOffsetPosition = m_pTransform->Get_Pos();
	_quaternion vOffsetQuaternion = m_pTransform->Get_QuaternionRotate();

	json =
	{
		{"effect_type", static_cast<_uint>(EFFECT_TYPE::MESH)},
		{"model_key",m_ModelKey},
		{"material_key",m_MaterialKey},

		{"delay_time", m_fDelayTime},
		{"duration", m_fDuration},
		{"is_loop",m_IsLoop},

		/* Texture Tag */
		{"diffuse_texture_tag",m_DiffuseTextureTag},
		{"dissolve_texture_tag",m_DissolveTextureTag},
		{"noise_texture_tag",m_NoiseTextureTag},
		{"mask_texture_tag",m_MaskTextureTagA},
		{"mask_texture_tagB",m_MaskTextureTagB},
		{"distortion_texture_tag",m_DistortionTextureTag},
		{"distortion_mask_texture_tag",m_DistortionMaskTextureTag},
		{"gradient_texture_tag",m_GradientTextureTag},

		/* Offset Transform */
		{"offset_position",json::array({vOffsetPosition.x,vOffsetPosition.y,vOffsetPosition.z})},
		{"offset_quaternion",json::array({vOffsetQuaternion.x,vOffsetQuaternion.y,vOffsetQuaternion.z,vOffsetQuaternion.w})},

		/* Texture Module */
		{"sampler_mode",m_TextureSlotModule.iSamplerModeParam},
		{"main_usage",m_TextureSlotModule.iMainUsageParam},
		{"channel_usage",
		{{"x",m_TextureSlotModule.vChannelUsageParam.x},
		{"y",m_TextureSlotModule.vChannelUsageParam.y},
		{"z",m_TextureSlotModule.vChannelUsageParam.z},
		{"w",m_TextureSlotModule.vChannelUsageParam.w}}},
		{"color_mode",m_TextureSlotModule.iColorMode},
		{"rgb_mask",m_TextureSlotModule.iRGBMask},

		/* Color Module */
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
		{"enable_dissolve",m_DissolveModule.fEnableDissolve},
		{"dissolve_ease_type",ENUM(m_DissolveModule.eEaseType)},
		{"dissolve_softness",m_DissolveModule.fDissolveSoftness},
		{"dissolve_start_progress",m_DissolveModule.fStartProgress},
		{"dissolve_end_progress",m_DissolveModule.fEndProgress},

		/* Bloom */
		{"bloom_intensity",m_BloomModule.fIntensity},
		{"bloom_threshold",m_BloomModule.fThreshold},
		{"bloom_softness",m_BloomModule.fSoftness},

		/* Noise */
		{"enable_noise",m_NoiseModule.fEnableNoise},
		{"noise_strength",m_NoiseModule.fNoiseStrength},
		{"noise_tilling",m_NoiseModule.fNoiseTilling},
		{"noise_uvspeed",{{"x",m_NoiseModule.vNoiseUVSpeed.x},{"y",m_NoiseModule.vNoiseUVSpeed.y}}},

		/* Mask */
		{"enable_mask",m_MaskModule.fEnableMaskA},
		{"enable_maskB",m_MaskModule.fEnableMaskB},
		{"mask_tilling",m_MaskModule.fMaskTilling},

		/* Distortion */
		{"use_diffuse_alpha",m_DistortionModule.useDiffuseAlpha},
		{"use_distortion_mask",m_DistortionModule.useDistortionMask},
		{"enable_distortion", m_DistortionModule.fEnableDistortion},
		{"distortion_strength", m_DistortionModule.fDistortionStrength},
		{"distortion_tilling",m_DistortionModule.fDistortionTilling},
		{"distortion_uvspeed",json::array({m_DistortionModule.vDistortionUVSpeed.x,m_DistortionModule.vDistortionUVSpeed.y})},

		/* Gradient */
		{"enable_gradient",m_GradientModule.fEnableGradient},
		{"gradient_mode",ENUM(m_GradientModule.eGradientMode)}
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

void CMeshNode_Edit::SetUp_MeshEffect()
{
	_bool isDirty = false;
	auto pMaterialInstance = Get_Component<CMaterial>()->Get_MaterialInstance(0);

	ImGui::SeparatorText("MeshEffect Setting");

	ImGui::DragFloat("Delay Time", &m_fDelayTime);
	ImGui::DragFloat("Duration", &m_fDuration);

	if (ImGui::Button("Add Diffuse Texture"))
		Add_Texture(EFFECT_TEXTURE_TYPE::DIFFUSE);
	if (ImGui::Button("Add Noise Texture"))
		Add_Texture(EFFECT_TEXTURE_TYPE::NOISE);
	if (ImGui::Button("Add Dissolve Texture"))
		Add_Texture(EFFECT_TEXTURE_TYPE::DISSOLVE);
	if (ImGui::Button("Add MaskA Texture"))
		Add_Texture(EFFECT_TEXTURE_TYPE::MASK_A);
	if (ImGui::Button("Add MaskB Texture"))
		Add_Texture(EFFECT_TEXTURE_TYPE::MASK_B);
	if (ImGui::Button("Add Distortion Texture"))
		Add_Texture(EFFECT_TEXTURE_TYPE::DISTORTION);
	if (ImGui::Button("Add Distortion Mask Texture"))
		Add_Texture(EFFECT_TEXTURE_TYPE::DISTORTION_MASK);
	if (ImGui::Button("Add Gradient Texture"))
		Add_Texture(EFFECT_TEXTURE_TYPE::GRADIENT);

	if (ImGui::CollapsingHeader("Texture Slot Module"))
	{
		if (Helper::DrawEnumCombo("Sampler Mode", m_TextureSlotModule.eSamplerMode, 100.f))
			m_TextureSlotModule.iSamplerModeParam = ENUM(m_TextureSlotModule.eSamplerMode);

		if (Helper::DrawEnumCombo("Main Usage", m_TextureSlotModule.eMainUsage, 100.f))
			m_TextureSlotModule.iMainUsageParam = ENUM(m_TextureSlotModule.iMainUsageParam);

		if (TEXTURE_SLOT_MODULE::MAIN_USAGE::AS_COLOR == m_TextureSlotModule.eMainUsage)
		{
			_bool useRGBMask = m_TextureSlotModule.iRGBMask > 0 ? true : false;

			ImGui::DragInt("Color Mode", reinterpret_cast<_int*>(&m_TextureSlotModule.iColorMode));
			ImGui::Checkbox("RGB Mask", &useRGBMask);
			m_TextureSlotModule.iRGBMask = useRGBMask ? 1 : 0;
		}

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
		_bool enableDissolve = m_DissolveModule.fEnableDissolve > 0.5f ? true : false;
		if (ImGui::Checkbox("Enable Dissolve", &enableDissolve))
			m_DissolveModule.fEnableDissolve = enableDissolve ? 1.f : 0.f;

		Helper::DrawEnumCombo("Dissolve Ease Type", m_DissolveModule.eEaseType, 100.f);
		ImGui::DragFloat("Dissolve Softness", &m_DissolveModule.fDissolveSoftness);
		ImGui::DragFloat("Start Progress", &m_DissolveModule.fStartProgress);
		ImGui::DragFloat("End Progress", &m_DissolveModule.fEndProgress);
	}

	if (ImGui::CollapsingHeader("Bloom Module"))
	{
		ImGui::DragFloat("Bloom Intensity", &m_BloomModule.fIntensity);
		ImGui::DragFloat("Bloom Threshold", &m_BloomModule.fThreshold);
		ImGui::DragFloat("Bloom Softness", &m_BloomModule.fSoftness);
	}

	if (ImGui::CollapsingHeader("Noise Module"))
	{
		_bool enableNoise = m_NoiseModule.fEnableNoise > 0.5f ? true : false;
		if (ImGui::Checkbox("Enable Noise", &enableNoise))
			m_NoiseModule.fEnableNoise = enableNoise ? 1.f : 0.f;

		ImGui::DragFloat("Noise Strength", &m_NoiseModule.fNoiseStrength);
		ImGui::DragFloat("Noise Tilling", &m_NoiseModule.fNoiseTilling);
		ImGui::DragFloat2("Noise UVSpeed", &m_NoiseModule.vNoiseUVSpeed.x);
	}

	if (ImGui::CollapsingHeader("Mask Module"))
	{
		_bool enableMaskA = m_MaskModule.fEnableMaskA > 0.5f ? true : false;
		_bool enableMaskB = m_MaskModule.fEnableMaskB > 0.5f ? true : false;

		if (ImGui::Checkbox("Enable MaskA", &enableMaskA))
			m_MaskModule.fEnableMaskA = enableMaskA ? 1.f : 0.f;
		if(ImGui::Checkbox("Enable MaskB",&enableMaskB))
			m_MaskModule.fEnableMaskB = enableMaskB ? 1.f : 0.f;

		ImGui::DragFloat("Mask Tilling", &m_MaskModule.fMaskTilling);
	}

	if (ImGui::CollapsingHeader("Distortion Module"))
	{
		_bool enableDistortion = m_DistortionModule.fEnableDistortion > 0.5f ? true : false;
		if (ImGui::Checkbox("Enable Distortion", &enableDistortion))
			m_DistortionModule.fEnableDistortion = enableDistortion ? 1.f : 0.f;

		ImGui::Checkbox("Use Diffuse Alpha", &m_DistortionModule.useDiffuseAlpha);
		ImGui::Checkbox("Use Distortion Mask", & m_DistortionModule.useDistortionMask);
		ImGui::DragFloat("Distortion Strength", &m_DistortionModule.fDistortionStrength);
		ImGui::DragFloat("Distortion Tilling", &m_DistortionModule.fDistortionTilling);
		ImGui::DragFloat2("Distortion UVSpeed", &m_DistortionModule.vDistortionUVSpeed.x);
	}

	if (ImGui::CollapsingHeader("Gradient Module"))
	{
		_bool enableGradient = m_GradientModule.fEnableGradient > 0.5f ? true : false;
		if (ImGui::Checkbox("Enable Gradient", &enableGradient))
		{
			m_GradientModule.fEnableGradient = enableGradient ? 1.f : 0.f;
		}
		ImGui::Text("%lf", m_GradientModule.fEnableGradient);

		Helper::DrawEnumCombo("Gradient Mode", m_GradientModule.eGradientMode, 100.f);
	}
}

void CMeshNode_Edit::Add_Texture(EFFECT_TEXTURE_TYPE type)
{

	if (!m_pContext->Textures.empty())
	{
		auto pMaterialInstance = Get_Component<CMaterial>()->Get_MaterialInstance(0);

		switch (type)
		{
		case EFFECT_TEXTURE_TYPE::DIFFUSE:
		{
			m_DiffuseTextureTag = m_pContext->TextureTags[0];

			auto pDiffuseTexture = ResourceManager()->Load_Texture(G_GlobalLevelKey, m_DiffuseTextureTag);
			pMaterialInstance->Set_Param("DiffuseTexture", { pDiffuseTexture->Get_SRV(),"Texture2D",0 });
		}break;
		case EFFECT_TEXTURE_TYPE::NOISE:
		{
			m_NoiseTextureTag = m_pContext->TextureTags[0];

			auto pNoiseTexture = ResourceManager()->Load_Texture(G_GlobalLevelKey, m_NoiseTextureTag);
			pMaterialInstance->Set_Param("NoiseTexture", { pNoiseTexture->Get_SRV(),"Texture2D",0 });
		}break;
		case EFFECT_TEXTURE_TYPE::DISSOLVE:
		{
			m_DissolveTextureTag = m_pContext->TextureTags[0];

			auto pDissolveTexture = ResourceManager()->Load_Texture(G_GlobalLevelKey, m_DissolveTextureTag);
			pMaterialInstance->Set_Param("DissolveTexture", { pDissolveTexture->Get_SRV(),"Texture2D",0 });
		}break;
		case EFFECT_TEXTURE_TYPE::MASK_A:
		{
			m_MaskTextureTagA = m_pContext->TextureTags[0];

			auto pMaskTexture = ResourceManager()->Load_Texture(G_GlobalLevelKey, m_MaskTextureTagA);
			pMaterialInstance->Set_Param("AlphaMaskTextureA", { pMaskTexture->Get_SRV(),"Texture2D",0 });
		}break;
		case EFFECT_TEXTURE_TYPE::MASK_B:
		{
			m_MaskTextureTagB = m_pContext->TextureTags[0];

			auto pMaskTexture = ResourceManager()->Load_Texture(G_GlobalLevelKey, m_MaskTextureTagB);
			pMaterialInstance->Set_Param("AlphaMaskTextureB", { pMaskTexture->Get_SRV(),"Texture2D",0 });
		}break;
		case EFFECT_TEXTURE_TYPE::DISTORTION:
		{
			m_DistortionTextureTag = m_pContext->TextureTags[0];

			auto pDistortionTexture = ResourceManager()->Load_Texture(G_GlobalLevelKey, m_DistortionTextureTag);
			pMaterialInstance->Set_Param("DistortionTexture", { pDistortionTexture->Get_SRV(),"Texture2D",0 });
		}break;
		case EFFECT_TEXTURE_TYPE::DISTORTION_MASK:
		{
			m_DistortionMaskTextureTag = m_pContext->TextureTags[0];

			auto pDistortionMaskTexture = ResourceManager()->Load_Texture(G_GlobalLevelKey, m_DistortionMaskTextureTag);
			pMaterialInstance->Set_Param("DistortionMaskTexture", { pDistortionMaskTexture->Get_SRV(),"Texture2D",0 });
		}break;
		case EFFECT_TEXTURE_TYPE::GRADIENT:
		{
			m_GradientTextureTag = m_pContext->TextureTags[0];

			auto pGradientTexture = ResourceManager()->Load_Texture(G_GlobalLevelKey, m_GradientTextureTag);
			pMaterialInstance->Set_Param("GradientTexture", { pGradientTexture->Get_SRV(),"Texture2D",0 });
		}break;
		default:
			break;
		}
	}
}
