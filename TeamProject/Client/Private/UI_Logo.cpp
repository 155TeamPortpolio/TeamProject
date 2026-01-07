#include "pch.h"
#include "UI_Logo.h"

#include "GameInstance.h"

HRESULT CUI_Logo::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    return S_OK;
}

HRESULT CUI_Logo::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    return S_OK;
}

void CUI_Logo::Awake()
{
    string strCurrentLevel = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey();

    // Logo 프리팹 (json) 로드 후 UI 트리 루트(CanvasPanel) 생성
    CUI_Object* pPrefab = Builder::Create_UIObject({ strCurrentLevel, "Proto_GameObject_CanvasPanel" })
        .Asset("logo.json")
        .Build("prefab");
    
    if (!pPrefab)
        return;

    // 생성된 루트 UI를 uiMgr에 등록
    CGameInstance::GetInstance()->Get_UIMgr()->Add_UIObject(pPrefab, strCurrentLevel);

    // 루트 핸들 캐싱
    m_hRoot = pPrefab->Get_Handle();

    // 루트 UI의 0번 애니메이션 재생 (FadeIn)
    if (m_hRoot.isValid())
        m_hRoot.Get()->Set_Animation(0);
}

void CUI_Logo::Update(_float dt)
{ 
}

CGameObject* CUI_Logo::Create()
{
    CUI_Logo* pInstance = new CUI_Logo();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_Logo");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_Logo::Clone(INIT_DESC* pArg)
{
    CUI_Logo* pInstance = new CUI_Logo(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_Logo");
        Safe_Release(pInstance);
    }
    return pInstance;
}