#include "pch.h"
#include "UI_Loading.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "Sprite2D.h"
#include "TextSlot.h"

HRESULT CUI_Loading::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    Add_Component<CObjectContainer>();

    return S_OK;
}

HRESULT CUI_Loading::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    const string& strNextLevelKey = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NextLevel();
    string fileName = {};
    string bgFileName = {};

    // 다음레벨에 따라 로드할 json 파일 이름 분기처리
    if (strNextLevelKey == "Zero_Level" ||
        strNextLevelKey == "Scott_Level")
    {
        fileName = "loading_hollow";
        bgFileName = "Loading_Hollow_Zero_Cam" + to_string(Helper::Get_Random_Int(1, 6));
    }
    else if (strNextLevelKey == "MainCity_Level")
    {
        fileName = "loading_city";
        bgFileName = "Loading_MainCity_Cam" + to_string(Helper::Get_Random_Int(1, 6));
    }
    else
    {
        fileName = "loading_default";
        bgFileName = "Loading_MainCity_Cam1";
    }

    //fileName = "loading_default"; //테스트 코드

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath(fileName + ".json")));

    Cache();

    if (m_pBg)
        m_pBg->Change_Texture(0, G_GlobalLevelKey, bgFileName + ".png");

    if (m_pSubtitle)
        m_pSubtitle->Set_Text(Get_RandomText(strNextLevelKey));

    return S_OK;
}

void CUI_Loading::Awake()
{
    // 자식(fade) 자식의 0번 애니메이션 재생 (FadeIn)
    if (auto pObj = Get_Component<CObjectContainer>()->Find_Descendant("fade"))
        if (auto pUI = dynamic_cast<CUI_Object*>(pObj))
            pUI->Set_Animation(0);

    // 자식(now loading)의 0번 애니메이션 재생 (각도 움직임)
    if (auto pObj = Get_Component<CObjectContainer>()->Find_Descendant("nowLoading"))
        if (auto pUI = dynamic_cast<CUI_Object*>(pObj))
            pUI->Set_Animation(0);
}

void CUI_Loading::Update(_float dt)
{
    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_Loading::Cache()
{
    auto pContainer = Get_Component<CObjectContainer>();

    auto pBG = pContainer->Find_Descendant("bg");
    if (pBG)
        m_pBg = pBG->Get_Component<CSprite2D>();

    auto pSubtitle = pContainer->Find_Descendant("subtitle");
    if (pSubtitle)
        m_pSubtitle = pSubtitle->Get_Component<CTextSlot>();
}

wstring CUI_Loading::Get_RandomText(const string& strNextLevelKey)
{
    int index = 0;

    // ---------------------------
    // 제로공동 / 스코트
    // ---------------------------
    if (strNextLevelKey == "Zero_Level" ||
        strNextLevelKey == "Scott_Level")
    {
        index = Helper::Get_Random_Int(0, 5);

        switch (index)
        {
        case 0:
            return L"공동 내부에서는 에테르 농도가 지속적으로 상승하며\n장시간 체류 시 침식 위험이 증가합니다.";
        case 1:
            return L"적의 공격 직전 금빛 알림이 나타나면 극한 지원을\n발동할 수 있으며 붉은빛 알림 시 극한 회피만\n가능합니다.";
        case 2:
            return L"공동 내부의 이벤트는 선택에 따라 다른 결과를\n초래할 수 있으니 신중하게 판단하세요.";
        case 3:
            return L"파티의 스트레스 수치가 100에 도달할 때마다\n침식 증상 1개를 랜덤으로\n획득합니다.";
        case 4:
            return L"스코트의 전용 화면을 통해 공동 탐사 진행\n상황과 데이터 기록을 관리할 수 있습니다.";
        case 5:
            return L"스코트에서는 공동 탐사 관련 튜토리얼을 진행할\n수 있으며 기초 시스템을 익힐 수 있습니다.";
        }
    }
    // ---------------------------
    // 메인시티
    // ---------------------------
    else if (strNextLevelKey == "MainCity_Level")
    {
        index = Helper::Get_Random_Int(0, 4);

        switch (index)
        {
        case 0:
            return L"메인시티에서는 각종 상점과 시설을 이용할 수\n있습니다. 의뢰 수주 및 에이전트 육성을\n진행하세요.";
        case 1:
            return L"아우의 복권은 하루 한 번 무료로 이용할 수\n있으며 페니를 획득할 수 있습니다.";
        case 2:
            return L"라면 가게에서 식사를 하면 일정 시간 동안\n전투에 유리한 버프 효과를 받을 수 있습니다.";
        case 3:
            return L"모집을 통해 새로운 에이전트를 영입할 수\n있으며 높은 등급일수록 강력한 능력을 보유합니다.";
        case 4:
            return L"기간 한정 모집은 특정 에이전트의 등장 확률이\n상승하며 일정 횟수 이상 진행 시 확정 보상이\n있습니다.";
        }
    }

    // ---------------------------
    // 기본
    // ---------------------------
    return L"잠시만 기다려 주세요.\n데이터를 불러오는 중입니다.\n로딩이 곧 완료됩니다.";
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