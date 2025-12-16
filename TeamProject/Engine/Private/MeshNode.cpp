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
	m_fElpasedTime += dt;

	if (!m_IsLoop)
	{
		if (m_fElpasedTime >= m_fDuration)
			return;

		auto pMaterialInstance = Get_Component<CMaterial>()->Get_MaterialInstance(0);

		_float t = m_fElpasedTime / m_fDuration;
		t = clamp(t, 0.f, 1.f);
		t = Math::EaseInCubic(t);

		m_fAlpha = Math::Lerp(m_vAlphaFade.x, m_vAlphaFade.y, t);

		pMaterialInstance->Set_Param("Alpha", { &m_fAlpha,"float",sizeof(_float) });
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
