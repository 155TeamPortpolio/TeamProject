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
	m_iMeshCounts = pModel->Get_MeshCount();

	m_Cols.resize(m_iMeshCounts);
	m_Rows.resize(m_iMeshCounts);
	m_CurrentFrameIndexs.resize(m_iMeshCounts);
	m_MaxFrameIndexs.resize(m_iMeshCounts);

	for (_int idx = 0; idx < pMaterialInstances.size(); ++idx)
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
}

void CGachaScreen::Update(_float dt)
{
	m_fElapsedTime += dt;
	if (m_fElapsedTime > m_fFrameDuration)
	{
		for (_int i = 0; i < m_iMeshCounts; ++i)
		{
			++m_CurrentFrameIndexs[i];
			if (m_CurrentFrameIndexs[i] >= m_MaxFrameIndexs[i])
				m_CurrentFrameIndexs[i] = 0;
		}
		m_fElapsedTime = 0.f;
	}
}

void CGachaScreen::Late_Update(_float dt)
{
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
