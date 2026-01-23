#include "pch.h"
#include "UI_Dialogue.h"

#include "GameInstance.h"
#include "DataBase.h"
#include "ObjectContainer.h"
#include "EventListener.h"
#include "TextSlot.h"
#include "ButtonUI.h"

#include "UI_DialogueMessage.h"

void CUI_Dialogue::Change_Dialogue()
{
    switch (m_tDialogueDesc.Result)
    {
    case DialogueResult::Success:
    case DialogueResult::Fail:
    {
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
        _int iSequenceID = m_tDialogueDesc.SequenceID;
        Open_Dialogue(m_tDialogueDesc.DialogueID, ++iSequenceID);
        break;
    }
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
    
    Bind_EventListener();

    Set_Alive(false);

    return S_OK;
}

void CUI_Dialogue::Update(_float dt)
{
    // 테스트 
    if (InputDevice()->Key_Down('O'))
    {
        UI_DIALOGUE_REQUEST_DESC desc = {};
        desc.strDialogueID = "Dialogue_003";
        desc.iSequenceID = 0;
        EventSystem()->Broadcast< UI_DIALOGUE_REQUEST_DESC>({ desc });
    } 

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
            Open_Dialogue(desc.strDialogueID, desc.iSequenceID);
        });
}

void CUI_Dialogue::Open_Dialogue(const string& strNewSequenceID, _uint iNewSequenceID)
{
    auto pair = make_pair(strNewSequenceID, iNewSequenceID);
    if (pair == make_pair(m_tDialogueDesc.DialogueID, m_tDialogueDesc.SequenceID))
        return;

    Change_State(STATE::VISIBLE);   // 여기서 하는게 맞나?

    m_tDialogueDesc = CDataBase::GetInstance()->GetNpcDialogueDesc(pair);

    if (auto pMessage = m_pChildren[ENUM(CHILD::MESSAGE)])
    {
        CUI_DialogueMessage::MESSAGE_DESC desc = {};
        desc.strName = m_tDialogueDesc.Name;
        desc.strMessage = m_tDialogueDesc.Text;

        pMessage->UI_Active(&desc);
    }

    switch (m_tDialogueDesc.DialogueType)
    {
    case DialogueType::Normal:  
        break;
    case DialogueType::Choice:
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
        Set_ChildUIDeActive(CHILD::MESSAGE);
        break;
    case STATE::VISIBLE:
        Set_Alive(true);
        Set_ChildUIActive(CHILD::MESSAGE);
        break;
    }
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