#include "pch.h"
#include "UI_Dialogue.h"

#include "GameInstance.h"
#include "DataBase.h"
#include "ObjectContainer.h"
#include "EventListener.h"
#include "TextSlot.h"
#include "ButtonUI.h"

HRESULT CUI_Dialogue::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CObjectContainer>();
    Add_Component<CEventListener>();

    return S_OK;
}

HRESULT CUI_Dialogue::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("dialogue.json")));
    Cache_Children();
    Bind_EventListener();

    m_tMessageTypeWriter.onTyped = [this](_uint iIndex, const wstring& strText) { Set_ChildText(ENUM(CHILD::MESSAGE), strText); };
    if (m_pNextButton)
        m_pNextButton->Set_OnClick([this]() { Change_Dialogue(); });

    Set_Alpha(0.f);
    Set_ChildAnimation(CHILD::ARROW1, 0);
    Set_ChildAnimation(CHILD::ARROW2, 0);

    return S_OK;
}

void CUI_Dialogue::Update(_float dt)
{
    __super::Update(dt);
     
    Update_TypingMessage(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_Dialogue::Cache_Children()
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
    auto pObj = pContainer->Find_Descendant("next");
    if (pObj)
    {
        auto pUI = dynamic_cast<CUI_Object*>(pObj);
        m_pNextButton = dynamic_cast<CButtonUI*>(pUI);
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

void CUI_Dialogue::Bind_EventListener()
{
    // 이벤트 : UI_DIALOGUE_REQUEST_DESC
    Get_Component<CEventListener>()->Add_Listener<UI_DIALOGUE_REQUEST_DESC>([this](const UI_DIALOGUE_REQUEST_DESC& desc)
        {
            Open_Dialogue(desc.strDialogueID, desc.iSequenceID);
        });
}

void CUI_Dialogue::Open_Dialogue(const string& strNewSequenceID, _uint iNewSequenceID)
{
    auto pair = make_pair(strNewSequenceID, iNewSequenceID);
    if (pair == make_pair(m_tDialogueDesc.DialogueID, m_tDialogueDesc.SequenceID))
        return;

    if (m_eState != STATE::VISIBLE)
        Change_State(STATE::VISIBLE);

    m_tDialogueDesc = CDataBase::GetInstance()->GetNpcDialogueDesc(pair);
    Set_ChildText(TEXTSLOT::NAME, m_tDialogueDesc.Name);
    Start_TypingMessage(m_tDialogueDesc.Text);
}

void CUI_Dialogue::Change_Dialogue()
{
    switch (m_tDialogueDesc.Result)
    {
    case DialogueResult::Success:
    case DialogueResult::Fail:
    {
        if (Complete_TypingMessage())
            return;

        Change_State(STATE::INVISIBLE);

        // 현재는 다이얼로그만 보내는데, 선택지일 때는 선택지로 
        NPC_INTERACT_DESC desc = {};
        desc.strName = m_tDialogueDesc.Name;
        desc.iCurSequenceID = m_tDialogueDesc.SequenceID;
        desc.iNextSequenceID = m_tDialogueDesc.NextSequenceID;
        desc.eResult = m_tDialogueDesc.Result;
        EventSystem()->Broadcast<NPC_INTERACT_DESC>({ desc });
    } 
        break;

    case DialogueResult::None:
    case DialogueResult::Running:
        if (Complete_TypingMessage())
            return;

        _int iSequenceID = m_tDialogueDesc.SequenceID;
        Open_Dialogue(m_tDialogueDesc.DialogueID, ++iSequenceID);
        break;
    }
}

void CUI_Dialogue::Change_State(STATE eState)
{
    if (m_eState == eState)
        return;

    m_eState = eState;
    switch (eState)
    {
    case STATE::INVISIBLE:
        Set_Animation(1);
        break;
    case STATE::VISIBLE:
        Set_Animation(0);
        break;
    }
}

void CUI_Dialogue::Start_TypingMessage(const _wstring& strText)
{
    m_tMessageTypeWriter.Start(strText);
    Set_ChildText(TEXTSLOT::MESSAGE, L"");
}

void CUI_Dialogue::Update_TypingMessage(_float dt)
{
    m_tMessageTypeWriter.Update(dt, ENUM(CHILD::MESSAGE));

    if (m_tMessageTypeWriter.isFinished())
        m_tMessageTypeWriter.Complete();
}

_bool CUI_Dialogue::Complete_TypingMessage()
{
    if (!m_tMessageTypeWriter.isTyping)
        return false;
    
    m_tMessageTypeWriter.Complete();
    Set_ChildText(TEXTSLOT::MESSAGE, m_tMessageTypeWriter.strFullText);
    return true;
}

void CUI_Dialogue::Set_ChildAnimation(CHILD eChild, _int iIndex)
{
    auto pChild = m_pChildren[ENUM(eChild)];
    if (!pChild)
        return;

    pChild->Set_Animation(iIndex);
}

void CUI_Dialogue::Set_ChildText(TEXTSLOT eTextSlot, const wstring& strText)
{
    auto pText = m_pTextSlots[ENUM(eTextSlot)];
    if (!pText)
        return;

    pText->Set_Text(strText);
}

void CUI_Dialogue::Set_ChildText(_uint iIndex, const wstring& strText)
{
    if (iIndex >= ENUM(TEXTSLOT::END))
        return;

    auto pText = m_pTextSlots[iIndex];
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