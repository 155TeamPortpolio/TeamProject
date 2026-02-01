#include "pch.h"
#include "GachaScreen.h"

#include "StaticModel.h"
#include "Material.h"
#include "Child.h"

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
}

void CGachaScreen::Priority_Update(_float dt)
{
}

void CGachaScreen::Update(_float dt)
{
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
