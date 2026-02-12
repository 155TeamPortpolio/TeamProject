#include "pch.h"
#include "WaterWave.h"

#include "TessellationModel.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "MaterialData.h"
#include "GameInstance.h"

CWaterWave::CWaterWave()
	:CGameObject()
{
}

CWaterWave::CWaterWave(const CWaterWave& rhs)
	:CGameObject(rhs)
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

	auto pModel = Get_Component<CTessellationModel>();
	pModel->ShadowCast(false);
	pModel->Set_RenderType(RENDER_PASS_TYPE::NONLIGHT_OPAQUE);
	ID3D11Device* pDevice = CGameInstance::GetInstance()->Get_Device();
	CMaterial* pMaterial = Get_Component<CMaterial>();
	CMaterialInstance* customInstance = CMaterialInstance::Create_Handle("Tessellation", "Opaque", pDevice);
	customInstance->ChangeTexture(TEXTURE_TYPE::DIFFUSE, 0);
	customInstance->Set_Param("g_Time", { &m_fAccTime, "float", sizeof(_float) });
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

	constexpr float CycleTime = 7.0f;
	constexpr float PeakTime = 0.3f;

	constexpr float CrashMoveDistance = 30.0f; 
	_float3 CrashDirection = { 0.f, 0.f, -1.f };

	float t = fmodf(m_fAccTime, CycleTime) / CycleTime;

	float overPeak = (t - PeakTime * 0.9f) / (1.0f - PeakTime * 0.9f);
	overPeak = max(0.f, min(1.f, overPeak)); // saturate

	float moveFactor = overPeak * overPeak * (3.0f - 2.0f * overPeak);

	float fadeOut = 1.0f - max(0.f, min(1.f, (t - 0.85f) / 0.15f));
	fadeOut = fadeOut * fadeOut * (3.0f - 2.0f * fadeOut);

	float finalMove = moveFactor * fadeOut * CrashMoveDistance;

	_float3 newPos;
	newPos.x = m_vOriginPos.x + CrashDirection.x * finalMove;
	newPos.y = m_vOriginPos.y + CrashDirection.y * finalMove;
	newPos.z = m_vOriginPos.z + CrashDirection.z * finalMove;

	m_pTransform->Set_Pos(newPos);
}

void CWaterWave::Late_Update(_float dt)
{
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
