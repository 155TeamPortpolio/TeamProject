#include "pch.h"
#include "SacrificeHand.h"
#include "GameInstance.h"
#include "Texture.h"

/* Component */
#include "SkeletalModel.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "MaterialData.h"
#include "Animator3D.h"

/* State */
#include "StateMachine.h"
#include "SacrificeHandState_Attack.h"
#include "SacrificeHandState_Idle.h"	

CSacrificeHand::CSacrificeHand()
	:CEnemy()
{
}

CSacrificeHand::CSacrificeHand(const CSacrificeHand& rhg)
	:CEnemy(rhg)
{
}

HRESULT CSacrificeHand::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CSkeletalModel>();
	Add_Component<CMaterial>();
	Add_Component<CAnimator3D>();

	auto pResource = CGameInstance::GetInstance()->Get_ResourceMgr();
	pResource->Add_ResourcePath("Monster_SacrificeBringerHand.model", "../Bin/Resources/Model/skeletal/Enemy/Sacrifice/Hand/Monster_SacrificeBringerHand.model");
	pResource->Add_ResourcePath("Monster_SacrificeBringerHand.mat", "../Bin/Resources/Model/skeletal/Enemy/Sacrifice/Hand/Monster_SacrificeBringerHand.mat");
	pResource->Add_ResourcePath("Monster_SacrificeBringerHand_Meta.json", "../Bin/Resources/Model/skeletal/Enemy/Sacrifice/Hand/Monster_SacrificeBringerHand_Meta.json");

	return S_OK;
}

HRESULT CSacrificeHand::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	auto pModel = Get_Component<CSkeletalModel>();
	pModel->Link_Model(G_GlobalLevelKey, "Monster_SacrificeBringerHand.model");

	auto pMaterial = Get_Component<CMaterial>();
	pMaterial->Link_Material(G_GlobalLevelKey, "Monster_SacrificeBringerHand.mat");

	auto pAnimator = Get_Component<CAnimator3D>();
	pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "Monster_SacrificeBringerHand.model");
	pAnimator->Link_MetaData(G_GlobalLevelKey, "Monster_SacrificeBringerHand_Meta.json");

	if (FAILED(Initialize_StateMachine()))
		return E_FAIL;

	return S_OK;
}

void CSacrificeHand::Awake()
{
	m_fDissolveTilling = 5.f;

	auto pMaterial = Get_Component<CMaterial>();
	auto& materialInstances = pMaterial->Get_MaterialInstances();
	auto dissolveTexture = ResourceManager()->Load_Texture(G_GlobalLevelKey, "Dissolve.png");

	for (const auto& instance : materialInstances)
	{
		instance->Set_Param("NoiseTexture", { dissolveTexture->Get_SRV(),"Texture2D",0 });
		instance->Set_Param("vRimLightColor", { &m_vRimLightColor,"float3",sizeof(_float3) });
		instance->Set_Param("fRimLightPower", { &m_fRimLightPower,"float",sizeof(_float) });
		instance->Set_Param("fDissolveProgress", { &m_fDissolveProgress,"float",sizeof(_float) });
		instance->Set_Param("fDissolveTiling", { &m_fDissolveTilling,"float",sizeof(_float) });
	}
}

void CSacrificeHand::Priority_Update(_float dt)
{
}

void CSacrificeHand::Update(_float dt)
{
	Get_Component<CAnimator3D>()->Update_Animation(dt);
	m_pStateMachine->Update(dt);
}

void CSacrificeHand::Late_Update(_float dt)
{
}

CSacrificeHand* CSacrificeHand::Create()
{
	CSacrificeHand* instance = new CSacrificeHand();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CSacrificeHand");
	}

	return instance;
}

CGameObject* CSacrificeHand::Clone(INIT_DESC* pArg)
{
	CSacrificeHand* instance = new CSacrificeHand(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CSacrificeHand");
	}

	return instance;
}

void CSacrificeHand::Free()
{
	__super::Free();

	Safe_Release(m_pStateMachine);
}

void CSacrificeHand::Phase1Attack()
{
	m_isAlive = true;
	SetVisable(true);

	m_AttackBlackBoard.eCurrPattern = PATTERN::PHASE1;
	m_pStateMachine->Change_State("Attack");
}

void CSacrificeHand::Phase2Attack()
{
	m_isAlive = true;
	SetVisable(true);

	m_AttackBlackBoard.eCurrPattern = PATTERN::PHASE2;
	m_pStateMachine->Change_State("Attack");
}

void CSacrificeHand::OverDrive_Start()
{
	m_isAlive = true;
	SetVisable(true);

	m_AttackBlackBoard.eCurrPattern = PATTERN::OVER_DRIVE_START;
	m_pStateMachine->Change_State("Attack");
}

void CSacrificeHand::OverDrive_Attack1()
{
	m_isAlive = true;
	SetVisable(true);

	m_AttackBlackBoard.eCurrPattern = PATTERN::OVER_DRIVE_ATTACK01;
	m_pStateMachine->Change_State("Attack");
}

void CSacrificeHand::OverDrive_Attack2()
{
	m_isAlive = true;
	SetVisable(true);

	m_AttackBlackBoard.eCurrPattern = PATTERN::OVER_DRIVE_ATTACK02;
	m_pStateMachine->Change_State("Attack");
}

void CSacrificeHand::OverDrive_Attack3()
{
	m_isAlive = true;
	SetVisable(true);

	m_AttackBlackBoard.eCurrPattern = PATTERN::OVER_DRIVE_ATTACK03;
	m_pStateMachine->Change_State("Attack");
}

void CSacrificeHand::SetVisable(_bool isActive)
{
	auto pModel = Get_Component<CSkeletalModel>();
	_uint iMeshCount = pModel->Get_MeshCount();

	if (isActive)
	{
		for (_uint i = 0; i < iMeshCount; ++i)
			pModel->SetDrawable(i, true);
	}
	else
	{
		for (_uint i = 0; i < iMeshCount; ++i)
			pModel->SetDrawable(i, false);
	}
}

void CSacrificeHand::Idle()
{
	m_pStateMachine->Change_State("Idle");
	SetVisable(false);
}

void CSacrificeHand::Set_DissolveState(DISSOLVE_STATE state, _float duration)
{
	m_eDissolveState = state;
	m_fDissolveDuration = duration;
	m_fDissolveElapsedTime = 0.f;

	if (DISSOLVE_STATE::APPEAR == state)
		m_fDissolveProgress = 1.f;
	else
		m_fDissolveProgress = 0.f;
}

void CSacrificeHand::Update_Dissolve(_float dt)
{
	if (m_fDissolveDuration > 0.f)
	{
		if (m_fDissolveElapsedTime < m_fDissolveDuration)
		{
			m_fDissolveElapsedTime += dt;
			_float t = m_fDissolveElapsedTime / m_fDissolveDuration;

			switch (m_eDissolveState)
			{
			case Client::CSacrificeHand::DISSOLVE_STATE::DISAPPEAR:
			{
				m_fDissolveProgress = t;
			}break;
			case Client::CSacrificeHand::DISSOLVE_STATE::APPEAR:
			{
				m_fDissolveProgress = 1.f - t;
			}break;
			case Client::CSacrificeHand::DISSOLVE_STATE::NONE:
				break;
			default:
				break;
			}
		}
		else
		{
			if (DISSOLVE_STATE::DISAPPEAR == m_eDissolveState)
				m_fDissolveProgress = 1.01f;
			else
				m_fDissolveProgress = 0.f;
		}
	}
}

HRESULT CSacrificeHand::Initialize_StateMachine()
{
	m_pStateMachine = CStateMachine<CSacrificeHand>::Create();
	if (!m_pStateMachine)
		return E_FAIL;

	if (FAILED(Initialize_States()))
		return E_FAIL;

	if (FAILED(Initialize_Transitions()))
		return E_FAIL;

	m_pStateMachine->Set_DefaultState("Idle");
	m_pStateMachine->Initialize(this);

	return S_OK;
}

HRESULT CSacrificeHand::Initialize_States()
{
	m_pStateMachine->Register_State("Idle", CSacrificeHandState_Idle::Create());
	m_pStateMachine->Register_State("Attack", CSacrificeHandState_Attack::Create());

	return S_OK;
}

HRESULT CSacrificeHand::Initialize_Transitions()
{
	return S_OK;
}
