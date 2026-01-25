#include "pch.h"
#include "UI_BossHUD.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "GaugeUI.h"
#include "TextSlot.h"

HRESULT CUI_BossHUD::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CObjectContainer>();

    return S_OK;
}

HRESULT CUI_BossHUD::Initialize(INIT_DESC* pArg)
{
    // 외부에서 전달받은 몬스터 정보 설정
    BOSS_HUD_DESC* pDesc = static_cast<BOSS_HUD_DESC*>(pArg);
    m_pMonsterStatus = pDesc->pMonsterStatus;

    __super::Initialize(pArg);

    Load_Json("hud_boss.json");
    Cache_Children();

    return S_OK;
}

void CUI_BossHUD::Awake()
{
    Set_Animation(0);
}

void CUI_BossHUD::Update(_float dt)
{
    __super::Update(dt);

    if (!m_pMonsterStatus)
        return;

    _float fRatio = m_pMonsterStatus->iNowHP / max(m_pMonsterStatus->iMaxHP, 1.f);

    // HP Front
    Set_GaugeFill(CHILD::GAUGE_HP_FRONT, fRatio);

    // HP Back
    Update_HPBackGauge(fRatio, dt);

    // Groggy
    Set_GaugeFill(CHILD::GAUGE_GROGGY, m_pMonsterStatus->iGroggyValue / m_fGroggyMax);
    Set_GroggyText(m_pMonsterStatus->iGroggyValue, 2);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_BossHUD::UI_Active(void* pArg)
{
    //if (!pArg)
    //    return;
    //
    //UI_TRANSITION_DESC* pDesc = static_cast<UI_TRANSITION_DESC*>(pArg);
    //if (pDesc->isFade)
    //    Set_Animation(0);
    //else
    Set_Alpha(1.f);
}

void CUI_BossHUD::UI_DeActive(void* pArg)
{
    Set_Alpha(0.f);
}

void CUI_BossHUD::Load_Json(const string& resourceKey)
{
    // JSON 기반 UI 구성 로드
    const string& filePath = ResourceManager()->Get_ResourcePath(resourceKey);
    Load(Helper::LoadJson<nlohmann::ordered_json>(filePath));
}

void CUI_BossHUD::Cache_Children()
{
    auto pContainer = Get_Component<CObjectContainer>();

    // 자식 UI 오브젝트 포인터를 배열에 캐싱
    for (_int i = 0; i < ENUM(CHILD::END); ++i)
    {
        const string& strInstanceName = INSTANCENAMES[i];
        if (strInstanceName.empty())
            continue;

        auto pObj = pContainer->Find_Descendant(strInstanceName);
        if (!pObj)
            continue;

        auto pUI = dynamic_cast<CUI_Object*>(pObj);
        if (!pUI)
            continue;

        m_pChildren[i] = pUI;

        if (auto pGauge = dynamic_cast<CGaugeUI*>(pUI))
            m_pGauges[i] = pGauge;
    }

    m_pGroggyText = m_pChildren[ENUM(CHILD::TEXT_GROGGY)]->Get_Component<CTextSlot>();
}

void CUI_BossHUD::Update_HPBackGauge(_float fRatio, _float dt)
{
    // HP 변화 감지
    if (fabs(fRatio - m_hpBack.fTargetRatio) > (HPBACK_DELTA / m_pMonsterStatus->iMaxHP))    // hp 변화량이 HPBACK_DELTA를 넘었을 때
    {
        m_hpBack.fTargetRatio = fRatio;
        m_hpBack.fDelayTimer = 0.f;
        m_hpBack.isDelay = true;
        m_isBlinking = true;
    }

    // Delay 처리
    if (m_hpBack.isDelay)
    {
        m_hpBack.fDelayTimer += dt;
        if (m_hpBack.fDelayTimer < m_hpBack.fDelayTime)
        {
            //Apply_Blink(fRatio, dt);  // 딜레이 때도 깜빡이게 할지 고민 중
            return;
        } 

        m_hpBack.isDelay = false; 
    }

    // 보간
    _float t = dt * HPBACK_LERP_SPEED;
    m_hpBack.fCurRatio = Math::Lerp(m_hpBack.fCurRatio, m_hpBack.fTargetRatio, t);
    Set_GaugeFill(CHILD::GAUGE_HP_BACK, m_hpBack.fCurRatio);

    // 깜빡임 종료 조건
    if (fabs(m_hpBack.fCurRatio - m_hpBack.fTargetRatio) < 0.001f)
    {
        m_hpBack.fCurRatio = m_hpBack.fTargetRatio;
        m_isBlinking = false;
        Set_ChildColor(CHILD::GAUGE_HP_BACK, UI_HPBACK_DARK);
    }

    // 깜빡임 적용
    if (m_isBlinking)
        Apply_Blink(fRatio, dt);
}

void CUI_BossHUD::Apply_Blink(_float fRatio, _float dt)
{
    _float fBlinkSpeed = Math::Lerp(BLINK_SPEED_MAX, BLINK_SPEED_MIN, fRatio);
    m_fBlinkAcc += dt * fBlinkSpeed;

    _float t = (sinf(m_fBlinkAcc) * 0.5f) + 0.5f; // 0 ~ 1
    Vector4 vColor = Vector4::Lerp(Vector4(UI_HPBACK_DARK), Vector4(UI_HPBACK_LIGHT), t);
    Set_ChildColor(CHILD::GAUGE_HP_BACK, vColor);
}

void CUI_BossHUD::Set_ChildColor(CHILD child, _float4 vColor)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    pChild->Set_Color(vColor);
}

void CUI_BossHUD::Set_GaugeFill(CHILD child, _float fFillAmount)
{
    auto pGauge = m_pGauges[ENUM(child)];
    if (!pGauge)
        return;

    pGauge->Set_FillAmount(fFillAmount);
}

void CUI_BossHUD::Set_GroggyText(int iNum, _int iWidth)
{
    if (!m_pGroggyText)
        return;
     
    wchar_t buf[32];
    Helper::Format_FixedZeroPad(buf, _countof(buf), iNum, iWidth);
    m_pGroggyText->Set_Text(buf);
}

CGameObject* CUI_BossHUD::Create()
{
    CUI_BossHUD* pInstance = new CUI_BossHUD();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_BossHUD");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_BossHUD::Clone(INIT_DESC* pArg)
{
    CUI_BossHUD* pInstance = new CUI_BossHUD(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_BossHUD");
        Safe_Release(pInstance);
    }
    return pInstance;
}