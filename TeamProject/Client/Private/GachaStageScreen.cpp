#include "pch.h"
#include "GachaStageScreen.h"

#include "StaticModel.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "Child.h"

#include "GameInstance.h"
#include "Shader.h"
#include "Texture.h"
#include "DataBase.h"

#include "GachaStage.h"
#include "Helper_Func.h"

CGachaStageScreen::CGachaStageScreen()
    :CGameObject()
{
}

CGachaStageScreen::CGachaStageScreen(const CGachaStageScreen& rhs)
    :CGameObject(rhs)
{
}

void CGachaStageScreen::SetScreen(GACHA_STAGE eStage, GachaGrade eGrade)
{
	auto pModel = Add_Component<CStaticModel>();
	auto pMaterial = Add_Component<CMaterial>();

	if (eStage == GACHA_STAGE::BANGBOO)
	{
		pModel->Link_Model("Gacha_Level", "BangBooScreen2.model");
		pMaterial->Link_Material("Gacha_Level", "BangBooScreen2.mat");
		ResetMaterialInstances({
			Helper::Get_Random_Int(0,3),
			Helper::Get_Random_Int(0,3), 
			Helper::Get_Random_Int(0,2),
			Helper::Get_Random_Int(0,2),
			Helper::Get_Random_Int(0,2),
			Helper::Get_Random_Int(0,2),
			eGrade == GachaGrade::B ? 0 : 1,
			0
			});
	}
	else
	{
		pModel->Link_Model("Gacha_Level", "Screen1.model");
		pMaterial->Link_Material("Gacha_Level", "Screen1.mat");
		//ResetMaterialInstances({});
	}
}

HRESULT CGachaStageScreen::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    auto pModel = Add_Component<CStaticModel>();
    auto pMaterial = Add_Component<CMaterial>();

    pModel->Link_Model("Gacha_Level", "BangBooScreen2.model");
    pMaterial->Link_Material("Gacha_Level", "BangBooScreen2.mat");

    //pModel->Link_Model("Gacha_Level", "Screen1.model");
    //pMaterial->Link_Material("Gacha_Level", "Screen1.mat");

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
	auto pMaterialInstances = pMaterial->Get_MaterialInstances();

	auto pModel = Get_Component<CModel>();
	m_iMaterialInstanceCounts = pMaterialInstances.size();

	m_Cols.resize(m_iMaterialInstanceCounts);
	m_Rows.resize(m_iMaterialInstanceCounts);
	m_CurrentFrameIndexs.resize(m_iMaterialInstanceCounts);
	m_MaxFrameIndexs.resize(m_iMaterialInstanceCounts);

	for (_int idx = 0; idx < m_iMaterialInstanceCounts; ++idx)
	{
		pMaterialInstances[idx]->Set_Param("FrameIndex", { &m_CurrentFrameIndexs[idx], "int", sizeof(_int) });
		pMaterialInstances[idx]->Set_Param("Col", { &m_Cols[idx], "int", sizeof(_int) });
		pMaterialInstances[idx]->Set_Param("Row", { &m_Rows[idx], "int", sizeof(_int) });

		string strTexture;
		pMaterialInstances[idx]->GetMaterialTextureKey(TEXTURE_TYPE::DIFFUSE, 0, strTexture);
		TV_DESC Desc = CDataBase::GetInstance()->GetTVDesc(strTexture);
		m_Cols[idx] = Desc.Col;
		m_Rows[idx] = Desc.Row;
		m_MaxFrameIndexs[idx] = Desc.MaxFrame;
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
		for (_int i = 0; i < m_iMaterialInstanceCounts; ++i)
		{
			++m_CurrentFrameIndexs[i];
			if (m_CurrentFrameIndexs[i] >= m_MaxFrameIndexs[i])
				m_CurrentFrameIndexs[i] = 0;
		}
		m_fElapsedTime = 0.f;
	}
}

void CGachaStageScreen::Late_Update(_float dt)
{
}

void CGachaStageScreen::ResetMaterialInstances(vector<_int> ScreenIndex)
{
	auto pMaterial = Get_Component<CMaterial>();
	auto pMaterialInstances = pMaterial->Get_MaterialInstances();
	m_iMaterialInstanceCounts = pMaterialInstances.size();

	m_Cols.resize(m_iMaterialInstanceCounts);
	m_Rows.resize(m_iMaterialInstanceCounts);
	m_CurrentFrameIndexs.resize(m_iMaterialInstanceCounts);
	m_MaxFrameIndexs.resize(m_iMaterialInstanceCounts);

	for (_int idx = 0; idx < m_iMaterialInstanceCounts; ++idx)
	{
		pMaterialInstances[idx]->Set_Param("FrameIndex", { &m_CurrentFrameIndexs[idx], "int", sizeof(_int) });
		pMaterialInstances[idx]->Set_Param("Col", { &m_Cols[idx], "int", sizeof(_int) });
		pMaterialInstances[idx]->Set_Param("Row", { &m_Rows[idx], "int", sizeof(_int) });

		string strTexture;
		pMaterialInstances[idx]->GetMaterialTextureKey(TEXTURE_TYPE::DIFFUSE, ScreenIndex[idx], strTexture);
		TV_DESC Desc = CDataBase::GetInstance()->GetTVDesc(strTexture);
		m_Cols[idx] = Desc.Col;
		m_Rows[idx] = Desc.Row;
		m_MaxFrameIndexs[idx] = Desc.MaxFrame;
	}
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
