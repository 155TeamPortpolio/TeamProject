#include "pch.h"
#include "UI_BattleHUD.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "EventListener.h"
#include "TextSlot.h"
#include "Sprite2D.h"
#include "GaugeUI.h" 

HRESULT CUI_BattleHUD::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CObjectContainer>();
    Add_Component<CEventListener>();

    return S_OK;
}

HRESULT CUI_BattleHUD::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    // JSON 기반 UI 구성 로드
    const string& filePath = ResourceManager()->Get_ResourcePath("hud_battle.json");
    Load(Helper::LoadJson<nlohmann::ordered_json>(filePath));

    // JSON으로 로드해서 만든 자식들 캐싱
    Cache_Children();

    // 클라이언트에서 만든 ui 자식으로 추가 (추가하면서 캐싱함)
    const string& strLevelKey = LevelManager()->Get_NowLevelKey();
    Add_PartObject(strLevelKey, "Proto_GameObject_Decibel", "decibel", CHILD::ULTIMATE1, _float2(50.f, 136.f));
    Add_PartObject(strLevelKey, "Proto_GameObject_BattleHUDAction", "action", CHILD::ACTION, _float2(1178.f, 655.f));
     
    // 이벤트 : UI_PLAYER_STATUS_DESC
    Get_Component<CEventListener>()->Add_Listener<UI_PLAYER_STATUS_DESC>([&](const UI_PLAYER_STATUS_DESC& desc)
        {
            Set_Values(desc);
        });

    //UI_DeActive();   // 나중에 battleSystem에서 보이게 제어할 때 처음에는 알파 0으로 하는게 좋을 듯

    return S_OK;
}

void CUI_BattleHUD::Awake()
{
    Set_Alpha(0.f);
}

void CUI_BattleHUD::Update(_float dt)
{
    __super::Update(dt);

    Update_HPBackGauge(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_BattleHUD::UI_Active(void* pArg)
{
    if (!pArg)
        return;

    SHOW_DESC* pDesc = static_cast<SHOW_DESC*>(pArg);
    if (pDesc->isFade)
        Set_Animation(0);
    else
        Set_Alpha(1.f);
}

void CUI_BattleHUD::UI_DeActive(void* pArg)
{
    Set_Alpha(0.f);
}

void CUI_BattleHUD::Add_PartObject(const string& strLevelKey, const string& strPrototypeTag, const string& strInstanceName, CHILD child, _float2 vOffset)
{
    CUI_Object* pUI = Builder::Create_UIObject({ strLevelKey, strPrototypeTag })
        .Offset(vOffset)
        .Build(strInstanceName);

    if (!pUI)
        return;

    Get_Component<CObjectContainer>()->Add_Child(pUI);
    m_pChildren[child] = pUI;

    m_pSprites[child] = pUI->Get_Component<CSprite2D>();
    m_ptextSlots[child] = pUI->Get_Component<CTextSlot>();
    m_pGauges[child] = dynamic_cast<CGaugeUI*>(pUI);
}

void CUI_BattleHUD::Cache_Children()
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
        m_pChildren[i] = pUI;

        m_pSprites[i] = pUI->Get_Component<CSprite2D>();
        m_ptextSlots[i] = pUI->Get_Component<CTextSlot>();
        m_pGauges[i] = dynamic_cast<CGaugeUI*>(pUI);
    } 
}

void CUI_BattleHUD::Set_Values(UI_PLAYER_STATUS_DESC desc)
{
    const _uint iIndex = ENUM(desc.eOwner);

    // Icon
    Change_ChildTexture(ICON_CHILD[iIndex], ICONTEXTURES[ENUM(desc.eCharacter)]);

    // HP
    _float fRatio = desc.hp.fCurValue / desc.hp.fMaxValue;
    Set_GaugeFill(HPFRONT_CHILD[iIndex], fRatio);

    // HP Back
    auto& hpBack = m_hpBack[iIndex];
    if (fabs(fRatio - hpBack.fTargetRatio) > (HPBACK_DELTA / desc.hp.fMaxValue))    // hp 변화량이 HPBACK_DELTA를 넘었을 때
    {
        hpBack.fTargetRatio = fRatio;
        hpBack.fDelayTimer = 0.f;
        hpBack.isDelay = true;
    }

    // Special
    _float fSpecialRatio = desc.special.fCurValue / desc.special.fMaxValue;
    _float fSpecialThresRatio = desc.specialThreshold / desc.special.fMaxValue;
    Set_GaugeFill(SPECIAL_CHILD[iIndex], fSpecialRatio);
    Set_Special(iIndex, fSpecialRatio, fSpecialThresRatio);

    // Ultimate
    _float fUltimateRatio = desc.ultimate.fCurValue / desc.ultimate.fMaxValue;
    Set_GaugeFill(ULTIMATE_CHILD[iIndex], fUltimateRatio);
    Set_UltimateIcon(iIndex, fUltimateRatio);

    // HP Text
    if (desc.eOwner == UI_STATUS_OWNER::ROLE1)
    {
        Set_NumberText(CHILD::CUR_HP_TEXT, static_cast<_int>(desc.hp.fCurValue), m_iPlayerHPWidth);
        Set_NumberText(CHILD::MAX_HP_TEXT, static_cast<_int>(desc.hp.fMaxValue), m_iPlayerHPWidth);
    }
}

void CUI_BattleHUD::Set_Special(_int iIndex, _float fRatio, _float fThresRatio)
{
    if (0 > iIndex || iIndex > 2)
        return;

    // special gauge, 기준점 색깔 변경
    if (fRatio >= fThresRatio)
    {
        Set_ChildColor(SPECIAL_CHILD[iIndex], Helper::HexToColor("#FBC3D6"));
        Set_ChildColor(SPECIALARROW_CHILD[iIndex], Helper::HexToColor("#FF0607"));
    }
    else
    {
        Set_ChildColor(SPECIAL_CHILD[iIndex], UI_GRAY_LIGHTEST);
        Set_ChildColor(SPECIALARROW_CHILD[iIndex], UI_GRAY_LIGHTEST);
    }

    // special 기준점 위치 변경
    Set_ChildAnchorOffsetX(SPECIALARROW_CHILD[iIndex], fThresRatio * SPECIAL_THRESHOLD[iIndex]);
}

void CUI_BattleHUD::Set_UltimateIcon(_int iIndex, _float fRatio)
{
    //if (0 >= iIndex || iIndex > 2)
    //    return;

    _bool isAlive = (fRatio >= 1.f);
    if (isAlive && !Is_ChildAlive(ULTIMATEICON_CHILD[iIndex]))
    {
        Set_ChildAlive(ULTIMATEICON_CHILD[iIndex], true);
        Set_ChildAnimation(ULTIMATEICON_CHILD[iIndex], 0); 
    }
    else if(!isAlive && Is_ChildAlive(ULTIMATEICON_CHILD[iIndex]))
        Set_ChildAlive(ULTIMATEICON_CHILD[iIndex], false);
}

void CUI_BattleHUD::Update_HPBackGauge(_float dt)
{
    for (_int i = 0; i < ROLE_COUNT; ++i)
    {
        auto& hpBack = m_hpBack[i];
        if (hpBack.isDelay)
        {
            hpBack.fDelayTimer += dt;

            if (hpBack.fDelayTimer < hpBack.fDelayTime)
                continue;

            hpBack.isDelay = false;
        }

        _float t = dt * HPBACK_LERP_SPEED;
        hpBack.fCurRatio = Math::Lerp(hpBack.fCurRatio, hpBack.fTargetRatio, t);
        Set_GaugeFill(HPBACK_CHILD[i], hpBack.fCurRatio);
    }
}

_bool CUI_BattleHUD::Is_ChildAlive(CHILD child)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return false;

    return pChild->Is_Alive();
}

void CUI_BattleHUD::Set_ChildAlive(CHILD child, _bool isAlive)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    pChild->Set_Alive(isAlive);
}

void CUI_BattleHUD::Set_ChildColor(CHILD child, _float4 vColor)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    pChild->Set_Color(vColor);
}

void CUI_BattleHUD::Set_ChildAnimation(CHILD child, _int iIndex)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    pChild->Set_Animation(iIndex);
}

void CUI_BattleHUD::Set_ChildAnchorOffsetX(CHILD child, _float fOffsetX)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    pChild->Set_AnchorOffsetX(fOffsetX);
}

void CUI_BattleHUD::Change_ChildTexture(CHILD child, const string& strTextureKey)
{
    auto pSprite = m_pSprites[ENUM(child)];
    if (!pSprite)
        return;

    pSprite->Change_Texture(0, G_GlobalLevelKey, strTextureKey);
}

void CUI_BattleHUD::Set_GaugeFill(CHILD child, _float fFillAmount)
{
    auto pGauge = m_pGauges[ENUM(child)];
    if (!pGauge)
        return;

    pGauge->Set_FillAmount(fFillAmount);
}

void CUI_BattleHUD::Set_NumberText(CHILD child, _int iNum, _int iWidth)
{
    auto pTextSlot = m_ptextSlots[ENUM(child)];
    if (!pTextSlot)
        return;

    wchar_t buf[32];
    Helper::Format_FixedZeroPad(buf, _countof(buf), iNum, iWidth);
    pTextSlot->Set_Text(buf);
}

CGameObject* CUI_BattleHUD::Create()
{
    CUI_BattleHUD* pInstance = new CUI_BattleHUD();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_BattleHUD");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_BattleHUD::Clone(INIT_DESC* pArg)
{
    CUI_BattleHUD* pInstance = new CUI_BattleHUD(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_BattleHUD");
        Safe_Release(pInstance);
    }
    return pInstance;
}