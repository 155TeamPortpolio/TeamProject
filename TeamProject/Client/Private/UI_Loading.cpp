#include "pch.h"
#include "UI_Loading.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

HRESULT CUI_Loading::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    return S_OK;
}

HRESULT CUI_Loading::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    return S_OK;
}

void CUI_Loading::Awake()
{
    m_hChildren.resize(PREFAB::END);

    const string& strNextLevelKey = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NextLevel();
    string fileName = {};

    // 다음레벨에 따라 분기
    if (strNextLevelKey == "City_Level")
        fileName = "loading_city";
    else if (strNextLevelKey == "Hollow_Level")
        fileName = "loading_hollow";
    else
        fileName = "loading_default";

    // 다음레벨에 맞는 json 로드
    string strCurrentLevel = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey();
    CUI_Object* pPrefab = Builder::Create_UIObject({ strCurrentLevel, "Proto_GameObject_CanvasPanel" })
        .Asset(fileName + ".json")
        .Build("prefabLoading");

    if (!pPrefab)
        return;

    // 
    CGameInstance::GetInstance()->Get_UIMgr()->Add_UIObject(pPrefab, strCurrentLevel);
    m_hRoot = pPrefab->Get_Handle();

    auto pNowLoading = dynamic_cast<CUI_Object*>(pPrefab->Get_Component<CObjectContainer>()->Find_Descendant("prefab_nowLoading"));
    if(pNowLoading)
        m_hChildren[PREFAB::NOW_LOADING] = pNowLoading->Get_Handle();

    // root의 0번 애니메이션 재생 (페이드인)
    if (m_hRoot.isValid())
        m_hRoot.Get()->Set_Animation(0);

    // now loading의 0번 애니메이션 재생 (각도 움직임)
    if (m_hChildren[PREFAB::NOW_LOADING].isValid())
        m_hChildren[PREFAB::NOW_LOADING].Get()->Set_Animation(0);
}

void CUI_Loading::Update(_float dt)
{
}

CGameObject* CUI_Loading::Create()
{
    CUI_Loading* pInstance = new CUI_Loading();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_Loading");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_Loading::Clone(INIT_DESC* pArg)
{
    CUI_Loading* pInstance = new CUI_Loading(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_Loading");
        Safe_Release(pInstance);
    }
    return pInstance;
}