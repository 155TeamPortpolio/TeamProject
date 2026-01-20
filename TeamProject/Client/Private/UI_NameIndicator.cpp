#include "pch.h"
#include "UI_NameIndicator.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "CharacterController.h"
#include "TextSlot.h"

HRESULT CUI_NameIndicator::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CObjectContainer>();

    return S_OK;
}

HRESULT CUI_NameIndicator::Initialize(INIT_DESC* pArg)
{
    INDICATOR_DESC* pDesc = static_cast<INDICATOR_DESC*>(pArg);
    m_strName = pDesc->strName;
    m_pCCT = pDesc->pCCT;

    __super::Initialize(pArg);

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("name_indicator.json")));
    Cache_Children();
    Set_Name(pDesc->strName);

    return S_OK;
}

void CUI_NameIndicator::Awake()
{
}

void CUI_NameIndicator::Update(_float dt)
{
    if(m_pCCT)
        XMStoreFloat3(&m_vPosition, m_pCCT->Get_FootPosition() + XMVectorSet(0.f, m_pCCT->Get_HalfSize() * 2.f, 0.f, 0.f));

    Update_WorldToScreen(m_vPosition);

    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_NameIndicator::Cache_Children()
{
    auto pContainer = Get_Component<CObjectContainer>();
    for (_int i = 0; i < ENUM(CHILD::END); ++i)
    {
        const string& strInstanceName = INSTANCENAMES[i];
        if (strInstanceName.empty())
            continue;

        auto pObj = pContainer->Find_Descendant(strInstanceName);
        if (!pObj)
            continue;

        auto pUI = dynamic_cast<CUI_Object*>(pObj);
        m_pChildren[i] = pUI;
    }

    m_pName = m_pChildren[ENUM(CHILD::NAME)]->Get_Component<CTextSlot>();
}

void CUI_NameIndicator::Set_Name(const wstring& strName)
{
    if (!m_pName)
        return;

    m_strName = strName;

    m_pName->Set_Text(strName);

    //// 부모 크기 = 텍스트 크기
    //_float2 textSize = m_pName->Get_TextSize() * m_pName->Get_Scale();
    //m_vSize = textSize;
    //
    //// 로컬 중심
    //_float2 localCenter = {
    //    m_vSize.x * 0.5f,
    //    m_vSize.y * 0.5f
    //};
    //
    //m_pName->Set_AutoPos(
    //    ANCHOR::Center,
    //    localCenter
    //);

    //if (!m_pName)
    //    return;
    //
    //m_pName->Set_Text(strName);
    //
    ////_float2 vSize = m_pName->Get_TextSize();
    ////TCHAR szSize[32] = {};
    ////swprintf_s(szSize, _countof(szSize), L"%f, %f\n", vSize.x, vSize.y);
    ////OutputDebugString(szSize);
    ////
    ////m_vSize = _float2(100.f, 20.f);// m_pName->Get_TextSize()* m_pName->Get_Scale();
    //_float2 parentCenter;
    //parentCenter.x = m_vLeftTop.x + m_vSize.x * 0.5f;
    //parentCenter.y = m_vLeftTop.y + m_vSize.y * 0.5f;
    //
    //// 4. 자식 텍스트를 부모 중심에 정렬
    //m_pName->Set_AutoPos(
    //    ANCHOR::Center,
    //    parentCenter
    //);
}

CGameObject* CUI_NameIndicator::Create()
{
    CUI_NameIndicator* pInstance = new CUI_NameIndicator();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_NameIndicator");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_NameIndicator::Clone(INIT_DESC* pArg)
{
    CUI_NameIndicator* pInstance = new CUI_NameIndicator(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_NameIndicator");
        Safe_Release(pInstance);
    }
    return pInstance;
}