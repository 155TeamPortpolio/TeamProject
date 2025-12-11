#include "UITool_Defines.h"
#include "TestUI.h"

#include "Sprite2D.h"
#include "GameInstance.h"
#include "ObjectContainer.h"

CTestUI::CTestUI()
{
}

CTestUI::CTestUI(const CTestUI& rhs)
    : CUI_Object(rhs)
{
}

HRESULT CTestUI::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    Add_Component<CObjectContainer>();

    return S_OK;
}

HRESULT CTestUI::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    CGameInstance::GetInstance()->Get_ResourceMgr()->Add_ResourcePath("Resources_Logo.png", "../Bin/Resources/UI/Logo.png");

    Get_Component<CSprite2D>()->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");
    Get_Component<CSprite2D>()->Add_Texture(G_GlobalLevelKey, "Resources_Logo.png");

    return S_OK;
}

void CTestUI::Priority_Update(_float dt)
{
    Get_Component<CObjectContainer>()->Priority_UpdateChild(dt);
}

void CTestUI::Update(_float dt)
{
    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CTestUI::Late_Update(_float dt)
{
    Get_Component<CObjectContainer>()->Late_UpdateChild(dt);
}

CGameObject* CTestUI::Create()
{
    CTestUI* pInstance = new CTestUI();

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CTestUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTestUI::Clone(INIT_DESC* pArg)
{
    CTestUI* pInstance = new CTestUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CTestUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTestUI::Free()
{
    __super::Free();
}