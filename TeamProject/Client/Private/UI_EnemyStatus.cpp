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

    Load_Json("enemy_status.json");
    Cache_Children();

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
     
    _float fRatio = m_pMonsterStatus->iNowHP / max(m_pMonsterStatus->iMaxHP, 1.f);

    // HP Front
    Set_GaugeFill(CHILD::GAUGE_HP_FRONT, m_pMonsterStatus->iNowHP / max(m_pMonsterStatus->iMaxHP, 1.f));

    // HP Back
    Update_HPBackGauge(fRatio, dt);

    // Groggy
    Set_GaugeFill(CHILD::GAUGE_GROGGY, m_pMonsterStatus->iGroggyValue / m_fGroggyMax);
    Set_GroggyText(m_pMonsterStatus->iGroggyValue, 2);

    // 모든 하위 UI 업데이트
    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_EnemyStatus::Load_Json(const string& resourceKey)
{
    // JSON 기반 UI 구성 로드
    const string& filePath = ResourceManager()->Get_ResourcePath(resourceKey);
    Load(Helper::LoadJson<nlohmann::ordered_json>(filePath));
}

void CUI_EnemyStatus::Cache_Children()
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

void CUI_EnemyStatus::Set_TargetLock(TARGET_LOCK_DESC& desc)
{
    // 다른 몬스터에 대한 이벤트면 락온 비활성화
    if (m_tOwnerHandle != desc.tHandle)
    {
        Set_ChildAlive(CHILD::LOCKON, false);
        return;
    }

    // 락온 UI 활성/비활성 처리
    const _bool bLock = desc.bLock;
    Set_ChildAlive(CHILD::LOCKON, bLock);
    // 락온 시작 시 애니메이션 재생
    if (bLock)
        Set_ChildAnimation(CHILD::LOCKON, 0);
}

void CUI_EnemyStatus::Set_WorldPosition()
{
    if (!m_pParentWorld || !m_pBoneLocal)
        return;

    Matrix matWorld = *m_pBoneLocal * *m_pParentWorld;
    Update_WorldToScreen(matWorld.Translation());
}

void CUI_EnemyStatus::Update_HPBackGauge(_float fRatio, _float dt)
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

void CUI_EnemyStatus::Apply_Blink(_float fRatio, _float dt)
{
    _float fBlinkSpeed = Math::Lerp(BLINK_SPEED_MAX, BLINK_SPEED_MIN, fRatio);
    m_fBlinkAcc += dt * fBlinkSpeed;

    _float t = (sinf(m_fBlinkAcc) * 0.5f) + 0.5f; // 0 ~ 1
    Vector4 vColor = Vector4::Lerp(Vector4(UI_HPBACK_DARK), Vector4(UI_HPBACK_LIGHT), t);
    Set_ChildColor(CHILD::GAUGE_HP_BACK, vColor);
}

void CUI_EnemyStatus::Set_ChildAlive(CHILD child, _bool isAlive)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;
    
    pChild->Set_Alive(isAlive);
}

void CUI_EnemyStatus::Set_ChildColor(CHILD child, _float4 vColor)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    pChild->Set_Color(vColor);
}

void CUI_EnemyStatus::Set_ChildAnimation(CHILD child, _int iIndex)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    pChild->Set_Animation(iIndex);
}

void CUI_EnemyStatus::Set_GaugeFill(CHILD child, _float fFillAmount)
{
    auto pGauge = m_pGauges[ENUM(child)];
    if (!pGauge)
        return;

    pGauge->Set_FillAmount(fFillAmount);
}

void CUI_EnemyStatus::Set_GroggyText(_int iNum, _int iWidth)
{
    if (!m_pGroggyText)
        return;

    wchar_t buf[32];
    Helper::Format_FixedZeroPad(buf, _countof(buf), iNum, iWidth);
    m_pGroggyText->Set_Text(buf);
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