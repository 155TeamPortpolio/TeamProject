#include "Engine_Defines.h"
#include "SpriteNode.h"
#include "PointModel.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "MaterialData.h"

#include "GameInstance.h"
#include "IResourceService.h"

CSpriteNode::CSpriteNode()
	:CEffectNode()
{
}

CSpriteNode::CSpriteNode(const CSpriteNode& rhs)
	:CEffectNode(rhs)
{
}

HRESULT CSpriteNode::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CPointModel>();
	Add_Component<CMaterial>();
	return S_OK;
}

HRESULT CSpriteNode::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	/* Init Node */
	SPRITE_NODE* pNodeDesc = static_cast<SPRITE_NODE*>(pArg);
	m_IsAnimated = pNodeDesc->isAnimated;
	m_iMaxFrameIndex = pNodeDesc->iMaxFrameIndex;
	m_iCurrFrameIndex = 0;
	m_fSpeed = pNodeDesc->fSpeed;

	Get_Component<CPointModel>()->Link_Model(G_GlobalLevelKey, "Engine_Default_Point");

	ID3D11Device* pDevice = CGameInstance::GetInstance()->Get_Device();
	CMaterial* pMaterial = Get_Component<CMaterial>();
	CMaterialInstance* customInstance = CMaterialInstance::Create_Handle("Point_Effect_Base", "Opaque", pDevice);
	customInstance->ChangeTexture(TEXTURE_TYPE::DIFFUSE, 0);

	/* Shader Param */
	customInstance->Set_Param("Col", { &m_iCol,"uint",sizeof(_uint) });
	customInstance->Set_Param("Rol", { &m_iRow,"uint",sizeof(_uint) });
	customInstance->Set_Param("FrameIndex", { &m_iCurrFrameIndex,"uint",sizeof(_uint) });

	pMaterial->Insert_MaterialInstance(customInstance, nullptr);
	auto MaterialDat = customInstance->Get_MaterialData();
	if (MaterialDat)
	{
		MaterialDat->Link_Shader(G_GlobalLevelKey, "VTX_Point.hlsl");
		auto resource = CGameInstance::GetInstance()->Get_ResourceMgr();

		/* Load Diffuse Texture */
		for (_uint i = 0; i < m_iMaxFrameIndex; ++i)
		{
			string key = pNodeDesc->TextureKey + to_string(i) + ".png";
			string path = pNodeDesc->TexturePath + to_string(i) + ".png";

			resource->Add_ResourcePath(key, path);
			MaterialDat->Link_Texture(G_GlobalLevelKey, key, TEXTURE_TYPE::DIFFUSE);
		}
	}

	return S_OK;
}

void CSpriteNode::Awake()
{
}

void CSpriteNode::Priority_Update(_float dt)
{
}

void CSpriteNode::Update(_float dt)
{
	__super::Update(dt);

	if (m_IsAnimated)
	{
		m_fSpriteElapsedTime += m_fSpeed * dt;
		if (m_fSpriteElapsedTime >= 1.f)
		{
			++m_iCurrFrameIndex;
			if (m_iCurrFrameIndex >= m_iMaxFrameIndex)
			{
				if (m_IsRepeat)
					m_iCurrFrameIndex = 0;
				else
					--m_iCurrFrameIndex;
			}

			m_fSpriteElapsedTime -= 1.f;
		}

		//CMaterialInstance* pMaterialInstance = Get_Component<CMaterial>()->Get_MaterialInstance(0);
		//pMaterialInstance->ChangeTexture(TEXTURE_TYPE::DIFFUSE, m_iCurrFrameIndex);
	}
}

void CSpriteNode::Late_Update(_float dt)
{
}

CSpriteNode* CSpriteNode::Create()
{
	CSpriteNode* instance = new CSpriteNode();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CSpriteNode");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CSpriteNode::Clone(INIT_DESC* pArg)
{
	CSpriteNode* instance = new CSpriteNode(*this);
	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Create Failed : CSpriteNode");
		Safe_Release(instance);
	}

	return instance;
}

void CSpriteNode::Free()
{
	__super::Free();
}
