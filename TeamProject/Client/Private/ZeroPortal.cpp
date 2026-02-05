#include "pch.h"
#include "ZeroPortal.h"
#include "LevelMgr.h"
#include "GameInstance.h"

//Components
#include "Material.h"
#include "MaterialInstance.h"
#include "MaterialData.h"
#include "ParticleSystem.h"
#include "EventListener.h"
#include "Stage.h"

CZeroPortal::CZeroPortal()
	: CInteractable()
{
}

CZeroPortal::CZeroPortal(const CZeroPortal& rhs)
	: CInteractable(rhs)
{
}

HRESULT CZeroPortal::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CParticleSystem>();
	Add_Component<CMaterial>();
	Add_Component<CEventListener>();

	return S_OK;
}

HRESULT CZeroPortal::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	EFFECT_ASSET asset = ResourceManager()->Load_EffectAsset(G_GlobalLevelKey, "zero_portal.json");
	if (asset.iNodeCount < 1)
		return E_FAIL;
	PARTICLE_NODE* particleParam = static_cast<PARTICLE_NODE*>(asset.Nodes[0]);


	ID3D11Device* pDevice = CGameInstance::GetInstance()->Get_Device();
	auto pModel = Get_Component<CParticleSystem>();
	pModel->ShadowCast(false);
	pModel->SetParticleParams(*particleParam);

	CMaterial* pMaterial = Get_Component<CMaterial>();
	CMaterialInstance* customInstance = CMaterialInstance::Create_Handle("Point_Effect_Base", "Default", pDevice);
	customInstance->ChangeTexture(TEXTURE_TYPE::DIFFUSE, 0);
	customInstance->Set_Blended(true);

	pMaterial->Insert_MaterialInstance(customInstance, nullptr);

	auto MaterialDat = customInstance->Get_MaterialData();
	if (MaterialDat)
	{
		MaterialDat->Link_Shader(G_GlobalLevelKey, "VTX_InstancePoint.hlsl");
		MaterialDat->Link_Texture(G_GlobalLevelKey, particleParam->TextureKey, TEXTURE_TYPE::DIFFUSE);
	}


	return S_OK;
}

void CZeroPortal::Awake()
{

}

void CZeroPortal::Priority_Update(_float dt)
{
}

void CZeroPortal::Update(_float dt)
{
	m_Time += dt;
	Get_Component<CCollider>()->Update(dt);
	Get_Component<CParticleSystem>()->Simulation_Particle(dt);
	//Extend(dt);

	if (m_bIsInteractable) {
		//Interact();
		//m_vTargetSize = { 3.f,4.f,3.f };
	}
	else {
		//m_vTargetSize = { 1.f,1.f,1.f };
	}
}

void CZeroPortal::Late_Update(_float dt)
{
}

void CZeroPortal::OnTriggerEnter(CGameObject* pOther)
{
	auto pCollidable = pOther->Get_Component<ICollidable>();
	if (pCollidable && (pCollidable->Get_Group() != COLLISION_GROUP::PLAYER))
		return;

	m_bIsInteractable = true;
}

void CZeroPortal::OnTriggerStay(CGameObject* pOher)
{

}

void CZeroPortal::OnTriggerExit(CGameObject* pOther)
{
	auto pCollidable = pOther->Get_Component<ICollidable>();
	if (pCollidable && (pCollidable->Get_Group() != COLLISION_GROUP::PLAYER))
		return;

	m_bIsInteractable = false;
}

void CZeroPortal::Interact(CGameObject* pObject)
{
	if (!m_bIsInteractable)
		return;

	m_pOwnerStage->StageChangeOn(m_choiceIndex);
	m_bIsInteractable = false;
}

OBJECT_HANDLE CZeroPortal::Get_InteractHandle()
{
	return Get_Handle();
}

void CZeroPortal::SetChoiceIndex(CStage* pOwener, int idx)
{
	m_pOwnerStage = pOwener;
	m_choiceIndex = idx;
}

void CZeroPortal::Extend(_float dt)
{
}

void CZeroPortal::Contract(_float dt)
{
}

CZeroPortal* CZeroPortal::Create()
{
	CZeroPortal* Instance = new CZeroPortal();
	if (FAILED(Instance->Initialize_Prototype()))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

CGameObject* CZeroPortal::Clone(INIT_DESC* pArg)
{
	CZeroPortal* Instance = new CZeroPortal(*this);
	if (FAILED(Instance->Initialize(pArg)))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

void CZeroPortal::Free()
{
	__super::Free();
}


