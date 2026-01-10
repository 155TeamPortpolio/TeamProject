#include "Engine_Defines.h"
#include "EffectContainer.h"

#include "GameInstance.h"
#include "IResourceService.h"
#include "IProtoService.h"
#include "EffectNode.h"
#include "ObjectContainer.h"

CEffectContainer::CEffectContainer()
	:CGameObject()
{
}

CEffectContainer::CEffectContainer(const CEffectContainer& rhs)
	:CGameObject(rhs)
{
}

HRESULT CEffectContainer::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CObjectContainer>();
	return S_OK;
}

HRESULT CEffectContainer::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	auto pResource = CGameInstance::GetInstance()->Get_ResourceMgr();
	EFFECT_DESC* pDesc = static_cast<EFFECT_DESC*>(pArg);

	EFFECT_ASSET pAsset = pResource->Load_EffectAsset(G_GlobalLevelKey, pDesc->EffectAssetKey);
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

void CEffectContainer::Awake()
{
}

void CEffectContainer::Priority_Update(_float dt)
{
}

void CEffectContainer::Update(_float dt)
{
	if (m_IsLoop)
		Get_Component<CObjectContainer>()->UpdateChild(dt);
	else
	{
		m_fElapsedTime += dt;
		if (m_fElapsedTime >= m_fDuration)
		{
			m_isAlive = false;
			return;
		}

		Get_Component<CObjectContainer>()->UpdateChild(dt);
	}

}

void CEffectContainer::Late_Update(_float dt)
{
}

CEffectContainer::EFFECT_CONTAINER_CONTEXT& CEffectContainer::GetEffectContext()
{
	return m_EffectContext;
}

void CEffectContainer::SetLinePoints(_float3 point0, _float3 point1)
{
	m_EffectContext.vLinePoint0 = point0;
	m_EffectContext.vLinePoint1 = point1;
}

void CEffectContainer::Play()
{
	for (const auto& node : m_Nodes)
		static_cast<CEffectNode*>(node)->Play();
}

void CEffectContainer::Stop()
{
	for (const auto& node : m_Nodes)
		static_cast<CEffectNode*>(node)->Stop();
}

CEffectContainer* CEffectContainer::Create()
{
	CEffectContainer* instance = new CEffectContainer();

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CEffectContainer");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CEffectContainer::Clone(INIT_DESC* pArg)
{
	CEffectContainer* instance = new CEffectContainer(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CEffectContainer");
		Safe_Release(instance);
	}

	return instance;
}

void CEffectContainer::Free()
{
	__super::Free();

	m_Nodes.clear();
}
