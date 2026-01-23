#include "pch.h"
#include "UI_SceneFrame.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

#include "CamDirector.h"

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
    for(_int i = 0; i < ENUM(Child::END); ++i)
        m_pChildren[i] = dynamic_cast<CUI_Object*>(pContainer->Find_Descendant(INSTANCENAMES[i]));

    m_Zpriority = -1;

    return S_OK;
}

void CUI_SceneFrame::Awake()
{
    UI_Active();
}

void CUI_SceneFrame::Update(_float dt)
{
    __super::Update(dt);

    if (CamDirector()->IsFinished(CamEventType::IntroFinished))
        UI_DeActive();

    // 테스트 코드
    //if (InputDevice()->Key_Down('P'))
    //    UI_Active();
    // 
    //if (InputDevice()->Key_Down('O'))
    //    UI_DeActive();

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_SceneFrame::UI_Active(void* pArg)
{
    m_vAnimPosition = {};
    Set_Alpha(1.f);
}

void CUI_SceneFrame::UI_DeActive(void* pArg)
{
    for (_int i = 0; i < ENUM(Child::END); ++i)
    {
        auto pChild = m_pChildren[i];
        if(pChild)
            pChild->Set_Animation(0);
    }
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