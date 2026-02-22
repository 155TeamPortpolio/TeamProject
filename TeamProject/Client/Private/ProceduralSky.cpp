#include "pch.h"
#include "ProceduralSky.h"

#include "QuadModel.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "MaterialData.h"
#include "GameInstance.h"

#include "Helper_Func.h"

#include "CameraMgr.h"

CProceduralSky::CProceduralSky()
	:CGameObject()
{
}

CProceduralSky::CProceduralSky(const CProceduralSky& rhs)
	:CGameObject(rhs)
{
}

void CProceduralSky::Set_CloudInfo(CLOUD_DESC Desc)
{
	m_vTopColor = Desc.topColor;
	m_vHorizonColor = Desc.horizonColor;
	m_vHazeColor = Desc.hazeColor;
	m_fSkyAtmosphereBlend = Desc.atmosphereBlend;
	m_vCloudBright = Desc.cloudBright;
	m_vCloudDark = Desc.cloudDark;
	m_fCloudCoverage = Desc.coverage;
}

CLOUD_DESC CProceduralSky::Get_CloudInfo()
{
	return CLOUD_DESC{
		m_vTopColor,
		m_vHorizonColor,
		m_vHazeColor,
		m_fSkyAtmosphereBlend,
		m_vCloudBright,
		m_vCloudDark,
		m_fCloudCoverage,
	};
}

HRESULT CProceduralSky::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CQuadModel>();
	Add_Component<CMaterial>();
	return S_OK;
}

HRESULT CProceduralSky::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);
	auto pModel = Get_Component<CQuadModel>();
	pModel->ShadowCast(false);
	pModel->Set_RenderType(RENDER_PASS_TYPE::PRIORITY);
	ID3D11Device* pDevice = CGameInstance::GetInstance()->Get_Device();
	CMaterial* pMaterial = Get_Component<CMaterial>();

	CMaterialInstance* customInstance = CMaterialInstance::Create_Handle("Cloud", "Opaque", pDevice);
	customInstance->ChangeTexture(TEXTURE_TYPE::DIFFUSE, 0);
	customInstance->Set_Param("g_Time", { &m_fAccTime, "float", sizeof(_float) });
	customInstance->Set_Param("SunDir", { &m_vSunDirection, "float3", sizeof(_float3) });
	customInstance->Set_Param("g_SkyTopColor", { &m_vTopColor, "float3", sizeof(_float3) });
	customInstance->Set_Param("g_SkyHorizonColor", { &m_vHorizonColor, "float3", sizeof(_float3) });
	customInstance->Set_Param("g_SkyAtmosphereBlend", { &m_fSkyAtmosphereBlend, "float", sizeof(_float) });
	customInstance->Set_Param("g_CloudBright", { &m_vCloudBright, "float3", sizeof(_float3) });
	customInstance->Set_Param("g_CloudDark", { &m_vCloudDark, "float3", sizeof(_float3) });
	customInstance->Set_Param("g_CloudCoverage_Param", { &m_fCloudCoverage, "float", sizeof(_float) });
	customInstance->Set_Param("g_HazeColor", { &m_vHazeColor, "float3", sizeof(_float3) });
	
	customInstance->Set_Param("matProjInv", { &m_MatProjectionInv, "matrix", sizeof(_float4x4) });
	customInstance->Set_Param("matViewInv", { &m_MatViewInv, "matrix", sizeof(_float4x4) });

	pMaterial->Insert_MaterialInstance(customInstance, nullptr);
	auto MaterialDat = customInstance->Get_MaterialData();
	if (MaterialDat)
	{
		MaterialDat->Link_Shader(G_GlobalLevelKey, "VTX_Cloud.hlsl");
		MaterialDat->Link_Texture(G_GlobalLevelKey, "cloud_noise.png", TEXTURE_TYPE::DIFFUSE);
		MaterialDat->Link_Texture(G_GlobalLevelKey, "Eff_Noise_092.png", TEXTURE_TYPE::NORMALS);
		MaterialDat->Link_Texture(G_GlobalLevelKey, "Eff_Noise_146.png", TEXTURE_TYPE::AMBIENT);
		MaterialDat->Link_Texture(G_GlobalLevelKey, "moon_bright.png", TEXTURE_TYPE::SPECULAR);
		MaterialDat->Link_Texture(G_GlobalLevelKey, "moon_glow.png", TEXTURE_TYPE::METALNESS);
	}

	return S_OK;
}

void CProceduralSky::Awake()
{
}

void CProceduralSky::Priority_Update(_float dt)
{
}

void CProceduralSky::Update(_float dt)
{
	m_fAccTime += dt;

	LIGHT_DESC Desc = LightManager()->Get_MainDirectional();
	Vector4 dir = Desc.vLightDirection;
	m_vSunDirection = Vector3(dir);

	m_MatProjectionInv = *CameraManager()->Get_InversedProjMatrix();
	m_MatViewInv = *CameraManager()->Get_InversedViewMatrix();
}

void CProceduralSky::Late_Update(_float dt)
{
}

void CProceduralSky::Render_GUI()
{
	ImGui::SeparatorText("Sky");

	float topColor[3] = { m_vTopColor.x, m_vTopColor.y, m_vTopColor.z };
	if (ImGui::ColorEdit3("Top Color", topColor,
		ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB))
	{
		m_vTopColor.x = topColor[0];
		m_vTopColor.y = topColor[1];
		m_vTopColor.z = topColor[2];
	}

	float horizonColor[3] = { m_vHorizonColor.x, m_vHorizonColor.y, m_vHorizonColor.z };
	if (ImGui::ColorEdit3("Horizon Color", horizonColor,
		ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB))
	{
		m_vHorizonColor.x = horizonColor[0];
		m_vHorizonColor.y = horizonColor[1];
		m_vHorizonColor.z = horizonColor[2];
	}

	float hazeColor[3] = { m_vHazeColor.x, m_vHazeColor.y, m_vHazeColor.z };
	if (ImGui::ColorEdit3("Haze Color", hazeColor,
		ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB))
	{
		m_vHazeColor.x = hazeColor[0];
		m_vHazeColor.y = hazeColor[1];
		m_vHazeColor.z = hazeColor[2];
	}

	ImGui::SliderFloat("Atmosphere Blend", &m_fSkyAtmosphereBlend, 0.0f, 1.0f, "%.2f");

	ImGui::SeparatorText("Cloud");

	float cloudBright[3] = { m_vCloudBright.x, m_vCloudBright.y, m_vCloudBright.z };
	if (ImGui::ColorEdit3("Cloud Bright", cloudBright,
		ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_HDR))
	{
		m_vCloudBright.x = cloudBright[0];
		m_vCloudBright.y = cloudBright[1];
		m_vCloudBright.z = cloudBright[2];
	}

	float cloudDark[3] = { m_vCloudDark.x, m_vCloudDark.y, m_vCloudDark.z };
	if (ImGui::ColorEdit3("Cloud Dark", cloudDark,
		ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_HDR))
	{
		m_vCloudDark.x = cloudDark[0];
		m_vCloudDark.y = cloudDark[1];
		m_vCloudDark.z = cloudDark[2];
	}

	ImGui::SliderFloat("Coverage", &m_fCloudCoverage, 0.0f, 1.0f, "%.2f");

	ImGui::SeparatorText("Preview");
	ImGui::ColorButton("##TopPrev", ImVec4(m_vTopColor.x, m_vTopColor.y, m_vTopColor.z, 1.0f),
		ImGuiColorEditFlags_NoTooltip, ImVec2(18, 18));
	ImGui::SameLine(); ImGui::Text("Top");
	ImGui::SameLine();
	ImGui::ColorButton("##HorizonPrev", ImVec4(m_vHorizonColor.x, m_vHorizonColor.y, m_vHorizonColor.z, 1.0f),
		ImGuiColorEditFlags_NoTooltip, ImVec2(18, 18));
	ImGui::SameLine(); ImGui::Text("Horizon");
	ImGui::SameLine();
	ImGui::ColorButton("##HazePrev", ImVec4(m_vHazeColor.x, m_vHazeColor.y, m_vHazeColor.z, 1.0f),
		ImGuiColorEditFlags_NoTooltip, ImVec2(18, 18));
	ImGui::SameLine(); ImGui::Text("Haze");
	ImGui::SameLine();
	ImGui::ColorButton("##BrightPrev", ImVec4(m_vCloudBright.x, m_vCloudBright.y, m_vCloudBright.z, 1.0f),
		ImGuiColorEditFlags_NoTooltip, ImVec2(18, 18));
	ImGui::SameLine(); ImGui::Text("Bright");
	ImGui::SameLine();
	ImGui::ColorButton("##DarkPrev", ImVec4(m_vCloudDark.x, m_vCloudDark.y, m_vCloudDark.z, 1.0f),
		ImGuiColorEditFlags_NoTooltip, ImVec2(18, 18));
	ImGui::SameLine(); ImGui::Text("Dark");

}

CProceduralSky* CProceduralSky::Create()
{
	CProceduralSky* Instance = new CProceduralSky();
	if (FAILED(Instance->Initialize_Prototype()))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

CGameObject* CProceduralSky::Clone(INIT_DESC* pArg)
{
	CProceduralSky* Instance = new CProceduralSky(*this);
	if (FAILED(Instance->Initialize(pArg)))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

void CProceduralSky::Free()
{
	__super::Free();
}
