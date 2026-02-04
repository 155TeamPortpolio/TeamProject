#include "pch.h"
#include "UI_FieldHUD.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

HRESULT CUI_FieldHUD::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CObjectContainer>();

    return S_OK;
}

HRESULT CUI_FieldHUD::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("hud_field.json")));

    return S_OK;
}

void CUI_FieldHUD::Awake()
{
}

void CUI_FieldHUD::Update(_float dt)
{
    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_FieldHUD::UI_Active(void* pArg)
{
    Set_Alive(true);
}

void CUI_FieldHUD::UI_DeActive(void* pArg)
{
    Set_Alive(false);
}

CGameObject* CUI_FieldHUD::Create()
{
    CUI_FieldHUD* pInstance = new CUI_FieldHUD();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_FieldHUD");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_FieldHUD::Clone(INIT_DESC* pArg)
{
    CUI_FieldHUD* pInstance = new CUI_FieldHUD(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_FieldHUD");
        Safe_Release(pInstance);
    }
    return pInstance;
}