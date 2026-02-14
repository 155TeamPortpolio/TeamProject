#include "pch.h"
#include "WaterWaves.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

#include "WaterWave.h"

CWaterWaves::CWaterWaves()
    :CGameObject()
{
}

CWaterWaves::CWaterWaves(const CWaterWaves& rhs)
    :CGameObject(rhs)
{
}

HRESULT CWaterWaves::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    Add_Component<CObjectContainer>();

    return S_OK;
}

HRESULT CWaterWaves::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);
    return S_OK;
}

void CWaterWaves::Awake()
{
	Add_WaterWave();
}

void CWaterWaves::Priority_Update(_float dt)
{
    if(Get_Component<CObjectContainer>())
        Get_Component<CObjectContainer>()->Priority_UpdateChild(dt);
}

void CWaterWaves::Update(_float dt)
{
    if (Get_Component<CObjectContainer>())
        Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CWaterWaves::Late_Update(_float dt)
{
    if (Get_Component<CObjectContainer>())
        Get_Component<CObjectContainer>()->Late_UpdateChild(dt);
}

HRESULT CWaterWaves::Add_WaterWave()
{
	auto pObjectContainer = Get_Component<CObjectContainer>();

	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_WaterWave", CWaterWave::Create());
	CWaterWave::WaterWaveDesc* waveDesc = new CWaterWave::WaterWaveDesc;
	waveDesc->fRoughness = 1.0f;
	waveDesc->fFoamAmount = 1.0f;
	waveDesc->fNoiseOffset = _float2(0.f, 0.f);
	waveDesc->vWaterTint = _float3(1.f, 1.f, 1.f);
	waveDesc->fTintStrength = 0.f;

	CGameObject* WaterWave1 = Builder::Create_Object({ "Zero_Level", "Proto_GameObject_WaterWave" })
		.Add_ObjDesc(waveDesc)
		.Build("WaterWave1");

	pObjectContainer->Add_Child(WaterWave1);

	//waveDesc = new CWaterWave::WaterWaveDesc;
	//waveDesc->fRoughness = 0.3f;
	//waveDesc->fFoamAmount = 0.15f;
	//waveDesc->fNoiseOffset = _float2(137.f, 53.f);
	//waveDesc->vWaterTint = _float3(0.3f, 0.08f, 0.8f);
	//waveDesc->fTintStrength = 0.85f;
	//CGameObject* WaterWave2 = Builder::Create_Object({ "Zero_Level", "Proto_GameObject_WaterWave" })
	//	.Add_ObjDesc(waveDesc)
	//	.Build("WaterWave2");
	//pObjectContainer->Add_Child(WaterWave2);

	/*waveDesc = new CWaterWave::WaterWaveDesc;
	waveDesc->fRoughness = 1.6f;
	waveDesc->fFoamAmount = 1.4f;
	waveDesc->fNoiseOffset = _float2(291.f, 179.f);
	waveDesc->vWaterTint = _float3(1.3f, 0.5f, 0.6f); 
	waveDesc->fTintStrength = 0.4f;
	CGameObject* WaterWave3 = Builder::Create_Object({ "Zero_Level", "Proto_GameObject_WaterWave" })
		.Add_ObjDesc(waveDesc)
		.Build("WaterWave3");
	pObjectContainer->Add_Child(WaterWave3);*/

	return S_OK;
}

CWaterWaves* CWaterWaves::Create()
{
	CWaterWaves* Instance = new CWaterWaves();
	if (FAILED(Instance->Initialize_Prototype()))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

CGameObject* CWaterWaves::Clone(INIT_DESC* pArg)
{
	CWaterWaves* Instance = new CWaterWaves(*this);
	if (FAILED(Instance->Initialize(pArg)))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

void CWaterWaves::Free()
{
	__super::Free();
}
