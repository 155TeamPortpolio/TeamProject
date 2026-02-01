#include "pch.h"
#include "GachaStageScreen.h"

#include "StaticModel.h"
#include "Material.h"
#include "Child.h"

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

    pModel->Link_Model("Gacha_Level", "BangBooScreen1.model");
    pMaterial->Link_Material("Gacha_Level", "BangBooScreen1.mat");

    //pModel->Link_Model("Gacha_Level", "AvatarScreen.model");
    //pMaterial->Link_Material("Gacha_Level", "AvatarScreen.mat");

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
}

void CGachaStageScreen::Priority_Update(_float dt)
{
}

void CGachaStageScreen::Update(_float dt)
{
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
