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

    const string& filePath = ResourceManager()->Get_ResourcePath("hud_field.json");
    Load(Helper::LoadJson<nlohmann::ordered_json>(filePath));

    Ready_PartObjects();

    /*임시*/
    m_handles[Child::GRADIENTF] = Get_DescendantHandle("gradientF");
    m_handles[Child::GRADIENTJ] = Get_DescendantHandle("gradientJ");

    return S_OK;
}

void CUI_FieldHUD::Awake()
{
}

void CUI_FieldHUD::Update(_float dt)
{
    if (!m_isTemp)
    {
        //Set_Animation(Child::GRADIENTF, 0); // 가까이 다가갔을 때 돌아가게
        Set_Animation(Child::GRADIENTJ, 0);
        m_isTemp = true;
    }

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_FieldHUD::Ready_PartObjects()
{
    //Add_PartObject("Proto_GameObject_FieldHUDAction", "action", &m_handles[ENUM(Child::ACTION)], _float2(1178.f, 655.f));
}

void CUI_FieldHUD::Add_PartObject(const string& strPrototypeTag, const string& strInstanceName, UI_HANDLE* pHandleOut, _float2 vOffset)
{
    CUI_Object* pObj = Builder::Create_UIObject({ LevelManager()->Get_NowLevelKey(), strPrototypeTag })
        .Offset(vOffset)
        .Build(strInstanceName);

    if (!pObj)
        return;

    Get_Component<CObjectContainer>()->Add_Child(pObj);

    if (pHandleOut)
        *pHandleOut = pObj->Get_Handle();
}

void CUI_FieldHUD::Set_Animation(Child child, _int iIndex)
{
    //ForChild(child, [iIndex](CUI_Object* ui) { ui->Set_Animation(iIndex); });
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