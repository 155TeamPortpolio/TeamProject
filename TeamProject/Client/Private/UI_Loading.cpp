#include "pch.h"
#include "UI_Loading.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

HRESULT CUI_Loading::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CObjectContainer>();

    return S_OK;
}

HRESULT CUI_Loading::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    const string& strNextLevelKey = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NextLevel();
    string fileName = {};
    
    // 다음레벨에 따라 로드할 json 파일 이름 분기처리
    if (strNextLevelKey == "City_Level")
        fileName = "loading_city";
    else if (strNextLevelKey == "Zero_Level")
        fileName = "loading_hollow";
    else
        fileName = "loading_default";

    fileName = "loading_default";
     
    // JSON 기반 UI 구성 로드
    const string& filePath = ResourceManager()->Get_ResourcePath(fileName + ".json");
    Load(Helper::LoadJson<nlohmann::ordered_json>(filePath));

    return S_OK;
}

void CUI_Loading::Awake()
{ 
    // 0번 애니메이션 재생 (FadeIn)
    Set_Animation(0);

    // now loading의 0번 애니메이션 재생 (각도 움직임)
    if (auto pObj = Get_Component<CObjectContainer>()->Find_Descendant("nowLoading"))
        if (auto pUI = dynamic_cast<CUI_Object*>(pObj))
            pUI->Set_Animation(0);
}

void CUI_Loading::Update(_float dt)
{
    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
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