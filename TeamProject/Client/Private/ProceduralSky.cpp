#include "pch.h"
#include "ProceduralSky.h"

#include "TriangleModel.h"
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

HRESULT CProceduralSky::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CTriangleModel>();
	Add_Component<CMaterial>();
	return S_OK;
}

HRESULT CProceduralSky::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);
	auto pModel = Get_Component<CTriangleModel>();
	pModel->ShadowCast(false);
	pModel->Set_RenderType(RENDER_PASS_TYPE::PRIORITY);
	ID3D11Device* pDevice = CGameInstance::GetInstance()->Get_Device();
	CMaterial* pMaterial = Get_Component<CMaterial>();

	CMaterialInstance* customInstance = CMaterialInstance::Create_Handle("Cloud", "Opaque", pDevice);
	customInstance->ChangeTexture(TEXTURE_TYPE::DIFFUSE, 0);
	customInstance->Set_Param("g_Time", { &m_fAccTime, "float", sizeof(_float) });
	customInstance->Set_Param("SunDir", { &m_vSunDirection, "float3", sizeof(_float3) });
	customInstance->Set_Param("TopColor", { &m_vTopColor, "float3", sizeof(_float3) });
	customInstance->Set_Param("HorizonColor", { &m_vHorizonColor, "float3", sizeof(_float3) });
	customInstance->Set_Param("GroundColor", { &m_vGroundColor, "float3", sizeof(_float3) });
	
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
