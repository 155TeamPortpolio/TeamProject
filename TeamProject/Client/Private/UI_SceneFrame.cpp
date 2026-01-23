#include "pch.h"
#include "UI_SceneFrame.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

#include "CamDirector.h"

#include "UIDirector.h"

HRESULT CUI_SceneFrame::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CObjectContainer>();

    return S_OK;
}

HRESULT CUI_SceneFrame::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    // JSON 기반 UI 구성 로드
    const string& filePath = ResourceManager()->Get_ResourcePath("scene_frame.json");
    Load(Helper::LoadJson<nlohmann::ordered_json>(filePath));

    // 자식을 멤버로 캐싱
    auto pContainer = Get_Component<CObjectContainer>();
    for(_int i = 0; i < ENUM(CHILD::END); ++i)
        m_pChildren[i] = dynamic_cast<CUI_Object*>(pContainer->Find_Descendant(INSTANCENAMES[i]));

    m_Zpriority = -1;

    return S_OK;
}

void CUI_SceneFrame::Awake()
{
    Set_Alive(false);
}

void CUI_SceneFrame::Update(_float dt)
{
    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);

    if (CamDirector()->IsFinished(CamEventType::IntroFinished))
        UI_DeActive();

    if (m_eState == STATE::INVISIBLE && Is_ChildAnimFinished(CHILD::TOP) && Is_ChildAnimFinished(CHILD::BOTTOM))
        Set_Alive(false);
}

void CUI_SceneFrame::UI_Active(void* pArg)
{
    Change_State(STATE::VISIBLE);
}

void CUI_SceneFrame::UI_DeActive(void* pArg)
{
    Change_State(STATE::INVISIBLE);
}

void CUI_SceneFrame::Change_State(STATE eState)
{
    if (eState == m_eState)
        return;
     
    m_eState = eState;

    switch (m_eState)
    {
    case STATE::VISIBLE:
        Set_Alive(true);
        break;
    case STATE::INVISIBLE:
        Set_ChildAnimation(CHILD::TOP, 0);
        Set_ChildAnimation(CHILD::BOTTOM, 0);
        break;
    }
}

void CUI_SceneFrame::Set_ChildAnimation(CHILD child, _int iIndex)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    pChild->Set_Animation(iIndex);
}

_bool CUI_SceneFrame::Is_ChildAnimFinished(CHILD child)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return false;

    return pChild->Is_AnimFinished();
}

CGameObject* CUI_SceneFrame::Create()
{
    CUI_SceneFrame* pInstance = new CUI_SceneFrame();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_SceneFrame");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_SceneFrame::Clone(INIT_DESC* pArg)
{
    CUI_SceneFrame* pInstance = new CUI_SceneFrame(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_SceneFrame");
        Safe_Release(pInstance);
    }
    return pInstance;
}