#include "pch.h"
#include "UI_Dialogue.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "TextSlot.h"

HRESULT CUI_Dialogue::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CObjectContainer>();

    return S_OK;
}

HRESULT CUI_Dialogue::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("dialogue.json")));

    Cache_Children();

    Set_Alpha(0.f);

    return S_OK;
}

void CUI_Dialogue::Awake()
{
}

void CUI_Dialogue::Update(_float dt)
{
    if (InputDevice()->Key_Down('P'))
    {
        Start_Typing(TYPING::NAME, L"가나다라마바사");
    }
    if (InputDevice()->Key_Down('O'))
    {
        Start_Typing(TYPING::TEXT, L"가나다라마바사\n가나다러아ㅓ리너아러아환어라ㅣ너이라ㅓㅣ\nㅇ러ㅏㄴㅇ러ㅣㅏㅇ널");
    }

    for (_int i = 0; i < ENUM(TYPING::END); ++i)
    {
        auto& typing = m_tTyping[i];

        if (typing.isTyping)
        {
            typing.fCharAcc += dt;

            if (typing.fCharAcc >= typing.fCharInterval)
            {
                typing.fCharAcc = 0.f;

                if (typing.iCurChar < typing.strFullText.length())
                {
                    typing.strCurText.push_back(typing.strFullText[typing.iCurChar]);
                    ++typing.iCurChar;

                    Set_ChildText(static_cast<TYPING>(i), typing.strCurText);
                }
                else
                    typing.isTyping = false;
            }
        }
    } 

    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_Dialogue::Cache_Children()
{
    auto pContainer = Get_Component<CObjectContainer>();

    // 자식 UI 오브젝트 포인터를 배열에 캐싱
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

    // 텍스트 컴포넌트를 배열에 캐싱
    for (_int i = 0; i < ENUM(TYPING::END); ++i)
    {
        const string& strInstanceName = TEXTNAMES[i];
        if (strInstanceName.empty())
            continue;

        auto pObj = pContainer->Find_Descendant(strInstanceName);
        if (!pObj)
            continue;

        m_pTexts[i] = pObj->Get_Component<CTextSlot>();
    }
}

void CUI_Dialogue::Start_Typing(TYPING typing, const _wstring& strText)
{
    auto& text = m_tTyping[ENUM(typing)];

    text.strFullText = strText;
    text.strCurText = L"";
    text.iCurChar = 0;
    text.fCharAcc = 0.f;
    text.isTyping = true;

    Set_ChildText(typing, L"");
}

void CUI_Dialogue::Set_ChildText(TYPING typing, const wstring& strText)
{
    auto pText = m_pTexts[ENUM(typing)];
    if (!pText)
        return;

    pText->Set_Text(strText);
}

CGameObject* CUI_Dialogue::Create()
{
    CUI_Dialogue* pInstance = new CUI_Dialogue();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_Dialogue");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_Dialogue::Clone(INIT_DESC* pArg)
{
    CUI_Dialogue* pInstance = new CUI_Dialogue(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_Dialogue");
        Safe_Release(pInstance);
    }
    return pInstance;
}