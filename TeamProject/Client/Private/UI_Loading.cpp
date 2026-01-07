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
    const string& strNextLevelKey = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NextLevel();
    string fileName = {};

    // 다음레벨에 따라 로드할 json 파일 이름 분기처리
    if (strNextLevelKey == "City_Level")
        fileName = "loading_city";
    else if (strNextLevelKey == "Hollow_Level")
        fileName = "loading_hollow";
    else
        fileName = "loading_default";

    //fileName = "loading_default";

    // Loading 프리팹 (json) 로드 후 UI 트리 루트(CanvasPanel) 생성
    string strCurrentLevel = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey();
    CUI_Object* pRoot = Builder::Create_UIObject({ strCurrentLevel, "Proto_GameObject_CanvasPanel" })
        .Asset(fileName + ".json")
        .Build("prefab");

    if (!pRoot)
        return;

    // 생성된 루트 UI를 uiMgr에 등록
    CGameInstance::GetInstance()->Get_UIMgr()->Add_UIObject(pRoot, strCurrentLevel);

    // UI 트리 기준으로 주요 핸들 캐싱 (root / chidlren)
    CacheHandle(pRoot);
     
    // 루트 UI의 0번 애니메이션 재생 (FadeIn)
    if (m_hRoot.isValid())
        m_hRoot.Get()->Set_Animation(0);

    // now loading의 0번 애니메이션 재생 (각도 움직임)
    if (m_hChildren[PREFAB::NOW_LOADING].isValid())
        m_hChildren[PREFAB::NOW_LOADING].Get()->Set_Animation(0);
}

void CUI_Loading::Update(_float dt)
{
}

void CUI_Loading::CacheHandle(CUI_Object* pRoot)
{
    // 루트 핸들 캐싱
    m_hRoot = pRoot->Get_Handle();

    // 자식 핸들 캐싱
    m_hChildren.resize(PREFAB::END);
    m_hChildren[PREFAB::NOW_LOADING] = pRoot->Get_DescendantHandle("nowLoading");
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