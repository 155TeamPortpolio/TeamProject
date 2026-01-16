#include "Engine_Defines.h"
#include "ParticleNode.h"
#include "GameInstance.h"
#include "ParticleSystem.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "MaterialData.h"

CParticleNode::CParticleNode()
	:CEffectNode()
{
}

CParticleNode::CParticleNode(const CParticleNode& rhs)
	:CEffectNode(rhs)
{
}

HRESULT CParticleNode::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CParticleSystem>();
	Add_Component<CMaterial>();
	return S_OK;
}

HRESULT CParticleNode::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	PARTICLE_NODE* pParticleNode = static_cast<PARTICLE_NODE*>(pArg);

	CParticleSystem* pParticle = Get_Component<CParticleSystem>();
	pParticle->Initialize(nullptr);
	pParticle->Link_Model(G_GlobalLevelKey, "Engine_Default_InstancePoint");
	pParticle->ShadowCast(false);

	ID3D11Device* pDevice = CGameInstance::GetInstance()->Get_Device();
	CMaterial* pMaterial = Get_Component<CMaterial>();
	CMaterialInstance* customInstance = CMaterialInstance::Create_Handle("Point_Effect_Base", "Default", pDevice);
	customInstance->ChangeTexture(TEXTURE_TYPE::DIFFUSE, 0);
	customInstance->Set_Blended(true);

	pMaterial->Insert_MaterialInstance(customInstance, nullptr);

	auto MaterialDat = customInstance->Get_MaterialData();
	if (MaterialDat)
	{
		MaterialDat->Link_Shader(G_GlobalLevelKey, "VTX_InstancePoint.hlsl");
		MaterialDat->Link_Texture(G_GlobalLevelKey, pParticleNode->TextureKey, TEXTURE_TYPE::DIFFUSE);
	}

	m_InstanceName = "ParticleNode";

	pParticle->SetParticleParams(*pParticleNode);
	if (m_IsLoop)
		m_IsEffectActive = true;

	_vector3 vPosition = pParticleNode->vOffsetPosition;
	_quaternion vQuaternion = pParticleNode->vOffsetQuaternion;

	m_pTransform->Set_Pos(vPosition);
	m_pTransform->Set_Quaternion(vQuaternion);

	return S_OK;
}

void CParticleNode::Awake()
{
}

void CParticleNode::Priority_Update(_float dt)
{
}

void CParticleNode::Update(_float dt)
{
	__super::Update(dt);

	if (m_IsEffectActive)
	{
		auto particle = Get_Component<CParticleSystem>();
		particle->Simulation_Particle(dt);
	}
}

void CParticleNode::Late_Update(_float dt)
{
}

void CParticleNode::Play()
{
	m_IsEffectActive = true;
	m_fElpasedTime = 0.f;

	Get_Component<CParticleSystem>()->Reset();
}

void CParticleNode::Stop()
{
	m_IsEffectActive = false;
}

CParticleNode* CParticleNode::Create()
{
	CParticleNode* instance = new CParticleNode();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CSpriteNode");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CParticleNode::Clone(INIT_DESC* pArg)
{
	CParticleNode* instance = new CParticleNode(*this);
	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Create Failed : CSpriteNode");
		Safe_Release(instance);
	}

	return instance;
}

void CParticleNode::Free()
{
	__super::Free();
}
