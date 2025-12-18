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
	if (m_SetMaterial && m_SetMesh)
	{
		__super::Update(dt);


		POST_PROCESS_COMMAND Command =
		{
			POSTPROCESS::MRT_Bloom,
			Get_Component<CMaterial>()->Get_Shader(0),
			m_pTransform->Get_WorldMatrix_Ptr(),
			[this](ID3D11DeviceContext* pContext)
			{
				Render_BloomEffect(pContext);
			}
		};

		CGameInstance::GetInstance()->Get_RenderSystem()->Add_PostProcessCommand(Command);
	}
}

void CMeshNode_Edit::Late_Update(_float dt)
{
}

void CMeshNode_Edit::Render_BloomEffect(ID3D11DeviceContext* pContext)
{
	auto RenderSys = CGameInstance::GetInstance()->Get_RenderSystem();
	auto effectModel = Get_Component<CModel>();
	auto effectMaterial = Get_Component<CMaterial>();
	auto effectShader = effectMaterial->Get_Shader(0);
	auto pCamMgr = CGameInstance::GetInstance()->Get_CameraMgr();
	
	ID3D11InputLayout* pLayout;
	RenderSys->Get_InputLayout(
		effectModel,
		effectShader,
		0,
		"Bright",
		&pLayout
	);

	pContext->IASetInputLayout(pLayout);
	effectShader->Apply("Bright", pContext);
	effectModel->Draw(pContext,0);
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

	m_fAlpha = 1.f;
	m_fElpasedTime = 0.f;
	m_vCurrUVOffset = m_vStartUVOffset;
	m_fDissolveThreshold = 0.f;
}

void CMeshNode_Edit::Import(nlohmann::ordered_json& json)
{

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
		{"base_color",{{"x",m_vBaseColor.x},{"y",m_vBaseColor.y},{"z",m_vBaseColor.z},{"w",m_vBaseColor.w}}},

		/* Alpha */
		{"alpha_fade_ease",static_cast<_uint>(m_eAlphaFadeEase)},
		{"alpha_fade",{{"x",m_vAlphaFade.x},{"y",m_vAlphaFade.y}}},

		/* Scale */
		{"scale_ease",static_cast<_uint>(m_eScaleEase)},
		{"start_scale",{{"x",m_vStartScale.x},{"y",m_vStartScale.y},{"z",m_vStartScale.z}}},
		{"end_scale",{{"x",m_vEndScale.x},{"y",m_vEndScale.y},{"z",m_vEndScale.z}}},

		/* UV Animation */
		{"uv_ease",static_cast<_uint>(m_eUVEase)},
		{"start_uv_offset",{{"x",m_vStartUVOffset.x},{"y",m_vStartUVOffset.y}}},
		{"end_uv_offset",{{"x",m_vEndUVOffset.x},{"y",m_vEndUVOffset.y}}},

		/* Sprite Animation */
		{"col", m_iCol},
		{"row", m_iRow},
		{"max_frame_index",m_iMaxFrameIndex},

		/* Dissolve */
		{"dissolve_ease",static_cast<_uint>(m_eDissolveEase)},
		{"dissolve_start_progress",m_fDissolveStartProgress}
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

			Get_Component<CMaterial>()->Get_MaterialInstance(0)->Set_Blended(true);
			Get_Component<CMaterial>()->Get_MaterialInstance(0)->Override_Pass("UVAnimation");

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
			if (ImGui::Selectable(Math::GetEaseLabel(v), selected))
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
	if (ImGui::Button("Set UV Anim Mode"))
	{
		m_eMode = MODE::UV_ANIMATION;

		pMaterialInstance->Override_Pass("UVAnimation");
	}
	if (ImGui::Button("Set Sprite Anim Mode"))
	{
		m_eMode = MODE::SPRITE_ANIAMTION;

		pMaterialInstance->Override_Pass("SpriteAnimation");
	}
	ImGui::DragFloat("Delay Time", &m_fDelayTime);
	ImGui::DragFloat("Duration", &m_fDuration);

	/*Default Params*/
	if (ImGui::BeginCombo("##seg_ease_alpha", Math::GetEaseLabel(m_eAlphaFadeEase)))
	{
		EaseType eType = m_eAlphaFadeEase;
		ChangeEaseType(m_eAlphaFadeEase, eType);
		ImGui::EndCombo();
	}
	ImGui::DragFloat2("Alpha Fade", &m_vAlphaFade.x);

	if (ImGui::BeginCombo("##seg_ease_scale", Math::GetEaseLabel(m_eScaleEase)))
	{
		EaseType eType = m_eScaleEase;
		ChangeEaseType(m_eScaleEase, eType);
		ImGui::EndCombo();
	}
	ImGui::DragFloat3("Start Scale", &m_vStartScale.x);
	ImGui::DragFloat3("End Scale", &m_vEndScale.x);

	_float baseColor[4] = { m_vBaseColor.x,m_vBaseColor.y,m_vBaseColor.z,m_vBaseColor.w };

	if (ImGui::ColorEdit4("Base Color", baseColor))
	{
		m_vBaseColor = _float4(baseColor[0], baseColor[1], baseColor[2], baseColor[3]);
		isDirty = true;
	}

	/*UV Animation*/
	if (ImGui::BeginCombo("##seg_ease_uv", Math::GetEaseLabel(m_eUVEase)))
	{
		EaseType eType = m_eUVEase;
		ChangeEaseType(m_eUVEase, eType);
		ImGui::EndCombo();
	}
	ImGui::DragFloat2("Start UV Offset", &m_vStartUVOffset.x);
	ImGui::DragFloat2("End UV Offset", &m_vEndUVOffset.x);

	/*Sprite Animation*/
	isDirty |= ImGui::DragInt("Col", reinterpret_cast<_int*>(&m_iCol));
	isDirty |= ImGui::DragInt("Row", reinterpret_cast<_int*>(&m_iRow));
	ImGui::DragInt("Max Frame Index", reinterpret_cast<_int*>(&m_iMaxFrameIndex));

	/*Dissolve*/
	if (ImGui::BeginCombo("##seg_ease_dissolve", Math::GetEaseLabel(m_eDissolveEase)))
	{
		EaseType eType = m_eDissolveEase;
		ChangeEaseType(m_eDissolveEase, eType);
		ImGui::EndCombo();
	}
	ImGui::DragFloat("Dissolve Start Progress", &m_fDissolveStartProgress);

	if (isDirty)
	{
		pMaterialInstance->Set_Param("Col", { &m_iCol,"uint",sizeof(_uint) });
		pMaterialInstance->Set_Param("Row", { &m_iRow,"uint",sizeof(_uint) });
		pMaterialInstance->Set_Param("vBaseColor", { &m_vBaseColor,"float4",sizeof(_float4) });
	}
}
