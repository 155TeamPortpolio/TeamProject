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

	Get_Component<CPointModel>()->Link_Model(G_GlobalLevelKey, "Engine_Default_Point");

	ID3D11Device* pDevice = CGameInstance::GetInstance()->Get_Device();
	CMaterial* pMaterial = Get_Component<CMaterial>();
	CMaterialInstance* customInstance = CMaterialInstance::Create_Handle("Point_Effect_Base", "", pDevice);
	pMaterial->Insert_MaterialInstance(customInstance, nullptr);

	//auto MaterialDat = customInstance->Get_MaterialData();
	//if(MaterialDat)
	//	MaterialDat->Link_Shader(G_GlobalLevelKey,)

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
