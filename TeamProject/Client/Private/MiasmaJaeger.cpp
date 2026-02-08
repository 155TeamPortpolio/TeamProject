#include "pch.h"
#include "MiasmaJaeger.h"

#include "BattleSystem.h"
#include "GameInstance.h"

#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"
#include "CharacterController.h"
#include "ObjectContainer.h"

#include "Texture.h"

#include "Helper_Func.h"
#include "Character.h"
#include "StateMachine.h"
#include "MiasmaJaegerState.h"

CMiasmaJaeger::CMiasmaJaeger()
	: CEnemy()
{
}

CMiasmaJaeger::CMiasmaJaeger(const CMiasmaJaeger& rhs)
	:CEnemy(rhs)
{
}

HRESULT CMiasmaJaeger::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CSkeletalModel>()->Link_Model("Zero_Level", "MiasmaJaeger.model");
	Add_Component<CMaterial>()->Link_Material("Zero_Level", "MiasmaJaeger.mat");
	Add_Component<CCharacterController>();
	Add_Component<CAnimator3D>();

	return S_OK;
}

HRESULT CMiasmaJaeger::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);
	auto pAnimator = Get_Component<CAnimator3D>();
	pAnimator->LinkAnimate_Model("Zero_Level", "MiasmaJaeger.model");
	pAnimator->Link_MetaData("Zero_Level", "MiasmaJaeger_Meta.json");
	pAnimator->Resize_Layer(2);
	pAnimator->Set_LayerType(ANIM_LAYER_STATE::ADDITIVE, 1);
	pAnimator->Set_Animation("GrenadierJaeger_Ani_Idle").Loop(true).Apply();

	auto pModel = Get_Component<CSkeletalModel>();
	pModel->Hide_MehsByName("GrenadierJaeger_Weapon_02");
	pModel->Hide_MehsByName("guardjaeger_weaopn_01");

	if (FAILED(Initialize_StateMachine()))
		return E_FAIL;
	return S_OK;
}

void CMiasmaJaeger::Awake()
{
	m_vRimLightColor = _float3(0.127, 0.029, 0.070);
	m_fRimLightPower = 2.2f;
	m_fDissolveTilling = 9.f;

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

	m_Dissolve.DisAppear(0.f);
}

void CMiasmaJaeger::Priority_Update(_float dt)
{
}

void CMiasmaJaeger::Update(_float dt)
{
	if (!m_LockedOn) {
		m_PlayerCharacterInfos.clear();
		m_PlayerCharacterInfos = BattleSystem()->GetBattleObjects(CBattleSystem::BATTLE_OBJ_TYPE::PLAYER);
		ComputeTargetingInfo();
	}
	Get_Component<CAnimator3D>()->Update_Animation(dt);
	Get_Component<CCharacterController>()->Update(dt);

	Update_Dissolve(dt);
	m_pStateMachine->Update(dt);
}

void CMiasmaJaeger::Late_Update(_float dt)
{
}

void CMiasmaJaeger::Render_GUI()
{
	__super::Render_GUI();
}

void CMiasmaJaeger::OnPooledAcquire(INIT_DESC* pArg)
{

}

void CMiasmaJaeger::OnPooledRelease()
{
	m_isOnAttack = false;
}

CMiasmaJaeger* CMiasmaJaeger::Create()
{
	CMiasmaJaeger* instance = new CMiasmaJaeger();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CMiasmaJaeger");
	}

	return instance;
}

CGameObject* CMiasmaJaeger::Clone(INIT_DESC* pArg)
{
	CMiasmaJaeger* instance = new CMiasmaJaeger(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CMiasmaJaeger");
	}

	return instance;
}

void CMiasmaJaeger::Free()
{
	__super::Free();
	Safe_Release(m_pStateMachine);
}

void CMiasmaJaeger::OnTriggerEnter(CGameObject* pOther)

{
	auto pCollidable = pOther->Get_Component<ICollidable>();
	if (pCollidable)
		return;

	else {
		auto pEnemy = dynamic_cast<CCharacter*>(pOther);
		if (nullptr != pEnemy)
		{
			pEnemy->Take_Damage(DAMAGE_TYPE::NORMAL, 10);
			CameraManager()->AddImpact(1, 0);
		}
	}
}

void CMiasmaJaeger::Update_Dissolve(_float dt)
{

	if (m_Dissolve.fDissolveElapsedTime < m_Dissolve.fDissolveDuration)
	{
		m_Dissolve.fDissolveElapsedTime += dt;
		_float t = m_Dissolve.fDissolveElapsedTime / m_Dissolve.fDissolveDuration;

		switch (m_Dissolve.eDissolveState)
		{
		case MiasmaJaegerDisolveState::DISAPPEAR:
		{
			m_fDissolveProgress = t;
		}break;
		case MiasmaJaegerDisolveState::DISSOLVE_STATE::APPEAR:
		{
			m_fDissolveProgress = 1.f - t;
		}break;
		case MiasmaJaegerDisolveState::DISSOLVE_STATE::NONE:
			break;
		default:
			break;
		}
	}
	else
	{
		if (MiasmaJaegerDisolveState::DISAPPEAR == m_Dissolve.eDissolveState)
			m_fDissolveProgress = 1.01f;
		else
			m_fDissolveProgress = 0.f;
	}
}

void CMiasmaJaeger::RotateToTarget(_float dt, _float rotateSpeed)
{
	_vector3 vPosition = m_pTransform->Get_Pos();
	_vector3 vCurrDir = m_pTransform->Dir(STATE::LOOK);
	_vector3 vTargetDir = m_tTargetingInfo.vDirToTarget;
	vCurrDir.Normalize();
	vTargetDir.Normalize();

	if (vCurrDir.Dot(vTargetDir) >= 0.99f)
		return;

	vCurrDir = _vector3::Lerp(vCurrDir, vTargetDir, dt * rotateSpeed);
	m_pTransform->Set_Look(vCurrDir);
}

HRESULT CMiasmaJaeger::Initialize_StateMachine()
{
	m_pStateMachine = CStateMachine<CMiasmaJaeger>::Create();
	if (!m_pStateMachine)
		return E_FAIL;

	if (FAILED(Initialize_States()))
		return E_FAIL;

	if (FAILED(Initialize_Transitions()))
		return E_FAIL;

	m_pStateMachine->Set_DefaultState("Appear");
	m_pStateMachine->Initialize(this);

	return S_OK;
}

HRESULT CMiasmaJaeger::Initialize_States()
{
	m_pStateMachine->Register_State("Appear", CMiasmaJaeger_Appear::Create());
	m_pStateMachine->Register_State("Attack", CMiasmaJaeger_Attack::Create());
	m_pStateMachine->Register_State("DisAppear", CMiasmaJaeger_DisAppear::Create());

	//m_pStateMachine->Register_State("Walk", CSacrificeState_Walk::Create());
	//m_pStateMachine->Register_State("Hit", CSacrificeState_Hit::Create());
	//m_pStateMachine->Register_State("Evade", CSacrificeState_Evade::Create());
	//m_pStateMachine->Register_State("Death", CSacrificeState_Death::Create());
	//m_pStateMachine->Register_State("ChangePhase", CSacrificeState_ChangePhase::Create());
	//m_pStateMachine->Register_State("Parry", CSacrificeState_Parry::Create());
	//m_pStateMachine->Register_State("Groggy", CSacrificeState_Groggy::Create());

	return S_OK;
}

HRESULT CMiasmaJaeger::Initialize_Transitions()
{
	m_pStateMachine->Register_Transition("Appear", "Attack",
		CStateMachine<CMiasmaJaeger>::CONDITION_TRIGGER, "Appear_To_Attack");
	m_pStateMachine->Register_Transition("Attack", "DisAppear",
		CStateMachine<CMiasmaJaeger>::CONDITION_TRIGGER, "Attack_To_DisAppear");
	return S_OK;
}
