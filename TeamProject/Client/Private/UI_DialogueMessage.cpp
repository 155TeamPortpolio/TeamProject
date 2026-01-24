#include "pch.h"
#include "UI_DialogueMessage.h"

#include "GameInstance.h"
#include "DataBase.h"
#include "ObjectContainer.h"
#include "Child.h"
#include "TextSlot.h"
#include "ButtonUI.h"

#include "UI_Dialogue.h"

HRESULT CUI_DialogueMessage::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CObjectContainer>();

    return S_OK;
}

HRESULT CUI_DialogueMessage::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("dialogue_message.json")));
    Cache_Children();

    m_tMessageTypeWriter.onTyped = [this](_uint iIndex, const wstring& strText) { Set_ChildText(ENUM(CHILD::MESSAGE), strText); };

    if (m_pBtnNext)
        m_pBtnNext->Set_OnClick([this]() { Change_Dialogue(); });

    Set_Alpha(0.f);
    Set_ChildAnimation(CHILD::ARROW1, 0);
    Set_ChildAnimation(CHILD::ARROW2, 0);

    return S_OK;
}

void CUI_DialogueMessage::Update(_float dt)
{
    __super::Update(dt);

    if (InputDevice()->Key_Down(VK_SPACE))
        Change_Dialogue();

    Update_TypingMessage(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_DialogueMessage::UI_Active(void* pArg)
{
    if (!pArg)
    {
        Set_Animation(0);
        return;
    } 

    MESSAGE_DESC* pDesc = static_cast<MESSAGE_DESC*>(pArg);
    Set_ChildText(TEXTSLOT::NAME, pDesc->strName);
    Start_TypingMessage(pDesc->strMessage);
    m_hasChoice = pDesc->hasChoice;
}

void CUI_DialogueMessage::UI_DeActive(void* pArg)
{
    Set_Animation(1);
}

void CUI_DialogueMessage::Cache_Children()
{
    auto pContainer = Get_Component<CObjectContainer>();

    // 자식 UI 오브젝트 포인터를 배열에 캐싱
    for (_int i = 0; i < ENUM(CHILD::END); ++i)
    {
        const string& strInstanceName = CHILD_INSTNAMES[i];
        if (strInstanceName.empty())
            continue;

        auto pObj = pContainer->Find_Descendant(strInstanceName);
        if (!pObj)
            continue;

        auto pUI = dynamic_cast<CUI_Object*>(pObj);
        m_pChildren[i] = pUI;
    }

    // 버튼 UI 포인터로 캐싱
    auto pObj = pContainer->Find_Descendant("btnNext");
    if (pObj)
    {
        auto pUI = dynamic_cast<CUI_Object*>(pObj);
        m_pBtnNext = dynamic_cast<CButtonUI*>(pUI);
    }

    // 텍스트 컴포넌트를 배열에 캐싱
    for (_int i = 0; i < ENUM(TEXTSLOT::END); ++i)
    {
        const string& strInstanceName = TEXTSLOT_INSTNAMES[i];
        if (strInstanceName.empty())
            continue;

        auto pObj = pContainer->Find_Descendant(strInstanceName);
        if (!pObj)
            continue;

        m_pTextSlots[i] = pObj->Get_Component<CTextSlot>();
    }
}

void CUI_DialogueMessage::Change_Dialogue()
{
    if (Complete_TypingMessage())
        return;

    if (m_hasChoice)
        return;

    if (auto parentObj = dynamic_cast<CUI_Dialogue*>(Get_Component<CChild>()->Get_Parent()))
        parentObj->Change_Dialogue();
}

void CUI_DialogueMessage::Show_Choices()
{
    if (!m_hasChoice)
        return;

    if (auto parentObj = dynamic_cast<CUI_Dialogue*>(Get_Component<CChild>()->Get_Parent()))
       parentObj->Show_Choices();
}

void CUI_DialogueMessage::Start_TypingMessage(const _wstring& strText)
{
    m_tMessageTypeWriter.Start(strText);
    Set_ChildText(TEXTSLOT::MESSAGE, L"");
}

void CUI_DialogueMessage::Update_TypingMessage(_float dt)
{
    m_tMessageTypeWriter.Update(dt, ENUM(CHILD::MESSAGE));

    if (m_tMessageTypeWriter.isFinished())
    {
        if (m_tMessageTypeWriter.isTyping)  /////////////
            Show_Choices();

        m_tMessageTypeWriter.Complete();
    } 
}

_bool CUI_DialogueMessage::Complete_TypingMessage()
{
    if (!m_tMessageTypeWriter.isTyping)
        return false;

    if (m_tMessageTypeWriter.isTyping)  ////////////
        Show_Choices();

    m_tMessageTypeWriter.Complete();
    Set_ChildText(TEXTSLOT::MESSAGE, m_tMessageTypeWriter.strFullText);

    return true;
}

void CUI_DialogueMessage::Set_ChildAnimation(CHILD eChild, _int iIndex)
{
    auto pChild = m_pChildren[ENUM(eChild)];
    if (!pChild)
        return;

    pChild->Set_Animation(iIndex);
}

void CUI_DialogueMessage::Set_ChildText(TEXTSLOT eTextSlot, const wstring& strText)
{
    auto pText = m_pTextSlots[ENUM(eTextSlot)];
    if (!pText)
        return;

    pText->Set_Text(strText);
}

void CUI_DialogueMessage::Set_ChildText(_uint iIndex, const wstring& strText)
{
    if (iIndex >= ENUM(TEXTSLOT::END))
        return;

    auto pText = m_pTextSlots[iIndex];
    if (!pText)
        return;

    pText->Set_Text(strText);
}

CGameObject* CUI_DialogueMessage::Create()
{
    CUI_DialogueMessage* pInstance = new CUI_DialogueMessage();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_DialogueMessage");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_DialogueMessage::Clone(INIT_DESC* pArg)
{
    CUI_DialogueMessage* pInstance = new CUI_DialogueMessage(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_DialogueMessage");
        Safe_Release(pInstance);
    }
    return pInstance;
}