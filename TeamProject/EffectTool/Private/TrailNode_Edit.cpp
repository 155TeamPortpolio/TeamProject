#include "pch.h"
#include "TrailNode.h"
#include "TrailNode_Edit.h"
#include "TrailModel.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "MaterialData.h"
#include "Helper_Func.h"
#include "GameInstance.h"

CTrailNode_Edit::CTrailNode_Edit()
	:CTrailNode()
{
}
CTrailNode_Edit::CTrailNode_Edit(const CTrailNode_Edit& rhs)
	:CTrailNode(rhs)
{
}
HRESULT CTrailNode_Edit::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}
HRESULT CTrailNode_Edit::Initialize(INIT_DESC* pArg)
{
	TRAIL_NODE_EDIT_DESC* pDesc = static_cast<TRAIL_NODE_EDIT_DESC*>(pArg);
	m_pContext = pDesc->pContext;

	CTrailModel* pTrail = Get_Component<CTrailModel>();
	pTrail->Initialize(nullptr);
	pTrail->Set_RenderType(RENDER_PASS_TYPE::RENDER_EFFECT);

	ID3D11Device* pDevice = CGameInstance::GetInstance()->Get_Device();
	CMaterial* pMaterial = Get_Component<CMaterial>();
	CMaterialInstance* pMaterialInstance = CMaterialInstance::Create_Handle("Trail_Effect_Base", "Opaque", pDevice);
	pMaterialInstance->ChangeTexture(TEXTURE_TYPE::DIFFUSE, 0);

	pMaterial->Insert_MaterialInstance(pMaterialInstance, nullptr);

	CMaterialData* pMaterialData = pMaterialInstance->Get_MaterialData();
	if (pMaterialData)
		pMaterialData->Link_Shader(G_GlobalLevelKey, "VTX_Trail.hlsl");

	return S_OK;
}
void CTrailNode_Edit::Awake()
{
}
void CTrailNode_Edit::Priority_Update(_float dt)
{
}
void CTrailNode_Edit::Update(_float dt)
{
	auto pTrail = Get_Component<CTrailModel>();

	if (CTrailModel::POINT_MODE::CENTER == m_eMode)
	{
		_vector3 vPosition = m_pTransform->Get_WorldPos();
		pTrail->Update_CenterPoint(vPosition, dt);
	}
	else
	{
		_vector3 vPosition0 = m_pTransform->Get_WorldPos();
		_vector3 vPosition1 = vPosition0;
		vPosition1.y -= 5.f;

		pTrail->Update_SegmentPoint(vPosition0, vPosition1, dt);
	}
}
void CTrailNode_Edit::Late_Update(_float dt)
{
}
void CTrailNode_Edit::Render_GUI()
{
	ImGui::PushID(this);
	AddTextures();
	SetUp_TrailEffect();
	ImGui::PopID();
}
void CTrailNode_Edit::Play()
{
}
void CTrailNode_Edit::Import(nlohmann::ordered_json& json)
{
	m_TextureKey = json.value("texture_key", m_TextureKey);
	m_TextureKey = json.value("texture_path", m_TextureKey);

	m_fDelayTime = json.value("delay_time", m_fDelayTime);
	m_fDuration = json.value("duration", m_fDuration);
	m_IsLoop = json.value("is_loop", m_IsLoop);

	m_eMode = static_cast<CTrailModel::POINT_MODE>(json.value("point_mode", 0));
	m_eTextureMode = static_cast<CTrailModel::TEXTURE_MODE>(json.value("texture_mode", 0));
	m_eColorMode = static_cast<CTrailModel::COLOR_MODE>(json.value("color_mode", 0));

	m_vUVSpeed.x = json.at("uv_speed").at("x").get<_float>();
	m_vUVSpeed.y = json.at("uv_speed").at("y").get<_float>();

	m_vStartColor.x = json.at("start_color").at("x").get<_float>();
	m_vStartColor.y = json.at("start_color").at("y").get<_float>();
	m_vStartColor.z = json.at("start_color").at("z").get<_float>();
	m_vStartColor.w = json.at("start_color").at("w").get<_float>();

	m_vEndColor.x = json.at("end_color").at("x").get<_float>();
	m_vEndColor.y = json.at("end_color").at("y").get<_float>();
	m_vEndColor.z = json.at("end_color").at("z").get<_float>();
	m_vEndColor.w = json.at("end_color").at("w").get<_float>();
	
	m_fMaxLifeTime = json.value("max_life_time", m_fMaxLifeTime);
	m_fMinDistance = json.value("min_distance", m_fMinDistance);

	m_fStartWidth = json.value("start_width", m_fStartWidth);
	m_fEndWidth = json.value("end_width", m_fEndWidth);

	/* Set Texture */
	{
		auto pMaterialData = Get_Component<CMaterial>()->Get_MaterialInstance(0)->Get_MaterialData();
		pMaterialData->Link_Texture("EffectEdit_Level", m_TextureKey, TEXTURE_TYPE::DIFFUSE);

		Get_Component<CMaterial>()->Get_MaterialInstance(0)->ChangeTexture(TEXTURE_TYPE::DIFFUSE, 0);
	}

	/* Set Param */
	auto pTrail = Get_Component<CTrailModel>();

	TRAIL_NODE trailNode{};
	trailNode.iMode = ENUM(m_eMode);
	trailNode.iTextureMode = ENUM(m_eTextureMode);
	trailNode.iColorMode = ENUM(m_eColorMode);

	trailNode.vUVSpeed = m_vUVSpeed;
	trailNode.fTile = m_fTile;

	trailNode.fMaxLifeTime = m_fMaxLifeTime;
	trailNode.fMinDistance = m_fMinDistance;

	trailNode.vStartColor = m_vStartColor;
	trailNode.vEndColor = m_vEndColor;

	trailNode.fStartWidth = m_fStartWidth;
	trailNode.fEndWidth = m_fEndWidth;

	pTrail->SetTrailParams(trailNode);
}
void CTrailNode_Edit::Export(nlohmann::ordered_json& json)
{
	json =
	{
		{"effect_type",ENUM(EFFECT_TYPE::TRAIL)},
		{"texture_key",m_TextureKey},
		{"texture_path",m_TextureKey},

		{"delay_time",m_fDelayTime},
		{"duration",m_fDuration},
		{"is_loop",m_IsLoop},

		{"point_mode",ENUM(m_eMode)},
		{"texture_mode",ENUM(m_eTextureMode)},
		{"color_mode",ENUM(m_eColorMode)},

		{"uv_speed",{{"x",m_vUVSpeed.x},{"y",m_vUVSpeed.y}}},
		{"tile",m_fTile},

		{"start_color",{{"x",m_vStartColor.x},{"y",m_vStartColor.y},{"z",m_vStartColor.z},{"w",m_vStartColor.w}}},
		{"end_color",{{"x",m_vEndColor.x},{"y",m_vEndColor.y},{"z",m_vEndColor.z},{"w",m_vEndColor.w}}},

		{"max_life_time",m_fMaxLifeTime},
		{"min_distance", m_fMinDistance},

		{"start_width",m_fStartWidth},
		{"end_width",m_fEndWidth}
	};

}
CTrailNode_Edit* CTrailNode_Edit::Create()
{
	CTrailNode_Edit* instance = new CTrailNode_Edit();

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Clone Failed : CTrailNode_Edit");
		Safe_Release(instance);
	}

	return instance;
}
CGameObject* CTrailNode_Edit::Clone(INIT_DESC* pArg)
{
	CTrailNode_Edit* instance = new CTrailNode_Edit(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CTrailNode_Edit");
		Safe_Release(instance);
	}

	return instance;
}
void CTrailNode_Edit::Free()
{
	__super::Free();
}

void CTrailNode_Edit::AddTextures()
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

void CTrailNode_Edit::SetUp_TrailEffect()
{
	_bool isDirty = false;

	ImGui::SeparatorText("TrailEffect Setting");

	isDirty |= Helper::DrawEnumCombo("Point Mode##0", m_eMode, 100.f);
	isDirty |= Helper::DrawEnumCombo("Texture Mode##1", m_eTextureMode, 100.f);
	isDirty |= Helper::DrawEnumCombo("Color Mode##2", m_eColorMode, 100.f);

	if (ImGui::CollapsingHeader("Texture Mode"))
	{
		isDirty |= ImGui::DragFloat2("UV Speed", &m_vUVSpeed.x);
		isDirty |= ImGui::DragFloat("Tile", &m_fTile);
	}

	if (ImGui::CollapsingHeader("Color Mode"))
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

	isDirty |= ImGui::DragFloat("Max Life Time", &m_fMaxLifeTime);
	isDirty |= ImGui::DragFloat("Min Distance", &m_fMinDistance);
	isDirty |= ImGui::DragFloat("Start Width", &m_fStartWidth);
	isDirty |= ImGui::DragFloat("End Width", &m_fEndWidth);

	if (isDirty)
	{
		auto pTrail = Get_Component<CTrailModel>();

		TRAIL_NODE trailNode{};
		trailNode.iMode = ENUM(m_eMode);
		trailNode.iTextureMode = ENUM(m_eTextureMode);
		trailNode.iColorMode = ENUM(m_eColorMode);

		trailNode.vUVSpeed = m_vUVSpeed;
		trailNode.fTile = m_fTile;

		trailNode.fMaxLifeTime = m_fMaxLifeTime;
		trailNode.fMinDistance = m_fMinDistance;
		
		trailNode.vStartColor = m_vStartColor;
		trailNode.vEndColor = m_vEndColor;

		trailNode.fStartWidth = m_fStartWidth;
		trailNode.fEndWidth = m_fEndWidth;

		pTrail->SetTrailParams(trailNode);
	}
}
