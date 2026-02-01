#include "pch.h"
#include "GachaStageScreen.h"

#include "StaticModel.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "Child.h"

#include "GameInstance.h"
#include "Shader.h"
#include "Texture.h"

CGachaStageScreen::CGachaStageScreen()
    :CGameObject()
{
}

CGachaStageScreen::CGachaStageScreen(const CGachaStageScreen& rhs)
    :CGameObject(rhs)
{
}

HRESULT CGachaStageScreen::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    auto pModel = Add_Component<CStaticModel>();
    auto pMaterial = Add_Component<CMaterial>();

    //pModel->Link_Model("Gacha_Level", "BangBooScreen2.model");
    //pMaterial->Link_Material("Gacha_Level", "BangBooScreen2.mat");

    pModel->Link_Model("Gacha_Level", "Screen1.model");
    pMaterial->Link_Material("Gacha_Level", "Screen1.mat");

    return S_OK;
}

HRESULT CGachaStageScreen::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CGachaStageScreen::Awake()
{
	auto pMaterial = Get_Component<CMaterial>();

	CTexture* pTexture = ResourceManager()->Load_Texture(G_GlobalLevelKey, "Gacha_Bangboo_04.png");
	if (pTexture == nullptr) return;

	auto pMaterialInstances = pMaterial->Get_MaterialInstances();
	for (auto& instance : pMaterialInstances)
	{
		instance->Set_Param("DiffuseTexture", { pTexture->Get_SRV(), "Texture2D", 0 });
		instance->Set_Param("FrameIndex", { &m_iCurrentFrameIndex, "int", sizeof(_int) });
		instance->Set_Param("Col", { &m_iCol, "int", sizeof(_int) });
		instance->Set_Param("Row", { &m_iRow, "int", sizeof(_int) });
	}
}

void CGachaStageScreen::Priority_Update(_float dt)
{
}

void CGachaStageScreen::Update(_float dt)
{
	m_fElapsedTime += dt;
	if (m_fElapsedTime > m_fFrameDuration)
	{
		++m_iCurrentFrameIndex;
		m_fElapsedTime = 0.f;
		if (m_iCurrentFrameIndex >= m_iMaxFrameIndex)
			m_iCurrentFrameIndex = 0;
	}
}

void CGachaStageScreen::Late_Update(_float dt)
{
}

CGachaStageScreen* CGachaStageScreen::Create()
{
	CGachaStageScreen* Instance = new CGachaStageScreen();
	if (FAILED(Instance->Initialize_Prototype()))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

CGameObject* CGachaStageScreen::Clone(INIT_DESC* pArg)
{
	CGachaStageScreen* Instance = new CGachaStageScreen(*this);
	if (FAILED(Instance->Initialize(pArg)))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

void CGachaStageScreen::Free()
{
	__super::Free();
}
