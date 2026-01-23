#include "pch.h"
#include "UI_Dialogue.h"

#include "GameInstance.h"
#include "DataBase.h"
#include "ObjectContainer.h"
#include "EventListener.h"
#include "TextSlot.h"
#include "ButtonUI.h"

#include "UI_DialogueMessage.h"
#include "UI_DialogueChoice.h"

#include "CamDirector.h"

void CUI_Dialogue::Change_Dialogue()
{
    switch (m_tDialogueDesc.Result)
    {
    case DialogueResult::Success:
    case DialogueResult::Fail:
    {
        Change_State(STATE::INVISIBLE);
        BroadCast_NPCInteractDesc(m_tDialogueDesc.Name, m_tDialogueDesc.SequenceID, m_tDialogueDesc.NextSequenceID, m_tDialogueDesc.Result);
    }
    break;

    case DialogueResult::Running:
    case DialogueResult::None:
        _int iSequenceID = m_tDialogueDesc.SequenceID;
        Open_Dialogue(m_tDialogueDesc.DialogueID, ++iSequenceID);
        break;
    }
}

void CUI_Dialogue::Change_Dialogue(ChoiceDesc desc)
{
    if (desc.Next_SequeceID == 0)
    {
        Change_State(STATE::INVISIBLE);
        BroadCast_NPCInteractDesc(m_tDialogueDesc.Name, m_tDialogueDesc.SequenceID, desc.Next_SequeceID, desc.Result);
       
        return;
    } 

    Open_Dialogue(m_tDialogueDesc.DialogueID, desc.Next_SequeceID);
}

void CUI_Dialogue::Show_Choices()
{
    if (m_tDialogueDesc.ChoiceNum <= 0)
        return;

    auto pChoice = m_pChildren[ENUM(CHILD::CHOICE)];
    if (!pChoice)
        return;

    CUI_DialogueChoice::CHOICE_DESC desc = {};
    desc.iChoiceNum = m_tDialogueDesc.ChoiceNum;
    switch (desc.iChoiceNum)
    {
    case 1:
        desc.strChoices.push_back(m_tDialogueDesc.Choice_ID1);
        break;
    case 2:
        desc.strChoices.push_back(m_tDialogueDesc.Choice_ID1);
        desc.strChoices.push_back(m_tDialogueDesc.Choice_ID2);
        break;
    case 3:
        desc.strChoices.push_back(m_tDialogueDesc.Choice_ID1);
        desc.strChoices.push_back(m_tDialogueDesc.Choice_ID2);
        desc.strChoices.push_back(m_tDialogueDesc.Choice_ID3);
        break;
    }

    pChoice->UI_Active(&desc);
}

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

    m_vSize = m_WinSize;

    Add_Children(G_GlobalLevelKey, "Proto_GameObject_DialogueMessage", CHILD::MESSAGE);
    Add_Children(G_GlobalLevelKey, "Proto_GameObject_DialogueChoice", CHILD::CHOICE);

    Bind_EventListener();

    Set_Alive(false);

    return S_OK;
}

void CUI_Dialogue::Update(_float dt)
{
    __super::Update(dt);

    if (m_eState == STATE::INVISIBLE && Is_ChildAnimFinished(CHILD::MESSAGE))
    {
        Set_Alive(false);
        return;
    } 

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_Dialogue::Add_Children(const string& strLevelTag, const string& strPrototypeTag, CHILD child)
{
    auto pObj = Builder::Create_UIObject({ strLevelTag, strPrototypeTag }).Build("");
    if (!pObj)
        return;

    Get_Component<CObjectContainer>()->Add_Child(pObj);
    m_pChildren[ENUM(child)] = pObj;
}

void CUI_Dialogue::Bind_EventListener()
{ 
    // 이벤트 : UI_DIALOGUE_REQUEST_DESC
    Get_Component<CEventListener>()->Add_Listener<UI_DIALOGUE_REQUEST_DESC>([this](const UI_DIALOGUE_REQUEST_DESC& desc)
        {
            CamDirector()->StartDialog();
            Open_Dialogue(desc.strDialogueID, desc.iSequenceID);
        });
}

void CUI_Dialogue::Open_Dialogue(const string& strNewSequenceID, _uint iNewSequenceID)
{
    auto pair = make_pair(strNewSequenceID, iNewSequenceID);
    if (m_eState == STATE::VISIBLE && pair == make_pair(m_tDialogueDesc.DialogueID, m_tDialogueDesc.SequenceID))
        return;

    Change_State(STATE::VISIBLE);   // 여기서 하는게 맞나?

    m_tDialogueDesc = CDataBase::GetInstance()->GetNpcDialogueDesc(pair);

    auto pMessage = m_pChildren[ENUM(CHILD::MESSAGE)];
    if (!pMessage)
        return;

    CUI_DialogueMessage::MESSAGE_DESC desc = {};
    desc.strName = m_tDialogueDesc.Name;
    desc.strMessage = m_tDialogueDesc.Text;
    desc.hasChoice = (m_tDialogueDesc.ChoiceNum > 0) ? true : false;

    pMessage->UI_Active(&desc);
}

void CUI_Dialogue::Change_State(STATE eState)
{
    if (m_eState == eState)
        return;
    
    m_eState = eState;
    switch (eState)
    {
    case STATE::INVISIBLE: 
        Set_ChildUIDeActive(CHILD::MESSAGE);
        CamDirector()->EndDialog();
        break;
    case STATE::VISIBLE:
        Set_Alive(true);
        Set_ChildUIActive(CHILD::MESSAGE);
        break;
    }
}

void CUI_Dialogue::BroadCast_NPCInteractDesc(wstring strName, _uint iCurSequenceID, _uint iNextSequenceID, DialogueResult eResult)
{
    NPC_INTERACT_DESC desc = {};
    desc.strName = strName;
    desc.iCurSequenceID = iCurSequenceID;
    desc.iNextSequenceID = iNextSequenceID;
    desc.eResult = eResult;
    EventSystem()->Broadcast<NPC_INTERACT_DESC>({ desc });
}

void CUI_Dialogue::Set_ChildUIActive(CHILD child)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    pChild->UI_Active();
}

void CUI_Dialogue::Set_ChildUIDeActive(CHILD child)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    pChild->UI_DeActive();
}

_bool CUI_Dialogue::Is_ChildAnimFinished(CHILD child)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return false;

    return pChild->Is_AnimFinished();
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