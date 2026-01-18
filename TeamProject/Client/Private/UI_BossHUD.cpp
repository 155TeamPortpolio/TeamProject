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
}

void CUI_BossHUD::Update(_float dt)
{
    if (!m_pMonsterStatus)
        return;

    //Set_GaugeFill(Child::GAUGE_HP_BACK, 0.7f);
    Set_GaugeFill(Child::GAUGE_HP_FRONT, m_pMonsterStatus->iNowHP / max(m_pMonsterStatus->iMaxHP, 1.f));
    Set_GaugeFill(Child::GAUGE_GROGGY, m_pMonsterStatus->iGroggyValue / m_fGroggyMax);
    Set_NumberText(Child::TEXT_GROGGY, m_pMonsterStatus->iGroggyValue, 2);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
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