#include "Engine_Defines.h"
#include "TrailNode.h"
#include "TrailModel.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "MaterialData.h"
#include "Child.h"
#include "EffectContainer.h"
#include "GameInstance.h"

CTrailNode::CTrailNode()
	:CEffectNode()
{
}

CTrailNode::CTrailNode(const CTrailNode& rhs)
	:CEffectNode(rhs)
{
}

HRESULT CTrailNode::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CTrailModel>();
	Add_Component<CMaterial>();
	return S_OK;
}

HRESULT CTrailNode::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	TRAIL_NODE* pTrailNode = static_cast<TRAIL_NODE*>(pArg);

	CTrailModel* pTrail = Get_Component<CTrailModel>();
	pTrail->Initialize(nullptr);
	pTrail->Set_RenderType(RENDER_PASS_TYPE::RENDER_EFFECT);
	pTrail->ShadowCast(false);

	ID3D11Device* pDevice = CGameInstance::GetInstance()->Get_Device();
	CMaterial* pMaterial = Get_Component<CMaterial>();
	CMaterialInstance* customInstance = CMaterialInstance::Create_Handle("Trail_Effect_Base", "Opaque", pDevice);
	customInstance->ChangeTexture(TEXTURE_TYPE::DIFFUSE, 0);
	//customInstance->Set_Blended(true);

	pMaterial->Insert_MaterialInstance(customInstance, nullptr);

	auto MaterialDat = customInstance->Get_MaterialData();
	if (MaterialDat)
	{
		MaterialDat->Link_Shader(G_GlobalLevelKey, "VTX_Trail.hlsl");
		MaterialDat->Link_Texture(G_GlobalLevelKey, pTrailNode->TextureKey, TEXTURE_TYPE::DIFFUSE);
	}

	m_InstanceName = "TrailNode";

	pTrail->SetTrailParams(*pTrailNode);
	m_eMode = static_cast<CTrailModel::POINT_MODE>(pTrailNode->iMode);

	return S_OK;
}

void CTrailNode::Awake()
{
}

void CTrailNode::Priority_Update(_float dt)
{
}

void CTrailNode::Update(_float dt)
{
	auto pTrail = Get_Component<CTrailModel>();

	auto pEffectContainer = Get_Component<CChild>()->Get_Parent();
	CEffectContainer::EFFECT_CONTAINER_CONTEXT& context = static_cast<CEffectContainer*>(pEffectContainer)->GetEffectContext();

	switch (m_eMode)
	{
	case Engine::CTrailModel::POINT_MODE::CENTER:
	{
		_vector3 vPosition = m_pTransform->Get_WorldPos();
		pTrail->Update_CenterPoint(context.vLinePoint0, dt);

	}break;
	case Engine::CTrailModel::POINT_MODE::SEGMENT:
	{
		_vector3 vPosition0 = m_pTransform->Get_WorldPos();
		_vector3 vPosition1 = vPosition0;
		vPosition1.y -= 5.f;

		pTrail->Update_SegmentPoint(context.vLinePoint0, context.vLinePoint1, dt);

	}break;
	case Engine::CTrailModel::POINT_MODE::LINE:
	{
		auto pCild = Get_Component<CChild>();
		auto pEffectContainer = static_cast<CEffectContainer*>(pCild->Get_Parent());
		
		if (!m_IsEffectActive)
			pTrail->SetFadeOut(true);

		pTrail->Add_LinePoint(context.vLinePoint0, context.vLinePoint1);
		pTrail->Update_LinePoint(dt);

	}break;
	default:
		break;
	}
}

void CTrailNode::Late_Update(_float dt)
{
}

void CTrailNode::Play()
{
	m_IsEffectActive = true;
	m_fElpasedTime = 0.f;

	if (CTrailModel::POINT_MODE::LINE == m_eMode)
	{
		Get_Component<CTrailModel>()->Reset();
		Get_Component<CTrailModel>()->SetFadeOut(false);
	}
}

void CTrailNode::Stop()
{
	m_IsEffectActive = false;

	if (CTrailModel::POINT_MODE::LINE == m_eMode)
		Get_Component<CTrailModel>()->SetFadeOut(true);
}

CTrailNode* CTrailNode::Create()
{
	CTrailNode* instance = new CTrailNode();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CTrailNode");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CTrailNode::Clone(INIT_DESC* pArg)
{
	CTrailNode* instance = new CTrailNode(*this);
	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Create Failed : CTrailNode");
		Safe_Release(instance);
	}

	return instance;
}

void CTrailNode::Free()
{
	__super::Free();
}
