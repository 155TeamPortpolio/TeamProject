#include "pch.h"
#include "UI_AnomalyGauge.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "Sprite2D.h"

HRESULT CUI_AnomalyGauge::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_AnomalyGauge::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("anomalyGauge_janeDoe.json")));
    Cache();

    return S_OK;
}

void CUI_AnomalyGauge::Awake()
{
    Change_State(TYPE::GAUGE, STATE::INACTIVE);
    Change_State(TYPE::INDICATOR, STATE::INACTIVE);
    Set_Gauge(m_fRatio);
}

void CUI_AnomalyGauge::Update(_float dt)
{
    if (InputDevice()->Key_Down('I'))
    {
        m_fRatio = min(1.f, m_fRatio + dt * 5.f);
        Set_Gauge(m_fRatio);
        Update_Gauge(m_fRatio);
    }

    if (InputDevice()->Key_Down('U'))
    {
        m_fRatio = max(0.f, m_fRatio - dt * 5.f);
        Set_Gauge(m_fRatio);
        Update_Gauge(m_fRatio);
    }

    if (InputDevice()->Key_Tap('Y'))
        Change_State(TYPE::INDICATOR, STATE::INACTIVE);

    __super::Update(dt);
}

void CUI_AnomalyGauge::UI_Active(void* pArg)
{ 
}

void CUI_AnomalyGauge::UI_DeActive(void* pArg)
{
}

void CUI_AnomalyGauge::Cache()
{
    auto pContainer = Get_Component<CObjectContainer>();

    for (_int i = 0; i < ENUM(CHILD::END); ++i)
    {
        auto pObj = pContainer->Find_Descendant(INSTANCENAMES[i]);
        if (!pObj)
            continue;

        m_pChildren[i] = dynamic_cast<CUI_Object*>(pObj);
    }
}

void CUI_AnomalyGauge::Update_Gauge(_float fRatio)
{
    if (m_fRatio >= 1.f)
    {
        Change_State(TYPE::INDICATOR, STATE::ACTIVE);
        Change_State(TYPE::GAUGE, STATE::ACTIVE);
    }
    else if (m_fRatio <= 0.f)
    {
        Change_State(TYPE::GAUGE, STATE::INACTIVE);
    }
}

void CUI_AnomalyGauge::Change_State(TYPE eType, STATE eState)
{
    _int iIndex = ENUM(eType);

    if (m_eState[iIndex] == eState)
        return;

    m_eState[iIndex] = eState;

    switch (eState)
    {
    case STATE::ACTIVE:
        Enter_Active(eType); 
        break;
    case STATE::INACTIVE:
        Enter_Inactive(eType);
        break;
    }
}

void CUI_AnomalyGauge::Enter_Active(TYPE eType)
{
    switch (eType)
    {
    case TYPE::GAUGE:
        Change_ChildTexture(CHILD::GAUGE, 1, "Color_UU_07.png");
        Set_ChildAnimation(CHILD::GAUGE_OVERLAY1, 0);
        Set_ChildAnimation(CHILD::GAUGE_OVERLAY2, 0);
        Set_ChildAnimation(CHILD::INDICATOR_OVERLAY, 0);
        break;
    case TYPE::INDICATOR:
        Set_ChildAlive(CHILD::INDICATOR, true);
        Set_ChildAnimation(CHILD::INDICATOR, 0); 
        break;
    }
}

void CUI_AnomalyGauge::Enter_Inactive(TYPE eType)
{
    switch (eType)
    {
    case TYPE::GAUGE:
        Change_ChildTexture(CHILD::GAUGE, 1, "Color_UU_01.png");
        break;
    case TYPE::INDICATOR:
        Set_ChildAnimation(CHILD::INDICATOR_OVERLAY, 0);
        Set_ChildAlive(CHILD::INDICATOR, false);
        break;
    }
}

void CUI_AnomalyGauge::Set_Gauge(_float fRatio)
{
    if (0.f > fRatio || fRatio > 1.f)
        return;

    auto pGauge = m_pChildren[ENUM(CHILD::GAUGE)];
    if (!pGauge)
        return;

    pGauge->Set_Scale({ fRatio, 1.f });
}

void CUI_AnomalyGauge::Set_ChildAlive(CHILD child, _bool isAlive)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    pChild->Set_Alive(isAlive);
}

void CUI_AnomalyGauge::Set_ChildAnimation(CHILD child, _int iIndex)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    pChild->Set_Animation(0);
}

void CUI_AnomalyGauge::Change_ChildTexture(CHILD child, _int iIndex, const string& strTextureKey)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    auto pSprite = pChild->Get_Component<CSprite2D>();
    if (!pSprite)
        return;

    pSprite->Change_Texture(iIndex, G_GlobalLevelKey, strTextureKey);
}

CGameObject* CUI_AnomalyGauge::Create()
{
    CUI_AnomalyGauge* pInstance = new CUI_AnomalyGauge();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_AnomalyGauge");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_AnomalyGauge::Clone(INIT_DESC* pArg)
{
    CUI_AnomalyGauge* pInstance = new CUI_AnomalyGauge(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_AnomalyGauge");
        Safe_Release(pInstance);
    }
    return pInstance;
}