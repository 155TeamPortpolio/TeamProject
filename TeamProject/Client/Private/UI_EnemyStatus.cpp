#include "pch.h"
#include "UI_EnemyStatus.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "EventListener.h"

#include "GaugeUI.h"
#include "TextSlot.h"

HRESULT CUI_EnemyStatus::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CObjectContainer>();
    Add_Component<CEventListener>();

    return S_OK;
}

HRESULT CUI_EnemyStatus::Initialize(INIT_DESC* pArg)
{
    // 외부에서 전달받은 몬스터/트랜스폼 정보 설정
    ENEMYSTATUS_DESC* pDesc = static_cast<ENEMYSTATUS_DESC*>(pArg);
    m_pParentWorld = pDesc->pParentWorld;
    m_pBoneLocal = pDesc->pBoneLocal;
    m_pMonsterStatus = pDesc->pMonsterStatus;
    m_tOwnerHandle = pDesc->tOwnerHandle;

    __super::Initialize(pArg);

    // JSON 기반 UI 구성 로드
    const string& filePath = ResourceManager()->Get_ResourcePath("enemy_status.json");
    Load(Helper::LoadJson<nlohmann::ordered_json>(filePath));

    // 자식 UI 핸들 캐싱
    for (_int i = 0; i < ENUM(Child::END); ++i)
        m_handles[i] = Get_DescendantHandle(INSTANCENAMES[i]);

    // 타겟 락온 이벤트 등록
    Get_Component<CEventListener>()->Add_Listener<TARGET_LOCK_DESC>([&](TARGET_LOCK_DESC desc)
        {
            Set_TargetLock(desc);
        });

    return S_OK;
}

void CUI_EnemyStatus::Update(_float dt)
{
    __super::Update(dt);

    // 몬스터 본 위치 기준으로 UI 위치 갱신
    Set_WorldPosition();

    Update_HPBackGauge(dt);

    // HP / Groggy 게이지 갱신
    _float fRatio = m_pMonsterStatus->iNowHP / max(m_pMonsterStatus->iMaxHP, 1.f);
    Set_GaugeFill(Child::GAUGE_HP_FRONT, m_pMonsterStatus->iNowHP / max(m_pMonsterStatus->iMaxHP, 1.f));
    if (fabs(fRatio - m_hpBack.fTargetRatio) > (HPBACK_DELTA / m_pMonsterStatus->iMaxHP))    // hp 변화량이 HPBACK_DELTA를 넘었을 때
    {
        m_hpBack.fTargetRatio = fRatio;
        m_hpBack.fDelayTimer = 0.f;
        m_hpBack.isDelay = true;
        Set_Color(Child::GAUGE_HP_BACK, UI_HPBACK_DARK);
    }

    Set_GaugeFill(Child::GAUGE_GROGGY, m_pMonsterStatus->iGroggyValue / m_fGroggyMax);

    // Groggy 텍스트 갱신
    Set_GroggyText(m_pMonsterStatus->iGroggyValue);

    // 모든 하위 UI 업데이트
    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_EnemyStatus::Set_TargetLock(TARGET_LOCK_DESC& desc)
{
    // 다른 몬스터에 대한 이벤트면 락온 비활성화
    if (m_tOwnerHandle != desc.tHandle)
    {
        Set_Alive(Child::LOCKON, false);
        return;
    }

    // 락온 UI 활성/비활성 처리
    const _bool bLock = desc.bLock;
    Set_Alive(Child::LOCKON, bLock);
    // 락온 시작 시 애니메이션 재생
    if (bLock)
        Set_Animation(Child::LOCKON, 0);
}

void CUI_EnemyStatus::Set_WorldPosition()
{
    if (!m_pParentWorld || !m_pBoneLocal)
        return;

    Matrix matWorld = *m_pBoneLocal * *m_pParentWorld;
    Update_WorldToScreen(matWorld.Translation());
}

void CUI_EnemyStatus::Update_HPBackGauge(_float dt)
{
    if (m_hpBack.isDelay)
    {
        m_hpBack.fDelayTimer += dt;

        if (m_hpBack.fDelayTimer < m_hpBack.fDelayTime)
            return;

        m_hpBack.isDelay = false;
        Set_Color(Child::GAUGE_HP_BACK, UI_HPBACK_LIGHT);
    }

    _float t = dt * HPBACK_LERP_SPEED;
    m_hpBack.fCurRatio = Math::Lerp(m_hpBack.fCurRatio, m_hpBack.fTargetRatio, t);
    Set_GaugeFill(Child::GAUGE_HP_BACK, m_hpBack.fCurRatio);
}

void CUI_EnemyStatus::Set_Alive(Child child, _bool isAlive)
{
    ForChild(child, [isAlive](CUI_Object* ui) {
        ui->Set_Alive(isAlive);
        });
}

void CUI_EnemyStatus::Set_Color(Child child, _float4 vColor)
{
    ForChild(child, [vColor](CUI_Object* ui) {
        ui->Set_Color(vColor);
        });
}

void CUI_EnemyStatus::Set_Animation(Child child, _int iIndex)
{
    ForChild(child, [iIndex](CUI_Object* ui) {
        ui->Set_Animation(iIndex);
        });
}

void CUI_EnemyStatus::Set_GaugeFill(Child child, _float fFillAmount)
{
    ForChild(child, [&](CUI_Object* ui) {
        auto pGauge = dynamic_cast<CGaugeUI*>(ui);
        if (!pGauge)
            return;

        pGauge->Set_FillAmount(fFillAmount);
        });
}

void CUI_EnemyStatus::Set_GroggyText(_int iGroggy)
{
    ForChild(Child::GROGGY_TEXT, [&](CUI_Object* ui) {
        auto pTextSlot = ui->Get_Component<CTextSlot>();
        if (!pTextSlot)
            return;

        wchar_t buf[32];
        Helper::Format_FixedZeroPad(buf, _countof(buf), iGroggy, 2);
        pTextSlot->Set_Text(buf);

        _float4 vColor = (iGroggy == 0) ? UI_GRAY_LIGHT : _float4(0.9960f, 0.6627f, 0.f, 1.f);
        pTextSlot->Set_Color(vColor);
        });
}

CGameObject* CUI_EnemyStatus::Create()
{
    CUI_EnemyStatus* pInstance = new CUI_EnemyStatus();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_EnemyStatus");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_EnemyStatus::Clone(INIT_DESC* pArg)
{
    CUI_EnemyStatus* pInstance = new CUI_EnemyStatus(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_EnemyStatus");
        Safe_Release(pInstance);
    }
    return pInstance;
}