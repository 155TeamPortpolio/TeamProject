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
#include "Player.h"

void CUI_Dialogue::Change_Dialogue()
{
    switch (m_tDialogueDesc.Result)
    {
    // 대화가 성공 / 실패로 종료되는 경우
    case DialogueResult::Success:
    case DialogueResult::Fail:
    {   
        // UI를 닫고
        Change_State(STATE::INVISIBLE);
        // NPC 상호작용 결과를 외부 시스템에 알림
        BroadCast_NPCInteractDesc(m_tDialogueDesc.Name, m_tDialogueDesc.SequenceID, m_tDialogueDesc.NextSequenceID, m_tDialogueDesc.Result);
    }
    break;

    // 아직 대화가 이어지는 경우
    case DialogueResult::Running:
    case DialogueResult::None:
        // 다음 시퀀스로 진행
        _int iSequenceID = m_tDialogueDesc.SequenceID;
        Open_Dialogue(m_tDialogueDesc.DialogueID, ++iSequenceID);
        break;
    }
}

void CUI_Dialogue::Change_Dialogue(ChoiceDesc desc)
{
    // 선택 결과가 대화 종료를 의미하는 경우
    if (desc.Next_SequeceID == 0)
    {
        // UI를 닫고
        Change_State(STATE::INVISIBLE);
        // NPC 상호작용 결과를 외부 시스템에 알림
        BroadCast_NPCInteractDesc(m_tDialogueDesc.Name, m_tDialogueDesc.SequenceID, desc.Next_SequeceID, desc.Result);
       
        return;
    } 

    // 다음 시퀀스가 있다면 해당 시퀀스로 대화 진행
    Open_Dialogue(m_tDialogueDesc.DialogueID, desc.Next_SequeceID);
}

void CUI_Dialogue::Show_Choices()
{
    // 선택지가 없는 경우 처리하지 않음
    if (m_tDialogueDesc.ChoiceNum <= 0)
        return;

    auto pChoice = m_pChildren[ENUM(CHILD::CHOICE)];
    if (!pChoice)
        return;

    // 선택지 UI에 넘길 데이터 구성
    // 선택지 개수에 따라 문자열 채우기
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

    // 선택지 UI 활성화
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
     
    Add_Children(G_GlobalLevelKey, "Proto_GameObject_DialogueMessage", CHILD::MESSAGE);
    Add_Children(G_GlobalLevelKey, "Proto_GameObject_DialogueChoice", CHILD::CHOICE);

    Bind_EventListener();

    // 초기 상태 세팅 
    Set_Alive(false);
    m_vSize = m_WinSize;

    return S_OK;
}

void CUI_Dialogue::Update(_float dt)
{
    __super::Update(dt);

    // 다이얼로그가 사라지는 상태이며
    // 메시지, 선택 애니메이션이 끝났다면 완전히 종료 처리
    if (m_eState == STATE::INVISIBLE && 
        Is_ChildAnimFinished(CHILD::MESSAGE) && 
        Is_ChildAnimFinished(CHILD::CHOICE))
    {
        // 플레이어 입력 언락
        if (auto pPlayer = dynamic_cast<CPlayer*>(ObjectManager()->Find_Global(ENUM(GLOBAL_ID::Player))))
           pPlayer->Unlock_Input();
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
            // 대화 UI 시작
            Open_Dialogue(desc.strDialogueID, desc.iSequenceID);
            // 대화용 카메라 연출 시작
            CamDirector()->StartDialog();
            // 플레이어 입력 잠금
            if (auto pPlayer = dynamic_cast<CPlayer*>(ObjectManager()->Find_Global(ENUM(GLOBAL_ID::Player))))
                pPlayer->Lock_Input();
        });
}

void CUI_Dialogue::Open_Dialogue(const string& strNewSequenceID, _uint iNewSequenceID)
{
    auto pair = make_pair(strNewSequenceID, iNewSequenceID);
    // 이미 같은 다이얼로그가 열려있다면 무시
    if (m_eState == STATE::VISIBLE && pair == make_pair(m_tDialogueDesc.DialogueID, m_tDialogueDesc.SequenceID))
        return;

    // 다이얼로그 표시 상태로 전환
    Change_State(STATE::VISIBLE);

    // DB에서 다이얼로그 데이터 가져오기
    m_tDialogueDesc = CDataBase::GetInstance()->GetNpcDialogueDesc(pair);

    auto pMessage = m_pChildren[ENUM(CHILD::MESSAGE)];
    if (!pMessage)
        return;

    // 메시지 UI에 넘길 데이터 구성
    CUI_DialogueMessage::MESSAGE_DESC desc = {};
    desc.strName = m_tDialogueDesc.Name;
    desc.strMessage = m_tDialogueDesc.Text;
    desc.hasChoice = (m_tDialogueDesc.ChoiceNum > 0) ? true : false;

    // 메시지 UI 활성화
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
        // 메시지 UI 비활성화 + 카메라 연출 종료
        Set_ChildUIDeActive(CHILD::MESSAGE);
        CamDirector()->EndDialog();
        break;
    case STATE::VISIBLE:
        // UI 활성화
        Set_ChildUIActive(CHILD::MESSAGE);
        Set_Alive(true);
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