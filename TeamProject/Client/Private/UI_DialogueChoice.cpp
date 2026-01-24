#include "pch.h"
#include "UI_DialogueChoice.h"

#include "GameInstance.h"
#include "DataBase.h"
#include "ObjectContainer.h"
#include "Child.h"
#include "TextSlot.h"
#include "ButtonUI.h"

#include "UI_Dialogue.h"

HRESULT CUI_DialogueChoice::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CObjectContainer>();

    return S_OK;
}

HRESULT CUI_DialogueChoice::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("dialogue_choice.json")));
    Cache_Children();

    // 각 선택지 버튼에 클릭 이벤트 바인딩
    for (_int i = 0; i < ENUM(BTNS::END); ++i)
    {
        if (!m_pBtns[i])
            continue;

        m_pBtns[i]->Set_OnClick([=]() { Change_Dialogue(i); });
    }

    // 초기 상태 세팅
    Set_Alive(false);

    return S_OK;
}

void CUI_DialogueChoice::Update(_float dt)
{
    __super::Update(dt);

    // 숫자 키 입력(1 ~ 3)으로 선택지 선택
    Update_KeyInput();

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_DialogueChoice::UI_Active(void* pArg)
{
    if (!pArg)
        return;

    auto pDataBase = CDataBase::GetInstance();

    // 선택지 정보 수신
    CHOICE_DESC* pDesc = static_cast<CHOICE_DESC*>(pArg);
    m_iNumChoices = pDesc->iChoiceNum;
    // 선택지 개수만큼 UI 세팅
    for (_int i = 0; i < pDesc->iChoiceNum; ++i)
    {
        // DB에서 선택지 상세 정보 로드
        m_pChoiceDesc[i] = pDataBase->GetNpcChoiceDesc(pDesc->strChoices[i]);

        Set_Alive(true); 
        Set_ChildAnimation(CHOICES[i], i);
        Set_ChildText(static_cast<TEXTSLOT>(i), m_pChoiceDesc[i].Text);
    }
}

void CUI_DialogueChoice::UI_DeActive(void* pArg)
{
}

void CUI_DialogueChoice::Cache_Children()
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

    // 버튼 UI 오브젝트 포인터를 배열에 캐싱
    for (_int i = 0; i < ENUM(CHILD::END); ++i)
    {
        const string& strInstanceName = BTN_INSTNAMES[i];
        if (strInstanceName.empty())
            continue;

        auto pObj = pContainer->Find_Descendant(strInstanceName);
        if (!pObj)
            continue;

        auto pBtn = dynamic_cast<CButtonUI*>(pObj);
        m_pBtns[i] = pBtn;
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

void CUI_DialogueChoice::Update_KeyInput()
{
    char vkKey = '0';
    for (_int i = 0; i < m_iNumChoices; ++i)
    {
        if (InputDevice()->Key_Down(vkKey + (i + 1)))
            Change_Dialogue(i);
    }
}

void CUI_DialogueChoice::Change_Dialogue(_int iIndex)
{
    // 선택지 UI 비활성화
    Set_Alive(false);

    // 부모 다이얼로그에게 선택 결과 전달
    if (auto parentObj = dynamic_cast<CUI_Dialogue*>(Get_Component<CChild>()->Get_Parent()))
        parentObj->Change_Dialogue(m_pChoiceDesc[iIndex]);
}

void CUI_DialogueChoice::Set_ChildAnimation(CHILD eChild, _int iIndex)
{
    auto pChild = m_pChildren[ENUM(eChild)];
    if (!pChild)
        return;

    pChild->Set_Animation(iIndex);
}

void CUI_DialogueChoice::Set_ChildText(TEXTSLOT eTextSlot, const wstring& strText)
{
    auto pText = m_pTextSlots[ENUM(eTextSlot)];
    if (!pText)
        return;

    pText->Set_Text(strText);
}

CGameObject* CUI_DialogueChoice::Create()
{
    CUI_DialogueChoice* pInstance = new CUI_DialogueChoice();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_DialogueChoice");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_DialogueChoice::Clone(INIT_DESC* pArg)
{
    CUI_DialogueChoice* pInstance = new CUI_DialogueChoice(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_DialogueChoice");
        Safe_Release(pInstance);
    }
    return pInstance;
}