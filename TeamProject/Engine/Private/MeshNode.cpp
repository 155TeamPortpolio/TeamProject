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

	MESH_NODE* pMeshNode = static_cast<MESH_NODE*>(pArg);

	CMaterial* pMaterial = Get_Component<CMaterial>();
	if (FAILED(pMaterial->Link_Material(G_GlobalLevelKey, pMeshNode->MaterialTag)))
	{
		MSG_BOX("Material Link Failed : CMeshNode");
		return E_FAIL;
	}
	
	auto pMaterialInstance = pMaterial->Get_MaterialInstance(0);
	pMaterialInstance->Override_Pass("UVAnimation");

	CStaticModel* pModel = Get_Component<CStaticModel>();
	pModel->Link_Model(G_GlobalLevelKey, pMeshNode->ModelTag);
	pModel->Set_RenderType(RENDER_PASS_TYPE::RENDER_EFFECT);

	/* Set Param */
	{
		/* Texture */
		m_TextureSlotModule.eSamplerMode = static_cast<TEXTURE_SLOT_MODULE::SAMPLER_MODE>(pMeshNode->SamplerMode);
		m_TextureSlotModule.eMainUsage = static_cast<TEXTURE_SLOT_MODULE::MAIN_USAGE>(pMeshNode->MainUsage);
		m_TextureSlotModule.eRed = static_cast<TEXTURE_SLOT_MODULE::CHANNEL_USAGE>(pMeshNode->Red);
		m_TextureSlotModule.eGreen = static_cast<TEXTURE_SLOT_MODULE::CHANNEL_USAGE>(pMeshNode->Green);
		m_TextureSlotModule.eBlue = static_cast<TEXTURE_SLOT_MODULE::CHANNEL_USAGE>(pMeshNode->Blue);
		m_TextureSlotModule.eAlpha = static_cast<TEXTURE_SLOT_MODULE::CHANNEL_USAGE>(pMeshNode->Alpha);

		/* Color */
		m_ColorModule.eEaseType = static_cast<EaseType>(pMeshNode->ColorEaseType);
		m_ColorModule.vStartColor = pMeshNode->vStartColor;
		m_ColorModule.vEndColor = pMeshNode->vEndColor;

		/* Scale */
		m_ScaleModule.eEaseType = static_cast<EaseType>(pMeshNode->ScaleEaseType);
		m_ScaleModule.vStartScale = pMeshNode->vStartScale;
		m_ScaleModule.vEndScale = pMeshNode->vEndScale;

		/* UV Anim */
		m_UVAnimaitonModule.eEaseType = static_cast<EaseType>(pMeshNode->UVEaseType);
		m_UVAnimaitonModule.vStartUVOffset = pMeshNode->vStartUVOffset;
		m_UVAnimaitonModule.vEndUVOffset = pMeshNode->vEndUVOffset;

		/* Sprite Anim */
		m_SpriteAnimationModule.iCol = pMeshNode->iCol;
		m_SpriteAnimationModule.iRow = pMeshNode->iRow;
		m_SpriteAnimationModule.iMaxFrameIndex = pMeshNode->iMaxFrameIndex;

		/* Dissolve */
		m_DissolveModule.eEaseType = static_cast<EaseType>(pMeshNode->DissolveEase);
		m_DissolveModule.fStartProgress = pMeshNode->fDissolveStartProgress;
		m_DissolveModule.fEndProgress = pMeshNode->fDissolveEndProgress;

		/* Bloom */
		m_BloomModule.fIntensity = pMeshNode->fBloomIntensity;
	}

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

	if (m_IsEffectActive)
	{
		m_fProgress = m_fElpasedTime / m_fDuration;

		Update_TextureSlotModule(dt);
		Update_ColorModule(dt);
		Update_ScaleModule(dt);
		Update_UVAnimationModule(dt);
		Update_SpriteAnimationModule(dt);
		Update_DissolveModule(dt);
		Update_BloomModule(dt);
		Bind_Params();
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

void CMeshNode::Update_TextureSlotModule(_float dt)
{
	m_TextureSlotModule.iSamplerModeParam = ENUM(m_TextureSlotModule.eSamplerMode);
	m_TextureSlotModule.iMainUsageParam = ENUM(m_TextureSlotModule.eMainUsage);
	m_TextureSlotModule.vChannelUsageParam.x = ENUM(m_TextureSlotModule.eRed);
	m_TextureSlotModule.vChannelUsageParam.y = ENUM(m_TextureSlotModule.eGreen);
	m_TextureSlotModule.vChannelUsageParam.z = ENUM(m_TextureSlotModule.eBlue);
	m_TextureSlotModule.vChannelUsageParam.w = ENUM(m_TextureSlotModule.eAlpha);
}

void CMeshNode::Update_ColorModule(_float dt)
{
	_float t = m_fElpasedTime / m_fDuration;

	m_ColorModule.vCurrColor = _vector4::Lerp(m_ColorModule.vStartColor, m_ColorModule.vEndColor, Math::ApplyEase(m_ColorModule.eEaseType, t));
}

void CMeshNode::Update_ScaleModule(_float dt)
{
	_float t = m_fElpasedTime / m_fDuration;

	m_ScaleModule.vCurrScale = _vector3::Lerp(m_ScaleModule.vStartScale, m_ScaleModule.vEndScale, Math::ApplyEase(m_ScaleModule.eEaseType, t));
	m_pTransform->Scale(m_ScaleModule.vCurrScale);
}

void CMeshNode::Update_UVAnimationModule(_float dt)
{
	_float t = m_fElpasedTime / m_fDuration;

	m_UVAnimaitonModule.vCurrUVOffset = _vector2::Lerp(m_UVAnimaitonModule.vStartUVOffset, m_UVAnimaitonModule.vEndUVOffset, Math::ApplyEase(m_UVAnimaitonModule.eEaseType, t));
}

void CMeshNode::Update_SpriteAnimationModule(_float dt)
{
	_float t = m_fElpasedTime / m_fDuration;

	m_SpriteAnimationModule.iCurrFrameIndex = static_cast<_uint>(t * m_SpriteAnimationModule.iMaxFrameIndex);
}

void CMeshNode::Update_DissolveModule(_float dt)
{
	_float t = m_fElpasedTime / m_fDuration;

	if (t >= m_DissolveModule.fStartProgress && t <= m_DissolveModule.fEndProgress)
		m_DissolveModule.fProgress = (t - m_DissolveModule.fStartProgress) / (m_DissolveModule.fEndProgress - m_DissolveModule.fStartProgress);

	if (t >= m_DissolveModule.fEndProgress)
		m_DissolveModule.fProgress = 1.f;
}

void CMeshNode::Update_BloomModule(_float dt)
{

}

void CMeshNode::Bind_Params()
{
	auto pMaterialInstance = Get_Component<CMaterial>()->Get_MaterialInstance(0);

	pMaterialInstance->Set_Param("Progress", { &m_fProgress,"float",sizeof(_float) });

	/* Texture */
	pMaterialInstance->Set_Param("SamplerMode", { &m_TextureSlotModule.iSamplerModeParam,"uint",sizeof(_uint)});
	pMaterialInstance->Set_Param("MainUsage", { &m_TextureSlotModule.iMainUsageParam,"uint",sizeof(_uint)});
	pMaterialInstance->Set_Param("ChannelUsage", { &m_TextureSlotModule.vChannelUsageParam,"uint4",sizeof(_uint4)});

	/* Color */
	pMaterialInstance->Set_Param("vBaseColor", { &m_ColorModule.vCurrColor,"float4",sizeof(_float4) });

	/* UV Anim */
	pMaterialInstance->Set_Param("UVOffset", { &m_UVAnimaitonModule.vCurrUVOffset,"float2",sizeof(_float2) });

	/* Sprite Anim */
	pMaterialInstance->Set_Param("Col", { &m_SpriteAnimationModule.iCol,"uint",sizeof(_uint) });
	pMaterialInstance->Set_Param("Row", { &m_SpriteAnimationModule.iRow,"uint",sizeof(_uint) });
	pMaterialInstance->Set_Param("FrameIndex", { &m_SpriteAnimationModule.iCurrFrameIndex,"uint",sizeof(_uint) });

	/* Dissolve */
	pMaterialInstance->Set_Param("DissolveProgress", { &m_DissolveModule.fProgress,"float",sizeof(_float) });

	/* Bloom */
	pMaterialInstance->Set_Param("BloomThreshold", { &m_BloomModule.fThreshold,"float",sizeof(_float) });
	pMaterialInstance->Set_Param("BloomSoftness", { &m_BloomModule.fSoftness,"float",sizeof(_float) });
	pMaterialInstance->Set_Param("BloomIntensity", { &m_BloomModule.fIntensity,"float",sizeof(_float) });

}
