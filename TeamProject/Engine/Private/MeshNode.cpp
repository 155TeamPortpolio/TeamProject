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
		m_vBaseColor = pMeshNode->vBaseColor;

		m_eAlphaFadeEase = static_cast<EaseType>(pMeshNode->AlphaFadeEase);
		m_vAlphaFade = pMeshNode->vAlphaFade;

		m_eScaleEase = static_cast<EaseType>(pMeshNode->ScaleEase);
		m_vStartScale = pMeshNode->vStartScale;
		m_vEndScale = pMeshNode->vEndScale;

		m_eUVEase = static_cast<EaseType>(pMeshNode->UVEase);
		m_vStartUVOffset = pMeshNode->vStartUVOffset;
		m_vEndUVOffset = pMeshNode->vEndUVOffset;

		m_iCol = pMeshNode->iCol;
		m_iRow = pMeshNode->iRow;
		m_iMaxFrameIndex = pMeshNode->iMaxFrameIndex;

		m_eDissolveEase = static_cast<EaseType>(pMeshNode->DissolveEase);
		m_fDissolveStartProgress = pMeshNode->fDissolveStartProgress;
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
		//_float t = m_fElpasedTime / m_fDuration;
		//
		//m_fThreshold = t;
		//m_fAlpha = Math::Lerp(m_vAlphaFade.x, m_vAlphaFade.y, Math::ApplyEase(m_eAlphaFadeEase, t));
		//_float3 vCurrScale = _vector3::Lerp(m_vStartScale, m_vEndScale, Math::ApplyEase(m_eScaleEase, t));
		//m_pTransform->Scale(vCurrScale);
		//
		//auto pMaterialInstance = Get_Component<CMaterial>()->Get_MaterialInstance(0);
		//switch (m_eMode)
		//{
		//case Engine::CMeshNode::MODE::UV_ANIMATION:
		//{
		//	m_vCurrUVOffset = _vector2::Lerp(m_vStartUVOffset, m_vEndUVOffset, Math::ApplyEase(m_eUVEase, t));
		//
		//	pMaterialInstance->Set_Param("UVOffset", { &m_vCurrUVOffset,"float2",sizeof(_float2) });
		//}break;
		//case Engine::CMeshNode::MODE::SPRITE_ANIAMTION:
		//{
		//	m_iCurrFrameIndex = static_cast<_uint>(m_iMaxFrameIndex * t);
		//
		//	auto pMaterialInstance = Get_Component<CMaterial>()->Get_MaterialInstance(0);
		//	pMaterialInstance->Set_Param("FrameIndex", { &m_iCurrFrameIndex,"uint",sizeof(_uint) });
		//}break;
		//default:
		//	break;
		//}
		//
		///*Dissolve*/
		//if (t >= m_fDissolveStartProgress)
		//{
		//	m_fDissolveThreshold = (t - m_fDissolveStartProgress) / (1.f - m_fDissolveStartProgress);
		//	m_fDissolveThreshold = Math::ApplyEase(m_eDissolveEase, m_fDissolveThreshold);
		//}
		//
		//if (m_fElpasedTime >= m_fDuration)
		//{
		//	m_fDissolveThreshold = 1.f;
		//	m_fThreshold = 1.f;
		//}
		//
		//pMaterialInstance->Set_Param("DissolveThreshold", { &m_fDissolveThreshold,"float",sizeof(_float) });
		//pMaterialInstance->Set_Param("Threshold", { &m_fThreshold,"float",sizeof(_float) });
		//pMaterialInstance->Set_Param("Alpha", { &m_fAlpha,"float",sizeof(_float) });
		//pMaterialInstance->Set_Param("BloomIntensity", { &m_fBloomIntensity,"float",sizeof(_float) });

		Update_ColorModule(dt);
		Update_ScaleModule(dt);
		Update_UVAnimationModule(dt);
		Update_SpriteAnimationModule(dt);
		Update_DissolveModule(dt);
		Update_BloomModule(dt);
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

	/* Texture */
	pMaterialInstance->Set_Param("ChannelUsage", { &m_TextureSlotModule.vChannelUsage,"float4",sizeof(_float4) });

	/* Color */
	pMaterialInstance->Set_Param("vBaseColor", { &m_ColorModule.vCurrColor,"float4",sizeof(_float4) });

	/* UV Anim */
	pMaterialInstance->Set_Param("UVOffset", { &m_UVAnimaitonModule.vCurrUVOffset,"float2",sizeof(_float2) });

	/* Sprite Anim */
	pMaterialInstance->Set_Param("Col", { &m_SpriteAnimationModule.iCol,"uint",sizeof(_uint) });
	pMaterialInstance->Set_Param("Row", { &m_SpriteAnimationModule.iRow,"uint",sizeof(_uint) });
	pMaterialInstance->Set_Param("FrameIndex", { &m_SpriteAnimationModule.iCurrFrameIndex,"uint",sizeof(_uint) });

	/* Bloom */
	pMaterialInstance->Set_Param("BloomIntensity", { &m_BloomModule.fIntensity,"float",sizeof(_float) });

	/* Dissolve */
	pMaterialInstance->Set_Param("DissolveProgress", { &m_DissolveModule.fProgress,"float",sizeof(_float) });
}
