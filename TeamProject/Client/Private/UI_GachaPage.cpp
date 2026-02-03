#include "pch.h"
#include "UI_GachaPage.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

#include "FieldSystem.h"
#include "UI_BackButton.h"

#include "UI_GachaConversion.h"

HRESULT CUI_GachaPage::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_GachaPage::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("gacha.json")));

    Create_BackButton();
    Create_Currency();
    Create_Channels();
    Create_Conversions();

    //Set_Alive(false);

	return S_OK;
}

void CUI_GachaPage::Awake()
{
}

void CUI_GachaPage::Update(_float dt)
{
	__super::Update(dt);

	Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_GachaPage::UI_Active(void* pArg)
{
    Set_Alive(true);
}

void CUI_GachaPage::UI_DeActive(void* pArg)
{
    Set_Alive(false);
}

void CUI_GachaPage::Create_BackButton()
{
    CUI_BackButton::BUTTON_DESC* pDesc = new CUI_BackButton::BUTTON_DESC;
    pDesc->onClick = [this]() { OnClick_Back(); };

    auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_BackButton" })
        .Add_UIDesc(pDesc)
        .Build("buttonBack");

    if (!pObj)
        return;

    Get_Component<CObjectContainer>()->Add_Child(pObj);
}

void CUI_GachaPage::Create_Currency()
{
    auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_GachaCurrency" })
        .Build("currency");

    if (!pObj)
        return;

    Get_Component<CObjectContainer>()->Add_Child(pObj);
}

void CUI_GachaPage::Create_Channels()
{
    for (_int i = 0; i < 8; ++i)
    {
        auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_GachaChannel" })
            .Build("channel");

        if (!pObj)
            continue;
        
        pObj->Set_Anchor(ANCHOR::Left | ANCHOR::Top);
        pObj->Set_AnchorOffset({ 57.f, 118.f + 88.f  * i });
        Get_Component<CObjectContainer>()->Add_Child(pObj);
    } 
}

void CUI_GachaPage::Create_Conversions()
{
    static const _int iMaxCount = 2;
    static const _int COSTS[2] = { 10000, 100000 };
    static const _int COUNTS[2] = { 1, 10 }; 

    for (_int i = 0; i < iMaxCount; ++i)
    {
        CUI_GachaConversion::CONVERSION_DESC* pDesc = new CUI_GachaConversion::CONVERSION_DESC;
        pDesc->iCost = COSTS[i];
        pDesc->iCount = COUNTS[i];
        pDesc->onClick = [this]() { OnClick_Conversion(); };

        auto pObj = Builder::Create_UIObject({ G_GlobalLevelKey, "Proto_GameObject_GachaConversion" })
            .Add_UIDesc(pDesc)
            .Build("conversion");

        if (!pObj)
            return;

        _float fStartX = -55.f - 390.f * iMaxCount;
        _float fSpacing = 390.f;

        pObj->Set_Anchor(ANCHOR::Right | ANCHOR::Bottom);
        pObj->Set_AnchorOffset({ fStartX + fSpacing * i, -68.f });
        Get_Component<CObjectContainer>()->Add_Child(pObj);
    }
}

void CUI_GachaPage::OnClick_Back()
{
    FieldSystem()->RequestExitTop();
}

void CUI_GachaPage::OnClick_Conversion()
{
    // 확인 배너 띄우고 
    // 확인 배너에서 확인 누르면 가챠 레벨로 넘어가고/가격만큼 데니 깎아
}

CGameObject* CUI_GachaPage::Create()
{
    CUI_GachaPage* pInstance = new CUI_GachaPage();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_GachaPage");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_GachaPage::Clone(INIT_DESC* pArg)
{
    CUI_GachaPage* pInstance = new CUI_GachaPage(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_GachaPage");
        Safe_Release(pInstance);
    }
    return pInstance;
}