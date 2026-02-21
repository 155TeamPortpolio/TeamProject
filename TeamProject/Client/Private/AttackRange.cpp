#include "pch.h"
#include "AttackRange.h"
#include "EffectContainer.h"

//Component
#include "ObjectContainer.h"

CAttackRange::CAttackRange()
	:CGameObject()
{
}

CAttackRange::CAttackRange(const CAttackRange& rhg)
	:CGameObject(rhg)
{
}

HRESULT CAttackRange::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CAttackRange::Initialize(INIT_DESC* arg)
{
	__super::Initialize(arg);

	auto pObjectContainer = Get_Component<CObjectContainer>();

	auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
		.Asset("attack_range.json")
		.Build("Attack_Range");
	if (pEffect)
	{
		//pEffect->Set_Alive(false);
		//pEffect->Stop();
		pObjectContainer->Add_Child(pEffect);
	}

	return S_OK;
}

void CAttackRange::Awake()
{
}

void CAttackRange::Priority_Update(_float dt)
{
}

void CAttackRange::Update(_float dt)
{
	Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CAttackRange::Late_Update(_float dt)
{
}

void CAttackRange::Play_AttackRange(_float3 position, _float range)
{
	auto pEffect = Get_Component<CObjectContainer>()->Find_ObjectByName("Attack_Range");
	if (!pEffect)
		return;

	m_pTransform->Set_WorldPos(_vector3(position));
	m_pTransform->Scale(_float3(range, range, range));

	pEffect->Set_Alive(true);
	static_cast<CEffectContainer*>(pEffect)->Play();
}

CAttackRange* CAttackRange::Create()
{
	auto instance = new CAttackRange();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CAttackRange");
		Safe_Release(instance);
	}
	return instance;
}

CGameObject* CAttackRange::Clone(INIT_DESC* pArg)
{
	auto instance = new CAttackRange(*this);
	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CAttackRange");
		Safe_Release(instance);
	}
	return instance;
}

void CAttackRange::Free()
{
	__super::Free();
}
