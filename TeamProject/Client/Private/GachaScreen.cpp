#include "pch.h"
#include "GachaScreen.h"

#include "StaticModel.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "Child.h"

#include "GameInstance.h"
#include "Shader.h"
#include "Texture.h"

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

	pModel->Get_MeshCount();
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

	m_Cols.resize(m_iMeshCounts);
	m_Rows.resize(m_iMeshCounts);
	m_CurrentFrameIndexs.resize(m_iMeshCounts);

	_int idx = 0;
	for (auto& instance : pMaterialInstances)
	{
		instance->Set_Param("FrameIndex", { &m_CurrentFrameIndexs[idx], "int", sizeof(_int)});
		instance->Set_Param("Col", { &m_Cols[idx], "int", sizeof(_int)});
		instance->Set_Param("Row", { &m_Rows[idx++], "int", sizeof(_int)});
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
			m_fElapsedTime = 0.f;
			if (m_CurrentFrameIndexs[i] >= m_MaxFrameIndexs[i])
				m_CurrentFrameIndexs[i] = 0;
		}
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
