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

    // JSON 기반 UI 구성 로드
    const string& filePath = ResourceManager()->Get_ResourcePath("hud_boss.json");
    Load(Helper::LoadJson<nlohmann::ordered_json>(filePath));

    // 자식 UI 핸들 캐싱
    for (_int i = 0; i < ENUM(Child::END); ++i)
        m_handles[i] = Get_DescendantHandle(INSTANCENAMES[i]);

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
    Set_GaugeFill(Child::GAUGE_HP_FRONT, fRatio);

    // HP Back
    Update_HPBackGauge(fRatio, dt);

    // Groggy
    Set_GaugeFill(Child::GAUGE_GROGGY, m_pMonsterStatus->iGroggyValue / m_fGroggyMax);
    Set_NumberText(Child::TEXT_GROGGY, m_pMonsterStatus->iGroggyValue, 2);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
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
    Set_GaugeFill(Child::GAUGE_HP_BACK, m_hpBack.fCurRatio);

    // 깜빡임 종료 조건
    if (fabs(m_hpBack.fCurRatio - m_hpBack.fTargetRatio) < 0.001f)
    {
        m_hpBack.fCurRatio = m_hpBack.fTargetRatio;
        m_isBlinking = false;
        Set_Color(Child::GAUGE_HP_BACK, UI_HPBACK_DARK);
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
    Set_Color(Child::GAUGE_HP_BACK, vColor);
}

void CUI_BossHUD::Set_Color(Child child, _float4 vColor)
{
    ForChild(child, [vColor](CUI_Object* ui) {
        ui->Set_Color(vColor);
        });
}

void CUI_BossHUD::Set_GaugeFill(Child child, _float fFillAmount)
{
    ForChild(child, [fFillAmount](CUI_Object* ui) {
        auto pGauge = dynamic_cast<CGaugeUI*>(ui);
        if (!pGauge)
            return;

        pGauge->Set_FillAmount(fFillAmount);
        });
}

void CUI_BossHUD::Set_NumberText(Child child, _int iNum, _int iWidth)
{
    ForChild(child, [iNum, iWidth](CUI_Object* ui) {
        auto pTextSlot = ui->Get_Component<CTextSlot>();
        if (!pTextSlot)
            return;

        wchar_t buf[32];
        Helper::Format_FixedZeroPad(buf, _countof(buf), iNum, iWidth);
        pTextSlot->Set_Text(buf);
        });
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