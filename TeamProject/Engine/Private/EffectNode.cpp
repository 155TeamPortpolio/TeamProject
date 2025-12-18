#include "Engine_Defines.h"
#include "EffectNode.h"

CEffectNode::CEffectNode()
	:CGameObject()
{
}

CEffectNode::CEffectNode(const CEffectNode& rhs)
	:CGameObject(rhs)
{
}

HRESULT CEffectNode::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CEffectNode::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	/* Init Node */
	EFFECT_NODE* pNodeDesc = static_cast<EFFECT_NODE*>(pArg);
	m_fDuration = pNodeDesc->fDuration;
	m_fElpasedTime = 0.f;
	m_IsLoop = pNodeDesc->isLoop;

	return S_OK;
}

void CEffectNode::Update(_float dt)
{
	if (!m_IsLoop)
	{
		m_fElpasedTime += dt;
		if (m_fElpasedTime >= m_fDuration)
		{
			//m_isAlive = false;
		}
	}
}

void CEffectNode::Render_GUI()
{
	__super::Render_GUI();
}

void CEffectNode::Free()
{
	__super::Free();
}
