#include "pch.h"
#include "MiasmaGrandierJaeger.h"

#include "BattleSystem.h"
#include "GameInstance.h"

#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"
#include "CharacterController.h"
#include "ObjectContainer.h"
#include "Texture.h"

#include "StateMachine.h"
#include "MiasmaJaegerState.h"

#include "Helper_Func.h"
#include "UIDirector.h"
#include "UI_DamageText.h"

#include "MiasmaProjectile.h"
#include "Character.h"
#include "AudioSource.h"

CMiasmaGrandierJaeger::CMiasmaGrandierJaeger()
	: CEnemy()
{
}

CMiasmaGrandierJaeger::CMiasmaGrandierJaeger(const CMiasmaGrandierJaeger& rhs)
	:CEnemy(rhs)
{
}

HRESULT CMiasmaGrandierJaeger::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	PrototypeManager()->Add_ProtoType("Zero_Level", "Proto_GameObject_MiasmaProjectile", CMiasmaProjectile::Create());
	Add_Component<CSkeletalModel>()->Link_Model("Zero_Level", "MiasmaJaeger.model");
	Add_Component<CMaterial>()->Link_Material("Zero_Level", "MiasmaJaeger.mat");
	Add_Component<CCharacterController>();
	Add_Component<CObjectContainer>();
	Add_Component<CAnimator3D>();
	Add_Component<CAudioSource>();

	return S_OK;
}

HRESULT CMiasmaGrandierJaeger::Initialize(INIT_DESC* pArg)
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

	if (FAILED(Initialize_Effects()))
		return E_FAIL;

	Get_Component<CAudioSource>()->SoundFolder("Zero_Level","../Bin/Resources/Zero/Enemy/MiasmaJaeger/Sound/");

	return S_OK;
}

void CMiasmaGrandierJaeger::Awake()
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
;
	m_Dissolve.DisAppear(0.f);
}

void CMiasmaGrandierJaeger::Priority_Update(_float dt)
{
	Get_Component<CObjectContainer>()->Priority_UpdateChild(dt);
}

void CMiasmaGrandierJaeger::Update(_float dt)
{
	if (!m_LockedOn) {
		m_PlayerCharacterInfos.clear();
		m_PlayerCharacterInfos = BattleSystem()->GetBattleObjects(CBattleSystem::BATTLE_OBJ_TYPE::PLAYER);
		ComputeTargetingInfo();
	}

	auto pAnimator = Get_Component<CAnimator3D>();
	pAnimator->Update_Animation(dt);
	Route_AnimEvent(pAnimator);
	RotateToTarget(dt, 6.f);
	Get_Component<CCharacterController>()->Update(dt);
	Get_Component<CObjectContainer>()->UpdateChild(dt);
	Update_Dissolve(dt);
	m_pStateMachine->Update(dt);
}

void CMiasmaGrandierJaeger::Late_Update(_float dt)
{
	Get_Component<CObjectContainer>()->Late_UpdateChild(dt);
	Get_Component<CCharacterController>()->Late_Update(dt);
}

void CMiasmaGrandierJaeger::Render_GUI()
{
	__super::Render_GUI();
}

void CMiasmaGrandierJaeger::OnPooledAcquire(INIT_DESC* pArg)
{

}

void CMiasmaGrandierJaeger::OnPooledRelease()
{
	LockOn(false);
	m_isOnAttack = false;
}

CMiasmaGrandierJaeger* CMiasmaGrandierJaeger::Create()
{
	CMiasmaGrandierJaeger* instance = new CMiasmaGrandierJaeger();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CMiasmaGrandierJaeger");
	}

	return instance;
}

CGameObject* CMiasmaGrandierJaeger::Clone(INIT_DESC* pArg)
{
	CMiasmaGrandierJaeger* instance = new CMiasmaGrandierJaeger(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CMiasmaGrandierJaeger");
	}

	return instance;
}

void CMiasmaGrandierJaeger::Free()
{
	__super::Free();
	Safe_Release(m_pStateMachine);
}

void CMiasmaGrandierJaeger::OnTriggerEnter(CGameObject* pOther)

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

void CMiasmaGrandierJaeger::Update_Dissolve(_float dt)
{

	if (m_Dissolve.fDissolveElapsedTime < m_Dissolve.fDissolveDuration)
	{
		m_Dissolve.fDissolveElapsedTime += dt;
		_float t = m_Dissolve.fDissolveElapsedTime / m_Dissolve.fDissolveDuration;

		switch (m_Dissolve.eDissolveState)
		{
		case DefilerDissolve::DISAPPEAR:
		{
			m_fDissolveProgress = t;
		}break;
		case DefilerDissolve::DISSOLVE_STATE::APPEAR:
		{
			m_fDissolveProgress = 1.f - t;
		}break;
		case DefilerDissolve::DISSOLVE_STATE::NONE:
			break;
		default:
			break;
		}
	}
	else
	{
		if (DefilerDissolve::DISAPPEAR == m_Dissolve.eDissolveState)
			m_fDissolveProgress = 1.01f;
		else
			m_fDissolveProgress = 0.f;
	}
}

void CMiasmaGrandierJaeger::RotateToTarget(_float dt, _float rotateSpeed)
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

void CMiasmaGrandierJaeger::Route_AnimEvent(CAnimator3D* animator)
{
	auto Bus = animator->Get_EventBus();

	for (EVENT_INST& instance : Bus)
	{
		switch (instance.Type)
		{
		case CLIP_EVENT_TYPE::NOTIFY:
			if (instance.Tag == "EvadeSign") 
			{
				LockOn(true);
				Active_AttackSign(false);
			}
			else if (instance.Tag == "Fire")
				Summon_Bullet();
			break;

		case CLIP_EVENT_TYPE::SOUND:
			Get_Component<CAudioSource>()->Slot(instance.Tag).Attribute3D(true).Volume(0.2f).Play();
			break;
		}
	}
}

_float3 CMiasmaGrandierJaeger::Get_FirePos()
{
	Matrix Bone = Get_Component<CAnimator3D>()->Get_BoneMatrix(CAnimator3D::BoneSpace::COMBINED, "Bn_Weapon1");
	Matrix World = m_pTransform->Get_WorldMatrix();

	_vector3  T, S;
	_quaternion R;
	(Bone * World).Decompose(S, R, T);

	return T;
}

void CMiasmaGrandierJaeger::Summon_Bullet()
{
	string lvKey = LevelManager()->Get_NowLevelKey();
	_float3 pos = Get_FirePos();
	_float3 target = m_tTargetingInfo.vTargetPos;
	target.y = pos.y;
	auto desc = new CMiasmaProjectile::MiasmaProjectileDesc(target);

	COLLIDER_DESC ColDesc = {};
	ColDesc.eGroup = COLLISION_GROUP::MONSTER;
	ColDesc.iCollisionMask = ENUM(COLLISION_GROUP::PLAYER_ATTACK) | ENUM(COLLISION_GROUP::PLAYER) | ENUM(COLLISION_GROUP::COMMON);
	ColDesc.bTrigger = true;
	ColDesc.bAutoFit = false;
	ColDesc.eType = COLLIDER_TYPE::BOX;
	ColDesc.vSize = { 1.f, 2.f, 2.f };

	auto Missile = Builder::Create_Object({ "Zero_Level", "Proto_GameObject_MiasmaProjectile" })
		.Collider(ColDesc).Add_ObjDesc(desc).Position(pos).FromPool().Build("Missile");
	ObjectManager()->Add_Object(Missile, { lvKey ,"Enemy_Layer" });
}

void CMiasmaGrandierJaeger::TakeDamage(DAMAGE_TYPE eDamageType, _float fDamage, CHARACTER charaName)
{
	BattleSystem()->HitVFX(eDamageType);
	m_HitCount++;
	if (m_HitCount > 3.f) {
		m_pStateMachine->Set_Trigger("Attack_To_DisAppear");
	}
	else {
		Get_Component<CAnimator3D>()
			->Set_Animation(1, "GrenadierJaeger_Ani_Hit_Stay ")
			.LayerBlend(1.f, 0.f, 1.f, EaseType::InOutQuint)
			.Loop(false)
			.Apply();
	}

	DAMAGE_DESC desc = {};
	_int damage = Helper::Get_Random_Int(1000, 10000); // юс╫ц
	desc.damage = damage;
	desc.followHandle = Get_Handle();
	desc.followOffset = Vector3(0.f, 1.3f, 0.f);
	desc.isEnemy = true;
	desc.charaName = charaName;

	UIDirector()->Request_DamageText(desc);
}

HRESULT CMiasmaGrandierJaeger::Initialize_StateMachine()
{
	m_pStateMachine = CStateMachine<CMiasmaGrandierJaeger>::Create();
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

HRESULT CMiasmaGrandierJaeger::Initialize_States()
{
	m_pStateMachine->Register_State("Appear",		CMiasmaGrandierJaeger_Appear::Create());
	m_pStateMachine->Register_State("Attack",		CMiasmaGrandierJaeger_Attack::Create());
	m_pStateMachine->Register_State("DisAppear",	CMiasmaGrandierJaeger_DisAppear::Create());
	m_pStateMachine->Register_State("Hit",			CMiasmaGrandierJaeger_Hit::Create());

	return S_OK;
}

HRESULT CMiasmaGrandierJaeger::Initialize_Transitions()
{
	m_pStateMachine->Register_Transition("Appear", "Attack",
		CStateMachine<CMiasmaGrandierJaeger>::CONDITION_TRIGGER, "Appear_To_Attack");
	m_pStateMachine->Register_Transition("Attack", "DisAppear",
		CStateMachine<CMiasmaGrandierJaeger>::CONDITION_TRIGGER, "Attack_To_DisAppear");

	m_pStateMachine->Register_AnyStateTransition("Hit",
		CStateMachine<CMiasmaGrandierJaeger>::CONDITION_TRIGGER, "Hit");

	return S_OK;
}

HRESULT CMiasmaGrandierJaeger::Initialize_Effects()
{
	auto pObjectContainer = Get_Component<CObjectContainer>();
	Create_AttackSign("Bip001_Head");
	return S_OK;
}
