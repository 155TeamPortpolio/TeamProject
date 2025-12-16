#include "Engine_Defines.h"
#include "MeshNode.h"
#include "Helper_Func.h"
#include "StaticModel.h"
#include "MaterialInstance.h"
#include "Material.h"
#include "MaterialData.h"

CMeshNode::CMeshNode()
	:CEffectNode()
{
}

CMeshNode::CMeshNode(const CMeshNode& rhs)
	:CEffectNode(rhs)
{
}

HRESULT CMeshNode::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CStaticModel>();
	Add_Component<CMaterial>();
	return S_OK;
}

HRESULT CMeshNode::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	return S_OK;
}

void CMeshNode::Awake()
{
}

void CMeshNode::Priority_Update(_float dt)
{
}

void CMeshNode::Update(_float dt)
{
	__super::Update(dt);

	switch (m_eMode)
	{
	case Engine::CMeshNode::MODE::UV_ANIMATION:
	{
		m_fThreshold = m_fElpasedTime / m_fDuration;

		auto pMaterialInstance = Get_Component<CMaterial>()->Get_MaterialInstance(0);
		pMaterialInstance->Set_Param("Threshold", { &m_fThreshold,"float",sizeof(_float) });
	}break;
	case Engine::CMeshNode::MODE::SPRITE_ANIAMTION:
	{
		_float t = m_fElpasedTime / m_fDuration;
		m_iCurrFrameIndex = static_cast<_uint>(m_iMaxFrameIndex * t);

		auto pMaterialInstance = Get_Component<CMaterial>()->Get_MaterialInstance(0);
		pMaterialInstance->Set_Param("FrameIndex", { &m_iCurrFrameIndex,"uint",sizeof(_uint) });
		pMaterialInstance->Set_Param("Alpha", { &m_fAlpha,"float",sizeof(_float) });
	}break;
	default:
		break;
	}
}

void CMeshNode::Late_Update(_float dt)
{
}

CMeshNode* CMeshNode::Create()
{
	CMeshNode* instance = new CMeshNode();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CMeshNode");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CMeshNode::Clone(INIT_DESC* pArg)
{
	CMeshNode* instance = new CMeshNode(*this);
	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Create Failed : CMeshNode");
		Safe_Release(instance);
	}

	return instance;
}

void CMeshNode::Free()
{
	__super::Free();
}
