#include "pch.h"
#include "WaterWave.h"

#include "TessellationModel.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "MaterialData.h"
#include "GameInstance.h"
#include "Child.h"

#include "Helper_Func.h"

CWaterWave::CWaterWave()
	:CGameObject()
{
}

CWaterWave::CWaterWave(const CWaterWave& rhs)
	:CGameObject(rhs), m_Roughness(rhs.m_Roughness) ,m_NoiseOffset(rhs.m_NoiseOffset), m_FoamAmount(rhs.m_FoamAmount)
{
}

HRESULT CWaterWave::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CTessellationModel>(32, 50.f);
	Add_Component<CMaterial>();
	return S_OK;
}

HRESULT CWaterWave::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);
	auto desc = static_cast<WaterWaveDesc*>(pArg);
	Initialize_Wave(*desc);

	auto pModel = Get_Component<CTessellationModel>();
	pModel->ShadowCast(false);
	pModel->Set_RenderType(RENDER_PASS_TYPE::NONLIGHT_OPAQUE);
	ID3D11Device* pDevice = CGameInstance::GetInstance()->Get_Device();
	CMaterial* pMaterial = Get_Component<CMaterial>();
	CMaterialInstance* customInstance = CMaterialInstance::Create_Handle("Tessellation", "Opaque", pDevice);
	customInstance->ChangeTexture(TEXTURE_TYPE::DIFFUSE, 0);
	customInstance->Set_Param("g_Time", { &m_fAccTime, "float", sizeof(_float) });
	customInstance->Set_Param("g_CycleTime", { &m_CycleTime, "float", sizeof(_float) });
	customInstance->Set_Param("g_Roughness", { &m_Roughness, "float", sizeof(_float) });
	customInstance->Set_Param("g_FoamAmount", { &m_FoamAmount, "float", sizeof(_float) });
	customInstance->Set_Param("g_NoiseOffset", { &m_NoiseOffset, "float2", sizeof(_float2) });
	customInstance->Set_Param("g_WaterTint", { &m_WaterTint, "float3", sizeof(_float3) });
	customInstance->Set_Param("g_TintStrength", { &m_TintStrength, "float", sizeof(_float) });
	// 형상
	customInstance->Set_Param("g_TsunamiHeight", { &m_TsunamiHeight,"float",  sizeof(float) });

	// 타이밍
	customInstance->Set_Param("g_PeakTime", { &m_PeakTime,"float",  sizeof(float) });
	customInstance->Set_Param("g_RiseWidth", { &m_RiseWidth, "float", sizeof(float) });
	customInstance->Set_Param("g_FallWidth", {&m_FallWidth, "float", sizeof(float)});
	customInstance->Set_Param("g_FadeInEnd", { &m_FadeInEnd, "float", sizeof(float) });
	customInstance->Set_Param("g_FadeOutStart", { &m_FadeOutStart, "float", sizeof(float) });
	customInstance->Set_Param("g_CurlStartRatio", { &m_CurlStartRatio, "float", sizeof(float) });
	customInstance->Set_Param("g_CurlDuration", { &m_CurlDuration, "float", sizeof(float) });

	// 벽 프로파일
	customInstance->Set_Param("g_WallDepthStart", { &m_WallDepthStart, "float", sizeof(float) });
	customInstance->Set_Param("g_WallDepthEnd", { &m_WallDepthEnd, "float", sizeof(float) });

	// 컬 강도
	customInstance->Set_Param("g_CurlForward", { &m_CurlForward, "float", sizeof(float) });
	customInstance->Set_Param("g_MaxCurlAngle", { &m_MaxCurlAngle, "float", sizeof(float) });
	pMaterial->Insert_MaterialInstance(customInstance, nullptr);
	auto MaterialDat = customInstance->Get_MaterialData();
	if (MaterialDat)
	{
		MaterialDat->Link_Shader(G_GlobalLevelKey, "VTX_Tessellation.hlsl");
		MaterialDat->Link_Texture(G_GlobalLevelKey, "WaterSurface_single.png", TEXTURE_TYPE::DIFFUSE);
		MaterialDat->Link_Texture(G_GlobalLevelKey, "Port_Water_DetailNormal_01_N.png", TEXTURE_TYPE::NORMALS);
		MaterialDat->Link_Texture(G_GlobalLevelKey, "Port_Water_DetailNormal_02_N.png", TEXTURE_TYPE::METALNESS);
		MaterialDat->Link_Texture(G_GlobalLevelKey, "Port_Water_DetailNormal_03_N.png", TEXTURE_TYPE::AMBIENT);
		MaterialDat->Link_Texture(G_GlobalLevelKey, "Port_WaterFoam_single_01.png", TEXTURE_TYPE::LIGHTMAP);
	}

	m_vOriginPos = _vector3(m_pTransform->Get_Pos());
	return S_OK;
}

void CWaterWave::Awake()
{
}

void CWaterWave::Priority_Update(_float dt)
{
}
void CWaterWave::Update(_float dt)
{
	m_fAccTime += dt;

	if (m_CycleTime <= 1e-6f)
		return;

	const _float3 crashDirection = { 1.f, 0.f, 0.f };

	const _float time01 = fmodf(m_fAccTime, m_CycleTime) / m_CycleTime;

	_float progress01 = (time01 - m_PeakTime * 0.9f) / (1.0f - m_PeakTime * 0.9f);
	progress01 = max(0.f, min(1.f, progress01));

	progress01 = progress01 * progress01 * (3.0f - 2.0f * progress01);

	const _float forwardMove = progress01 * m_CrashMoveDistance;

	_float3 newPos;
	newPos.x = m_vOriginPos.x + crashDirection.x * forwardMove;
	newPos.y = m_vOriginPos.y + crashDirection.y * forwardMove;
	newPos.z = m_vOriginPos.z + crashDirection.z * forwardMove;

	m_pTransform->Set_Pos(newPos);
}

void CWaterWave::Late_Update(_float dt)
{
	if (m_fAccTime >= m_CycleTime){
		m_isAlive = false;
		ObjectManager()->Remove_Object(this);
	}
}

void CWaterWave::Initialize_Wave(WaterWaveDesc Desc)
{
	m_FoamAmount =  Desc.fFoamAmount;
	m_Roughness = Desc.fRoughness;
	m_NoiseOffset = Desc.fNoiseOffset;
	m_WaterTint = Desc.vWaterTint;
	m_TintStrength = Desc.fTintStrength;
	m_RiseWidth = Desc.fRiseWidth; 
	m_TsunamiHeight = Desc.fTsunamiHeight;
	m_PeakTime = Desc.fPeakTime;
	m_FallWidth = Desc.fFallWidth;
	m_FadeInEnd = Desc.fFadeInEnd;
	m_FadeOutStart = Desc.fFadeOutStart;
	m_CurlStartRatio = Desc.fCurlStartRatio;
	m_CurlDuration = Desc.fCurlDuration;
	m_WallDepthStart = Desc.fWallDepthStart;
	m_WallDepthEnd = Desc.fWallDepthEnd;
	m_CurlForward = Desc.fCurlForward;
	m_MaxCurlAngle = Desc.fMaxCurlAngle;
}

CWaterWave* CWaterWave::Create()
{
	CWaterWave* Instance = new CWaterWave();
	if (FAILED(Instance->Initialize_Prototype()))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

CGameObject* CWaterWave::Clone(INIT_DESC* pArg)
{
	CWaterWave* Instance = new CWaterWave(*this);
	if (FAILED(Instance->Initialize(pArg)))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

void CWaterWave::Free()
{
	__super::Free();
}
