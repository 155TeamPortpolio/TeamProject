#include "Engine_Defines.h"
#include "EffectContainer.h"

#include "GameInstance.h"
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

	EFFECT_ASSET* pAsset = static_cast<EFFECT_ASSET*>(pArg);
	m_fDuration = pAsset->fDuration;
	m_IsLoop = pAsset->isLoop;
	m_iNumNodes = pAsset->Nodes.size();
	m_Nodes.resize(m_iNumNodes);

	auto proto = CGameInstance::GetInstance()->Get_PrototypeMgr();
	for (_uint i = 0; i < m_iNumNodes; ++i)
	{
		CGameObject* pNode = nullptr;
		EFFECT_NODE nodeDesc = pAsset->Nodes[i];
		switch (pAsset->Nodes[i].eType)
		{
		case Engine::EFFECT_TYPE::SPRITE:
			pNode = proto->Clone_Prototype(G_GlobalLevelKey, "Proto_GameObject_SpriteNode", &nodeDesc);
			break;
		case Engine::EFFECT_TYPE::PARTICLE:
			break;
		case Engine::EFFECT_TYPE::MESH:
			break;
		case Engine::EFFECT_TYPE::END:
			break;
		default:
			break;
		}

		if (pNode)
			m_Nodes[i] = static_cast<CEffectNode*>(pNode);
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
	if (!m_IsLoop)
	{
		m_fElapsedTime += dt;
	}

	Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CEffectContainer::Late_Update(_float dt)
{
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

	for (auto& node : m_Nodes)
		Safe_Release(node);
	m_Nodes.clear();
}
