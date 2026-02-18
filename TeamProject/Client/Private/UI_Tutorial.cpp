#include "pch.h"
#include "UI_Tutorial.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

#include "UI_ButtonPanel.h"

HRESULT CUI_Tutorial::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

    Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_Tutorial::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("tutorial.json")));

    Create_ExitButton();
    Create_EnterButton();

	return S_OK;
}

void CUI_Tutorial::Awake()
{
}

void CUI_Tutorial::Update(_float dt)
{
    __super::Update(dt);
    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

HRESULT CUI_Tutorial::Create_ExitButton()
{
    CUI_ButtonPanel::BUTTON_DESC* pDesc = new CUI_ButtonPanel::BUTTON_DESC;
    pDesc->strJsonKey = "tutorial_exitButton.json";

    auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_ButtonPanel" })
        .Add_UIDesc(pDesc)
        .Build("buttonExit");

    if (!pObj)
        return E_FAIL;

    pObj->Set_OnClick([this]() {});
    Get_Component<CObjectContainer>()->Add_Child(pObj);

    return S_OK;
}

HRESULT CUI_Tutorial::Create_EnterButton()
{
    CUI_ButtonPanel::BUTTON_DESC* pDesc = new CUI_ButtonPanel::BUTTON_DESC;
    pDesc->strJsonKey = "tutorial_enterButton.json";

    auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_ButtonPanel" })
        .Add_UIDesc(pDesc)
        .Build("buttonEnter");

    if (!pObj)
        return E_FAIL;

    pObj->Set_OnClick([this]() { });
    Get_Component<CObjectContainer>()->Add_Child(pObj);

    return S_OK;
}

CGameObject* CUI_Tutorial::Create()
{
    CUI_Tutorial* pInstance = new CUI_Tutorial();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_Tutorial");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_Tutorial::Clone(INIT_DESC* pArg)
{
    CUI_Tutorial* pInstance = new CUI_Tutorial(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_Tutorial");
        Safe_Release(pInstance);
    }
    return pInstance;
}