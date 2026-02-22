      #include "pch.h"
#include "SacrificeHand.h"
#include "GameInstance.h"
#include "Texture.h"

/* Object */
#include "EffectContainer.h"
#include "Hand_Core.h"
#include "Hand_Sword.h"

/* Component */
#include "SkeletalModel.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "MaterialData.h"
#include "ObjectContainer.h"
#include "Animator3D.h"
#include "AudioSource.h"
#include "BoneFollower.h"

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
	Add_Component<CAudioSource>();
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

	auto pAudio = Get_Component<CAudioSource>();
	pAudio->SoundFolder(G_GlobalLevelKey, "../Bin/Resources/Zero/Enemy/Sacrifice/Sound");

	if (FAILED(Initialize_StateMachine()))
		return E_FAIL;

	if (FAILED(Create_Children()))
		return E_FAIL;

	if (FAILED(Initialize_Effects()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSacrificeHand::Initialize_Effects()
{
	auto pObjectContainer = Get_Component<CObjectContainer>();

	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("attack_range.json")
			.Build("Attack_Range");

		pEffect->Get_Component<CTransform>()->Scale(_float3(8.f, 8.f, 8.f));
		pEffect->Stop();
		pObjectContainer->Add_Child(pEffect, false);
	}

	return S_OK;
}

void CSacrificeHand::Awake()
{
	m_fDissolveTilling = 5.f;
	m_vRimLightColor = _float3(1.f, 0.f, 0.f);
	m_fRimLightPower = 0.f;

	auto pMaterial = Get_Component<CMaterial>();
	auto& materialInstances = pMaterial->Get_MaterialInstances();
	auto dissolveTexture = ResourceManager()->Load_Texture(G_GlobalLevelKey, "Dissolve.png");

	for (_uint i = 0; i < materialInstances.size(); ++i)
	{
		/* Ä®¸¸ ¸²¶óÀÌÆ® ¸ÔÀ½ */
		if (2 == i)
			materialInstances[i]->Set_Param("fRimLightPower", { &m_fSwordRimLightPower,"float",sizeof(_float) });
		else
			materialInstances[i]->Set_Param("fRimLightPower", { &m_fRimLightPower,"float",sizeof(_float) });

		materialInstances[i]->Set_Param("NoiseTexture", { dissolveTexture->Get_SRV(),"Texture2D",0 });
		materialInstances[i]->Set_Param("vRimLightColor", { &m_vRimLightColor,"float3",sizeof(_float3) });
		materialInstances[i]->Set_Param("fDissolveProgress", { &m_fDissolveProgress,"float",sizeof(_float) });
		materialInstances[i]->Set_Param("fDissolveTiling", { &m_fDissolveTilling,"float",sizeof(_float) });
	}
}

void CSacrificeHand::Priority_Update(_float dt)
{
}

void CSacrificeHand::Update(_float dt)
{
	__super::Update(dt);

	m_pStateMachine->Update(dt);
	Get_Component<CAnimator3D>()->Update_Animation(dt);
	Route_AnimEvent();
}

void CSacrificeHand::Late_Update(_float dt)
{
}

void CSacrificeHand::Route_AnimEvent()
{
	auto pAnimator = Get_Component<CAnimator3D>();
	auto bus = pAnimator->Get_EventBus();

	for (EVENT_INST& instance : bus)
	{
		switch (instance.Type)
		{
		case CLIP_EVENT_TYPE::NOTIFY:
			break;

		case CLIP_EVENT_TYPE::SOUND:
			Control_Sound(instance.Tag);
			break;
		}
	}
}

void CSacrificeHand::Control_Sound(const string& event)
{
	Get_Component<CAudioSource>()->Slot(event).Volume(0.6f).Attribute3D(false).Loop(false).Play();
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
	m_AttackBlackBoard.eCurrPattern = PATTERN::PHASE1;
	m_pStateMachine->Change_State("Attack");
}

void CSacrificeHand::Phase2Attack()
{
	m_isAlive = true;
	m_AttackBlackBoard.eCurrPattern = PATTERN::PHASE2;
	m_pStateMachine->Change_State("Attack");
}

void CSacrificeHand::OverDrive_Start()
{
	m_isAlive = true;
	m_AttackBlackBoard.eCurrPattern = PATTERN::OVER_DRIVE_START;
	m_pStateMachine->Change_State("Attack");
}

void CSacrificeHand::OverDrive_Attack1()
{
	m_isAlive = true;
	m_AttackBlackBoard.eCurrPattern = PATTERN::OVER_DRIVE_ATTACK01;
	m_pStateMachine->Change_State("Attack");
}

void CSacrificeHand::OverDrive_Attack2()
{
	m_isAlive = true;
	m_AttackBlackBoard.eCurrPattern = PATTERN::OVER_DRIVE_ATTACK02;
	m_pStateMachine->Change_State("Attack");
}

void CSacrificeHand::OverDrive_Attack3()
{
	m_isAlive = true;
	m_AttackBlackBoard.eCurrPattern = PATTERN::OVER_DRIVE_ATTACK03;
	m_pStateMachine->Change_State("Attack");
}

void CSacrificeHand::SetVisable(_bool isActive)
{
	auto pModel = Get_Component<CSkeletalModel>();
	_uint iMeshCount = pModel->Get_MeshCount();

	for (_uint i = 0; i < iMeshCount; ++i)
		pModel->SetDrawable(i, isActive);
}

void CSacrificeHand::Idle()
{
	m_pStateMachine->Change_State("Idle");
	SetVisable(false);
}

void CSacrificeHand::Active_Bubble()
{
	auto pObjectContainer = Get_Component<CObjectContainer>();
	auto pBubble = pObjectContainer->Find_ObjectByName("Sacrifice_Hand_Bubble");
	static_cast<CEffectContainer*>(pBubble)->Play();
}

void CSacrificeHand::Deactive_Bubble()
{
	auto pObjectContainer = Get_Component<CObjectContainer>();
	auto pBubble = pObjectContainer->Find_ObjectByName("Sacrifice_Hand_Bubble");
	static_cast<CEffectContainer*>(pBubble)->Stop();
}

void CSacrificeHand::Set_DissolveState(DISSOLVE_STATE state, _float duration)
{
	m_eDissolveState = state;
	m_fDissolveDuration = duration;
	m_fDissolveElapsedTime = 0.f;

	if (DISSOLVE_STATE::APPEAR == state)
		m_fDissolveProgress = 1.1f;
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

void CSacrificeHand::Active_HandCore()
{
	auto pHandCore = Get_Component<CObjectContainer>()->Find_ObjectByName("Hand_Core");
	if(pHandCore)
		static_cast<CHand_Core*>(pHandCore)->Active_Hand();
}

void CSacrificeHand::Deactive_HandCore()
{
	auto pHandCore = Get_Component<CObjectContainer>()->Find_ObjectByName("Hand_Core");
	if (pHandCore)
		static_cast<CHand_Core*>(pHandCore)->Deactive_Hand();
}

void CSacrificeHand::Active_Sword()
{
	auto pHandSword = Get_Component<CObjectContainer>()->Find_ObjectByName("Hand_Sword");
	if (pHandSword)
		static_cast<CHand_Sword*>(pHandSword)->Active_Sword();
}

void CSacrificeHand::Deactive_Sword()
{
	auto pHandSword = Get_Component<CObjectContainer>()->Find_ObjectByName("Hand_Sword");
	if (pHandSword)
		static_cast<CHand_Sword*>(pHandSword)->Deactive_Sword();
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

HRESULT CSacrificeHand::Create_Children()
{
	auto pAnimator = Get_Component<CAnimator3D>();
	auto pObjectContainer = Get_Component<CObjectContainer>();

	{
		COLLIDER_DESC desc{};
		desc.eType = COLLIDER_TYPE::SPHERE;

		auto pHandCore = Builder::Create_Object({ "Zero_Level","Proto_GameObject_HandCore" })
			.Collider(desc)
			.Build("Hand_Core");

		if (pHandCore)
		{
			pHandCore->Get_Component<CBoneFollower>()->Link_Bone(pAnimator, "Eye01_A1");
			pObjectContainer->Add_Child(pHandCore, false);
		}
	}

	{
		COLLIDER_DESC desc{};
		desc.eType = COLLIDER_TYPE::BOX;

		auto pHandSword = Builder::Create_Object({ "Zero_Level","Proto_GameObject_HandSword" })
			.Collider(desc)
			.Build("Hand_Sword");

		if (pHandSword)
		{
			pHandSword->Get_Component<CBoneFollower>()->Link_Bone(pAnimator, "Ctr_HSword");
			pObjectContainer->Add_Child(pHandSword, false);
		}
	}

	{
		auto pBubble = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("sacrifice_hand_bubble.json")
			.Build("Sacrifice_Hand_Bubble");
		//pBubble->Stop();
		pObjectContainer->Add_Child(pBubble, false);

		_smatrix offsetMatrix = _smatrix::Identity;
		offsetMatrix.Translation(_vector3(-4.f, 0.f, 0.f));
		pBubble->AttachBone(pAnimator, "Ctr_Main", offsetMatrix);
	}
	return S_OK;
}
