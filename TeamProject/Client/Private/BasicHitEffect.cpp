#include "pch.h"
#include "BasicHitEffect.h"
#include "GameInstance.h"

// Component
#include "ObjectContainer.h"
#include "EffectNode.h"

CBasicHitEffect::CBasicHitEffect()
	:CEffectContainer()
{
}

CBasicHitEffect::CBasicHitEffect(const CBasicHitEffect& rhg)
	:CEffectContainer(rhg)
{
}

HRESULT CBasicHitEffect::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CBasicHitEffect::Initialize(INIT_DESC* arg)
{
	CGameObject::Initialize(arg);

	EFFECT_ASSET pAsset = ResourceManager()->Load_EffectAsset(G_GlobalLevelKey, "basic_hit.json");
	m_IsBillBoard = pAsset.isBillboard;
	m_fDuration = pAsset.fDuration;
	m_IsLoop = pAsset.isLoop;
	m_iNumNodes = pAsset.Nodes.size();
	m_Nodes.resize(m_iNumNodes);

	auto proto = CGameInstance::GetInstance()->Get_PrototypeMgr();
	for (_uint i = 0; i < m_iNumNodes; ++i)
	{
		CGameObject* pNode = nullptr;
		EFFECT_NODE* pNodeDesc = pAsset.Nodes[i];
		switch (static_cast<EFFECT_TYPE>(pNodeDesc->eType))
		{
		case Engine::EFFECT_TYPE::SPRITE:
			pNode = proto->Clone_Prototype(G_GlobalLevelKey, "Proto_GameObject_SpriteNode", pNodeDesc);
			break;
		case Engine::EFFECT_TYPE::PARTICLE:
			pNode = proto->Clone_Prototype(G_GlobalLevelKey, "Proto_GameObject_ParticleNode", pNodeDesc);
			break;
		case Engine::EFFECT_TYPE::MESH:
			pNode = proto->Clone_Prototype(G_GlobalLevelKey, "Proto_GameObject_MeshNode", pNodeDesc);
			break;
		case Engine::EFFECT_TYPE::TRAIL:
			pNode = proto->Clone_Prototype(G_GlobalLevelKey, "Proto_GameObject_TrailNode", pNodeDesc);
			break;
		case Engine::EFFECT_TYPE::END:
			break;
		default:
			break;
		}

		if (pNode)
		{
			m_Nodes[i] = static_cast<CEffectNode*>(pNode);
			Get_Component<CObjectContainer>()->Add_Child(pNode);
		}
	}

	return S_OK;
}

void CBasicHitEffect::Awake()
{
}

void CBasicHitEffect::Priority_Update(_float dt)
{
	__super::Priority_Update(dt);
}

void CBasicHitEffect::Update(_float dt)
{
	__super::Update(dt);
}

void CBasicHitEffect::Late_Update(_float dt)
{
	__super::Late_Update(dt);
}

void CBasicHitEffect::OnPooledAcquire(INIT_DESC* pArg)
{
	__super::OnPooledAcquire(pArg);
}

void CBasicHitEffect::OnPooledRelease()
{
	__super::OnPooledRelease();
}

CBasicHitEffect* CBasicHitEffect::Create()
{
	auto instance = new CBasicHitEffect();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CBasicHitEffect");
		Safe_Release(instance);
	}
	return instance;
}

CGameObject* CBasicHitEffect::Clone(INIT_DESC* pArg)
{
	auto instance = new CBasicHitEffect(*this);
	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CBasicHitEffect");
		Safe_Release(instance);
	}
	return instance;
}

void CBasicHitEffect::Free()
{
	__super::Free();
}
