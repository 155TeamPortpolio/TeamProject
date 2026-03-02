#include "pch.h"
#include "UI_SwitchGauge.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

void CUI_SwitchGauge::Set_Gauge(_float fRatio)
{
    _bool isVisible = fRatio > 0.f;

    for (_int i = 0; i < ENUM(GAUGE::END); ++i)
        if (auto pGauge = m_pGauges[i])
        {
            pGauge->Set_Alive(isVisible);
            pGauge->Set_Size(_float2(m_vSize.x * fRatio, m_vSize.y));
        } 
}

HRESULT CUI_SwitchGauge::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    Add_Component<CObjectContainer>();

    return S_OK;
}

HRESULT CUI_SwitchGauge::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("switch_gauge.json")));
    Cache();

    return S_OK;
}

void CUI_SwitchGauge::Awake()
{
}

void CUI_SwitchGauge::Update(_float dt)
{
    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_SwitchGauge::UI_Active(void* pArg)
{
    Set_Animation(0);
    for (auto& pChild : Get_Component<CObjectContainer>()->Get_Children())
        if (auto pUI = dynamic_cast<CUI_Object*>(pChild))
            pUI->Set_Animation(0);
}

void CUI_SwitchGauge::UI_DeActive(void* pArg)
{
}

void CUI_SwitchGauge::Cache()
{
    for (_int i = 0; i < ENUM(GAUGE::END); ++i)
        m_pGauges[i] = dynamic_cast<CUI_Object*>(Get_Component<CObjectContainer>()->Find_Descendant(INSTANCENAMES[i]));
}

CGameObject* CUI_SwitchGauge::Create()
{
    CUI_SwitchGauge* pInstance = new CUI_SwitchGauge();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_SwitchGauge");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_SwitchGauge::Clone(INIT_DESC* pArg)
{
    CUI_SwitchGauge* pInstance = new CUI_SwitchGauge(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_SwitchGauge");
        Safe_Release(pInstance);
    }
    return pInstance;
}