 #include "pch.h"
#include "MiasmaSpawnBall.h"

#include "GameInstance.h"
#include "BattleSystem.h"

#include "StaticModel.h"
#include "Material.h"
#include "Collider.h"
#include "RigidBody.h"
#include "ObjectContainer.h"

#include "Helper_Func.h"
#include "EffectContainer.h"

CMiasmaSpawnBall::CMiasmaSpawnBall()
	: CGameObject()
{
}

CMiasmaSpawnBall::CMiasmaSpawnBall(const CMiasmaSpawnBall& rhg)
	: CGameObject(rhg)
{

}

HRESULT CMiasmaSpawnBall::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	//Add_Component<CStaticModel>()->Link_Model(G_GlobalLevelKey, "Default.model");
	//Add_Component<CMaterial>()->Link_Material(G_GlobalLevelKey, "Default.mat");
	Add_Component<CObjectContainer>();
	Add_Component<CCollider>();

	return S_OK;
}
HRESULT CMiasmaSpawnBall::Initialize(INIT_DESC* initDesc)
{
	__super::Initialize(initDesc);

	const SpawnParbolar* desc = static_cast<SpawnParbolar*>(initDesc);

	m_startPos = desc->startPos;
	m_targetPos = desc->targetPos;
	m_travelTime = (desc->travelTime > 1e-4f) ? desc->travelTime : 0.45f;
	m_arcHeight = desc->arcHeight;
	m_sideArc = desc->sideArc;
	m_destroyOnArrive = desc->destroyOnArrive;

	m_elapsed = 0.f;

	auto* transform = Get_Component<CTransform>();
	if (transform)
		transform->Set_Pos(m_startPos);

	Initialize_Effects();

	return S_OK;
}

void CMiasmaSpawnBall::Awake()
{
}

void CMiasmaSpawnBall::Priority_Update(_float dt)
{
}

void CMiasmaSpawnBall::Update(_float dt)
{

	m_elapsed += dt;

	const _float timeRatio = clamp(m_elapsed / m_travelTime, 0.f, 1.f);

	const _float curveT =  Math::SmoothStep01(timeRatio);

	const _vector3 start = m_startPos;
	const _vector3 target = m_targetPos;

	_vector3 position = start + (target - start) * curveT;

	const _float arc01 = sinf(timeRatio * XM_PI); 
	position.y += arc01 * m_arcHeight;

	if (fabsf(m_sideArc) > 1e-4f)
	{
		_vector3 toTarget = (target - start);
		toTarget.y = 0.f;
		if (toTarget.LengthSquared() > 1e-6f)
		{
			toTarget.Normalize();
			_vector3 right = _vector3(toTarget.z, 0.f, -toTarget.x); // 90µµ È¸Àü
			position += right * (arc01 * m_sideArc);
		}
	}

	/* Direction */
	_vector3 currPosition = m_pTransform->Get_WorldPos();
	_vector3 nextPosition = position;
	_vector3 dir = nextPosition - currPosition;

	if (dir.Length() < 0.01f)
		dir = _vector3(0.f, 0.f, 1.f);
	else
		dir.Normalize();

	m_pTransform->Set_Look(dir);
	m_pTransform->Set_Pos(position);
	Get_Component<CCollider>()->Update(dt);
	Get_Component<CObjectContainer>()->UpdateChild(dt);

	if (timeRatio >= 1.f && Get_Position().y <=-1.5f)
	{
		SpawnJaeger();

		/* Effect */
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("deifler_spawnball_explode.json")
			.Build("Defiler_SpawnBall_Explode");

		if (pEffect)
		{
			auto pEffectTransform = pEffect->Get_Component<CTransform>();
			pEffectTransform->Set_WorldPos(m_pTransform->Get_WorldPos());

			ObjectManager()->Add_Object(pEffect, { "Zero_Level","Effect_Layer" });
		}
	}
}
void CMiasmaSpawnBall::Late_Update(_float dt)
{

}

void CMiasmaSpawnBall::Render_GUI()
{
}

void CMiasmaSpawnBall::OnPooledAcquire(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	const SpawnParbolar* desc = static_cast<SpawnParbolar*>(pArg);

	m_startPos = desc->startPos;
	m_targetPos = desc->targetPos;
	m_travelTime = (desc->travelTime > 1e-4f) ? desc->travelTime : 0.45f;
	m_arcHeight = desc->arcHeight;
	m_sideArc = desc->sideArc;
	m_destroyOnArrive = desc->destroyOnArrive;

	m_elapsed = 0.f;

	auto* transform = Get_Component<CTransform>();
	if (transform)
		transform->Set_Pos(m_startPos);
}

void CMiasmaSpawnBall::OnPooledRelease()
{
}

void CMiasmaSpawnBall::OnTriggerEnter(CGameObject* pOther)
{
}

void CMiasmaSpawnBall::Initialize_Effects()
{
	auto pObjectContainer = Get_Component<CObjectContainer>();

	auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
		.Asset("defiler_spawnball.json")
		.Build("Defiler_SpawnBall");

	pObjectContainer->Add_Child(pEffect);
}

void CMiasmaSpawnBall::SpawnJaeger()
{
	const string levelKey = LevelManager()->Get_NowLevelKey();

	COLLIDER_DESC ColDesc = {};
	ColDesc.eGroup = COLLISION_GROUP::MONSTER;
	ColDesc.iCollisionMask = ENUM(COLLISION_GROUP::PLAYER_ATTACK) | ENUM(COLLISION_GROUP::PLAYER) ;
	ColDesc.bTrigger = false;
	ColDesc.bAutoFit = false;
	ColDesc.eType = COLLIDER_TYPE::BOX;
	ColDesc.vSize = { 5.f,3.5f, 3.f };
	auto jaeger = Builder::Create_Object({ "Zero_Level", "Proto_GameObject_MiasmaHeavy" })
		.Position(m_targetPos)
		.Collider(ColDesc)
		.Build("MiasmaUnit");
	ObjectManager()->Add_Object(jaeger, { levelKey, "Enemy_Layer" });
	BattleSystem()->EnterBattleObject(BATTLE_OBJ_TYPE::MONSTER, jaeger->Get_Handle());
	ObjectManager()->Remove_Object(this);
}

CMiasmaSpawnBall* CMiasmaSpawnBall::Create()
{
	CMiasmaSpawnBall* instance = new CMiasmaSpawnBall();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CMiasmaSpawnBall");
	}

	return instance;
}

CGameObject* CMiasmaSpawnBall::Clone(INIT_DESC * pArg)
{
	CMiasmaSpawnBall* instance = new CMiasmaSpawnBall(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CMiasmaProjectile");
	}

	return instance;
}

void CMiasmaSpawnBall::Free()
{
	__super::Free();
}
