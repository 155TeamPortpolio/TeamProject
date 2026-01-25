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

    // 타이핑 중 글자 출력 콜백. 타이핑이 진행될 때마다 MESSAGE 텍스트 슬롯 갱신
    m_tMessageTypeWriter.onTyped = [this](_uint iIndex, const wstring& strText) { Set_ChildText(ENUM(CHILD::MESSAGE), strText); };

    // 다음 버튼 클릭 시 다이얼로그 진행
    if (m_pBtnNext)
        m_pBtnNext->Set_OnClick([this]() { Change_Dialogue(); });

    // 초기 상태 세팅
    Set_Alpha(0.f);
    Set_ChildAnimation(CHILD::ARROW1, 0);
    Set_ChildAnimation(CHILD::ARROW2, 0);

    return S_OK;
}

void CUI_DialogueMessage::Update(_float dt)
{
    __super::Update(dt);

    // 스페이스 입력으로 다이얼로그 진행
    Update_KeyInput();

    // 타이핑 메시지 업데이트
    Update_TypingMessage(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_DialogueMessage::UI_Active(void* pArg)
{
    // 인자가 없으면 기본 등장 애니메이션만 재생
    if (!pArg)
    {
        Set_Animation(0);
        return;
    } 

    // 다이얼로그 메시지 데이터 수신
    MESSAGE_DESC* pDesc = static_cast<MESSAGE_DESC*>(pArg);
    Set_ChildText(TEXTSLOT::NAME, pDesc->strName);  // 화자 이름 설정
    Start_TypingMessage(pDesc->strMessage);         // 메시지 타이핑 시작
    m_hasChoice = pDesc->hasChoice;                 // 선택지 존재 여부 저장
}

void CUI_DialogueMessage::UI_DeActive(void* pArg)
{
    // 사라지는 애니메이션 재생
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

void CUI_DialogueMessage::Update_KeyInput()
{
    if (InputDevice()->Key_Down(VK_SPACE))
        Change_Dialogue();
}

void CUI_DialogueMessage::Change_Dialogue()
{
    // 타이핑 중이면 먼저 전체 출력 처리
    if (Complete_TypingMessage())
        return;

    // 선택지가 있는 경우 메시지 단계에서는 진행 불가
    if (m_hasChoice)
        return;

    // 부모 UI_Dialogue에게 다이얼로그 진행 요청
    if (auto parentObj = dynamic_cast<CUI_Dialogue*>(Get_Component<CChild>()->Get_Parent()))
        parentObj->Change_Dialogue();
}

void CUI_DialogueMessage::Show_Choices()
{
    if (!m_hasChoice)
        return;

    // 부모 UI_Dialogue에 선택지 표시 요청
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

    // 타이핑이 끝났을 때 처리
    if (m_tMessageTypeWriter.isFinished())
    {
        // 타이핑이 끝나는 순간 선택지 표시
        if (m_tMessageTypeWriter.isTyping)
            Show_Choices();

        // 타이핑 상태 정리
        m_tMessageTypeWriter.Complete();
    } 
}

_bool CUI_DialogueMessage::Complete_TypingMessage()
{
    if (!m_tMessageTypeWriter.isTyping)
        return false;

    // 스킵 시에도 선택지는 표시
    if (m_tMessageTypeWriter.isTyping)
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