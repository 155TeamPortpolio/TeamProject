#include "pch.h"
#include "GachaScreen.h"

#include "StaticModel.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "Child.h"

#include "GameInstance.h"
#include "Shader.h"
#include "Texture.h"
#include "DataBase.h"

CGachaScreen::CGachaScreen()
	:CGameObject()
{
}

CGachaScreen::CGachaScreen(const CGachaScreen& rhs)
	:CGameObject(rhs)
{
}

void CGachaScreen::PlayTVSequence(vector<WEAPON_DESC>* ResultDesc)
{
	if (m_fScreenElapsedTime >= m_fIntervalScreenDuration)
	{
		if(m_iCurPlayingIndex < 10) 
			++m_iCurPlayingIndex;

		SetMaterialInstances(m_iCurPlayingIndex,{
			(*ResultDesc)[0].Grade == GachaGrade::S ? 5 : (*ResultDesc)[0].Grade == GachaGrade::A ? 3 : 1,
			(*ResultDesc)[1].Grade == GachaGrade::S ? 5 : (*ResultDesc)[1].Grade == GachaGrade::A ? 3 : 1,
			(*ResultDesc)[2].Grade == GachaGrade::S ? 5 : (*ResultDesc)[2].Grade == GachaGrade::A ? 3 : 1,
			(*ResultDesc)[3].Grade == GachaGrade::S ? 5 : (*ResultDesc)[3].Grade == GachaGrade::A ? 3 : 1,
			(*ResultDesc)[4].Grade == GachaGrade::S ? 5 : (*ResultDesc)[4].Grade == GachaGrade::A ? 3 : 1,
			(*ResultDesc)[5].Grade == GachaGrade::S ? 5 : (*ResultDesc)[5].Grade == GachaGrade::A ? 3 : 1,
			(*ResultDesc)[6].Grade == GachaGrade::S ? 5 : (*ResultDesc)[6].Grade == GachaGrade::A ? 3 : 1,
			(*ResultDesc)[7].Grade == GachaGrade::S ? 5 : (*ResultDesc)[7].Grade == GachaGrade::A ? 3 : 1,
			(*ResultDesc)[8].Grade == GachaGrade::S ? 5 : (*ResultDesc)[8].Grade == GachaGrade::A ? 3 : 1,
			(*ResultDesc)[9].Grade == GachaGrade::S ? 5 : (*ResultDesc)[9].Grade == GachaGrade::A ? 3 : 1,
			});
		m_fScreenElapsedTime = 0.f;
	}
}

void CGachaScreen::SetupInitialTVSequence(vector<WEAPON_DESC>* ResultDesc)
{
	auto pMaterial = Get_Component<CMaterial>();
	auto pMaterialInstances = pMaterial->Get_MaterialInstances();

	SetMaterialInstances(pMaterialInstances.size(), {
	(*ResultDesc)[0].Grade == GachaGrade::S ? 4 : (*ResultDesc)[0].Grade == GachaGrade::A ? 2 : 0,
	(*ResultDesc)[1].Grade == GachaGrade::S ? 4 : (*ResultDesc)[1].Grade == GachaGrade::A ? 2 : 0,
	(*ResultDesc)[2].Grade == GachaGrade::S ? 4 : (*ResultDesc)[2].Grade == GachaGrade::A ? 2 : 0,
	(*ResultDesc)[3].Grade == GachaGrade::S ? 4 : (*ResultDesc)[3].Grade == GachaGrade::A ? 2 : 0,
	(*ResultDesc)[4].Grade == GachaGrade::S ? 4 : (*ResultDesc)[4].Grade == GachaGrade::A ? 2 : 0,
	(*ResultDesc)[5].Grade == GachaGrade::S ? 4 : (*ResultDesc)[5].Grade == GachaGrade::A ? 2 : 0,
	(*ResultDesc)[6].Grade == GachaGrade::S ? 4 : (*ResultDesc)[6].Grade == GachaGrade::A ? 2 : 0,
	(*ResultDesc)[7].Grade == GachaGrade::S ? 4 : (*ResultDesc)[7].Grade == GachaGrade::A ? 2 : 0,
	(*ResultDesc)[8].Grade == GachaGrade::S ? 4 : (*ResultDesc)[8].Grade == GachaGrade::A ? 2 : 0,
	(*ResultDesc)[9].Grade == GachaGrade::S ? 4 : (*ResultDesc)[9].Grade == GachaGrade::A ? 2 : 0,
	Helper::Get_Random_Int(0, 2),
	Helper::Get_Random_Int(0, 2),
	Helper::Get_Random_Int(0, 2),
	Helper::Get_Random_Int(0, 2),
	Helper::Get_Random_Int(0, 2),
	Helper::Get_Random_Int(0, 2),
	Helper::Get_Random_Int(0, 2),
	Helper::Get_Random_Int(0, 2),
	Helper::Get_Random_Int(0, 2),
	Helper::Get_Random_Int(0, 2)
		});
}

HRESULT CGachaScreen::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    auto pModel = Add_Component<CStaticModel>();
    auto pMaterial = Add_Component<CMaterial>();

    pModel->Link_Model("Gacha_Level", "TVScreen1.model");
    pMaterial->Link_Material("Gacha_Level", "TVScreen1.mat");
    return S_OK;
}

HRESULT CGachaScreen::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CGachaScreen::Awake()
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
		pMaterialInstances[idx]->Set_Param("FrameIndex", { &m_CurrentFrameIndexs[idx], "int", sizeof(_int)});
		pMaterialInstances[idx]->Set_Param("Col", { &m_Cols[idx], "int", sizeof(_int)});
		pMaterialInstances[idx]->Set_Param("Row", { &m_Rows[idx], "int", sizeof(_int)});

		string strTexture;
		pMaterialInstances[idx]->GetMaterialTextureKey(TEXTURE_TYPE::DIFFUSE, 0, strTexture);
		TV_DESC Desc = CDataBase::GetInstance()->GetTVDesc(strTexture);
		m_Cols[idx] = Desc.Col;
		m_Rows[idx] = Desc.Row;
		m_MaxFrameIndexs[idx] = Desc.MaxFrame;
	}
}

void CGachaScreen::Priority_Update(_float dt)
{
	m_fScreenElapsedTime += dt;
}

void CGachaScreen::Update(_float dt)
{
	m_fFrameElapsedTime += dt;
	if (m_fFrameElapsedTime > m_fFrameDuration)
	{
		for (_int i = 0; i < m_iMaterialInstanceCounts; ++i)
		{
			++m_CurrentFrameIndexs[i];
			if (m_CurrentFrameIndexs[i] >= m_MaxFrameIndexs[i])
				m_CurrentFrameIndexs[i] = 0;
		}
		m_fFrameElapsedTime = 0.f;
	}
}

void CGachaScreen::Late_Update(_float dt)
{
}

void CGachaScreen::SetMaterialInstances(_int ChangeNum, vector<_int> ScreenIndex)
{
	auto pMaterial = Get_Component<CMaterial>();
	auto pMaterialInstances = pMaterial->Get_MaterialInstances();

	for (_int idx = 0; idx < ChangeNum; ++idx)
	{
		string strTexture;
		pMaterialInstances[idx]->ChangeTexture(TEXTURE_TYPE::DIFFUSE, ScreenIndex[idx]);
		pMaterialInstances[idx]->GetMaterialTextureKey(TEXTURE_TYPE::DIFFUSE, ScreenIndex[idx], strTexture);
		TV_DESC Desc = CDataBase::GetInstance()->GetTVDesc(strTexture);
		m_Cols[idx] = Desc.Col;
		m_Rows[idx] = Desc.Row;
		m_MaxFrameIndexs[idx] = Desc.MaxFrame;
	}
}

CGachaScreen* CGachaScreen::Create()
{
	CGachaScreen* Instance = new CGachaScreen();
	if (FAILED(Instance->Initialize_Prototype()))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

CGameObject* CGachaScreen::Clone(INIT_DESC* pArg)
{
	CGachaScreen* Instance = new CGachaScreen(*this);
	if (FAILED(Instance->Initialize(pArg)))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

void CGachaScreen::Free()
{
	__super::Free();
}
