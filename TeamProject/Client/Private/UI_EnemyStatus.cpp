#include "pch.h"
#include "UI_EnemyStatus.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "GaugeUI.h"
#include "TextSlot.h"

HRESULT CUI_EnemyStatus::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CObjectContainer>();

    return S_OK;
}

HRESULT CUI_EnemyStatus::Initialize(INIT_DESC* pArg)
{
    ENEMYSTATUS_DESC* pDesc = static_cast<ENEMYSTATUS_DESC*>(pArg);
    m_pParentWorld = pDesc->pParentWorld;
    m_pBoneLocal = pDesc->pBoneLocal;
    m_pMonsterStatus = pDesc->pMonsterStatus;

    __super::Initialize(pArg);

    auto pResourceMgr = CGameInstance::GetInstance()->GetInstance()->Get_ResourceMgr();
    const string& filePath = pResourceMgr->Get_ResourcePath("enemy_status.json");
    Load(Helper::LoadJson<nlohmann::ordered_json>(filePath));

    for (_int i = 0; i < ENUM(Child::END); ++i)
        m_handles[i] = Get_DescendantHandle(INSTANCENAMES[i]);

    return S_OK;
}

void CUI_EnemyStatus::Update(_float dt)
{
    __super::Update(dt);

    Set_WorldPosition();

    //Set_Gauge(Child::HP_GUAGE, fFillAmount);
    //Set_Gauge(Child::GROGGY_GAUGE, fFillAmount);  // 그로기 맥스는 무조건 100
    //Set_GroggyText(5);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_EnemyStatus::Set_WorldPosition()
{
    if (!m_pParentWorld || !m_pBoneLocal)
        return;

    Matrix matWorld = *m_pBoneLocal * *m_pParentWorld;
    Update_WorldToScreen(matWorld.Translation());
}

void CUI_EnemyStatus::Set_GroggyText(_int iGroggy)
{
    ForChild(Child::GROGGY_TEXT, [&](CUI_Object* ui) {
        auto pTextSlot = ui->Get_Component<CTextSlot>();
        if (!pTextSlot)
            return;

        wchar_t buf[32];
        swprintf_s(buf, _countof(buf), L"%02d", iGroggy % 100);
        pTextSlot->Set_Text(buf);

        _float4 vColor = (iGroggy == 0) ? UI_GRAY_LIGHT : _float4(0.9960f, 0.6549f, 0.0039f, 1.f);
        pTextSlot->Set_Color(vColor);
        });
}

void CUI_EnemyStatus::Set_Gauge(Child child, _float fFillAmount)
{
    ForChild(child, [&](CUI_Object* ui) {
        auto pGauge = dynamic_cast<CGaugeUI*>(ui);
        if (!pGauge)
            return;

        pGauge->Set_FillAmount(fFillAmount);
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