#include "pch.h"
#include "TestCloud.h"

#include "StaticModel.h"
#include "Material.h"
#include "Shader.h"
#include "Texture.h"

#include "GameInstance.h"
#include "MaterialInstance.h"

CTestCloud::CTestCloud()
{
}

CTestCloud::CTestCloud(const CTestCloud& rhs)
	:CGameObject(rhs)
{
}

void CTestCloud::Set_CloudColor(_float3 newCloudColor)
{
	m_vCloudColor = newCloudColor;

	auto instance = Get_Component<CMaterial>()->Get_MaterialInstance(0);
	instance->Set_Param("g_CloudColor", { &m_vCloudColor, "float3", sizeof(_float3) });
}

void CTestCloud::Set_SkyColor(_float3 newSkyColor)
{
	m_vSkyColor = newSkyColor;

	auto instance = Get_Component<CMaterial>()->Get_MaterialInstance(0);
	instance->Set_Param("g_SkyColor", { &m_vSkyColor, "float3", sizeof(_float3) });
}

void CTestCloud::Set_CloudInfo(_float3 newSkyColor, _float3 newCloudColor)
{
	m_vCloudColor = newCloudColor;
	m_vSkyColor = newSkyColor;

	auto instance = Get_Component<CMaterial>()->Get_MaterialInstance(0);
	instance->Set_Param("g_CloudColor", { &m_vCloudColor, "float3", sizeof(_float3) });
	instance->Set_Param("g_SkyColor", { &m_vSkyColor, "float3", sizeof(_float3) });
}

HRESULT CTestCloud::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CMaterial>();
	Add_Component<CStaticModel>();

	auto pRcsMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
	pRcsMgr->Add_ResourcePath("CloudTest.model",
		"../Bin/Resources/Cloud/CloudTest.model");
	pRcsMgr->Add_ResourcePath("CloudTest.mat",
		"../Bin/Resources/Cloud/CloudTest.mat");

	Get_Component<CStaticModel>()->Link_Model("Test_Level", "CloudTest.model");
	Get_Component<CMaterial>()->Link_Material("Test_Level", "CloudTest.mat");

	return S_OK;
}

HRESULT CTestCloud::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	GAMEOBJECT_DESC* pObjDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

	return S_OK;
}

void CTestCloud::Awake()
{
	auto model = Get_Component<CModel>();
	model->Set_RenderType(RENDER_PASS_TYPE::PRIORITY);
	model->ShadowCast(false);

	auto instance =Get_Component<CMaterial>()->Get_MaterialInstance(0);
	instance->Set_Param("g_CloudColor", { &m_vCloudColor, "float3", sizeof(_float3) });
	instance->Set_Param("g_SkyColor", { &m_vSkyColor, "float3", sizeof(_float3) });

	//=====================Shader Test=========================
	/*auto RenderSys = CGameInstance::GetInstance()->Get_RenderSystem();
	auto pDevice = CGameInstance::GetInstance()->Get_Device();
	auto newTexture = CTexture::Create(pDevice, L"../Bin/Resources/Noise/VX_Noise_XL_07.png","VX_Noise_XL_07.png", true);
	RenderSys->Add_NoiseTexture("Noise1", newTexture);
	RenderSys->Apply_Noise({ "Noise1" }, 5.f);
	NoiseTextures.push_back(newTexture);*/
}

void CTestCloud::Priority_Update(_float dt)
{
	m_fAccTime += dt;

	auto instance = Get_Component<CMaterial>()->Get_MaterialInstance(0);
	instance->Set_Param("g_Time", { &m_fAccTime, "float", sizeof(_float) });
}

void CTestCloud::Update(_float dt)
{
}

void CTestCloud::Late_Update(_float dt)
{
}

void CTestCloud::Render_GUI()
{
	float skyColor[3] = { m_vSkyColor.x, m_vSkyColor.y, m_vSkyColor.z };
	float cloudColor[3] = { m_vSkyColor.x, m_vSkyColor.y, m_vSkyColor.z };
	ImGui::SeparatorText("Sky Color");
	if (ImGui::ColorEdit3("Sky Color", skyColor,
		ImGuiColorEditFlags_Float |
		ImGuiColorEditFlags_DisplayRGB |
		ImGuiColorEditFlags_InputRGB))
	{
		m_vSkyColor.x = skyColor[0];
		m_vSkyColor.y = skyColor[1];
		m_vSkyColor.z = skyColor[2];
	}

	if (ImGui::ColorEdit3("Cloud Color", cloudColor,
		ImGuiColorEditFlags_Float |
		ImGuiColorEditFlags_DisplayRGB |
		ImGuiColorEditFlags_InputRGB))
	{
		m_vCloudColor.x = cloudColor[0];
		m_vCloudColor.y = cloudColor[1];
		m_vCloudColor.z = cloudColor[2];
	}

	// 미리보기(작은 컬러칩)
	ImGui::SameLine();
	ImGui::ColorButton("##SkyPreview",
		ImVec4(m_vSkyColor.x, m_vSkyColor.y, m_vSkyColor.z, 1.0f),
		ImGuiColorEditFlags_NoTooltip, ImVec2(18, 18));

	ImGui::SameLine();
	ImGui::ColorButton("##CloudPreview",
		ImVec4(m_vCloudColor.x, m_vCloudColor.y, m_vCloudColor.z, 1.0f),
		ImGuiColorEditFlags_NoTooltip, ImVec2(18, 18));
}

CTestCloud* CTestCloud::Create()
{
	CTestCloud* Instance = new CTestCloud();
	if (FAILED(Instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CTestCloud");
		Safe_Release(Instance);
	}

	return Instance;
}

CGameObject* CTestCloud::Clone(INIT_DESC* pArg)
{
	CTestCloud* Instance = new CTestCloud(*this);

	if (FAILED(Instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CTestCloud");
		Safe_Release(Instance);
	}

	return Instance;
}

void CTestCloud::Free()
{
	__super::Free();
	//for (auto& Texture : NoiseTextures) Safe_Release(Texture);
	//NoiseTextures.clear();
}
