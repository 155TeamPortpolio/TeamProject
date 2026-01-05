#include "pch.h"
#include "TrailNode.h"
#include "TrailNode_Edit.h"
#include "TrailModel.h"
#include "Material.h"

CTrailNode_Edit::CTrailNode_Edit()
	:CTrailNode()
{
}
CTrailNode_Edit::CTrailNode_Edit(const CTrailNode_Edit& rhs)
	:CTrailNode(rhs)
{
}
HRESULT CTrailNode_Edit::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CTrailModel>();
	Add_Component<CMaterial>();
	return S_OK;
}
HRESULT CTrailNode_Edit::Initialize(INIT_DESC* pArg)
{
	TRAIL_NODE_EDIT_DESC* pDesc = static_cast<TRAIL_NODE_EDIT_DESC*>(pArg);
	m_pContext = pDesc->pContext;

	auto pModel = Get_Component<CTrailModel>();
	pModel->Set_RenderType(RENDER_PASS_TYPE::RENDER_EFFECT);

	return S_OK;
}
void CTrailNode_Edit::Awake()
{
}
void CTrailNode_Edit::Priority_Update(_float dt)
{
}
void CTrailNode_Edit::Update(_float dt)
{
}
void CTrailNode_Edit::Late_Update(_float dt)
{
}
void CTrailNode_Edit::Render_GUI()
{
}
void CTrailNode_Edit::Play()
{
}
void CTrailNode_Edit::Import(nlohmann::ordered_json& json)
{
}
void CTrailNode_Edit::Export(nlohmann::ordered_json& json)
{
}
CTrailNode_Edit* CTrailNode_Edit::Create()
{
	CTrailNode_Edit* instance = new CTrailNode_Edit();

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Clone Failed : CTrailNode_Edit");
		Safe_Release(instance);
	}

	return instance;
}
CGameObject* CTrailNode_Edit::Clone(INIT_DESC* pArg)
{
	CTrailNode_Edit* instance = new CTrailNode_Edit(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CTrailNode_Edit");
		Safe_Release(instance);
	}

	return instance;
}
void CTrailNode_Edit::Free()
{
	__super::Free();
}