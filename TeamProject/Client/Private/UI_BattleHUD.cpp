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

    Add_Component<CEventListener>();

    return S_OK;
}

HRESULT CUI_BattleHUD::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    m_handles.resize(Child::END);

    auto pRoot = Ready_Prefab();
    if (!pRoot)
        MSG_BOX("Failed to Ready_Prefab : CUI_BattleHUD");

    // 이벤트 : UI_PLAYER_STATUS_DESC
    Get_Component<CEventListener>()->Add_Listener<UI_PLAYER_STATUS_DESC>([&](const UI_PLAYER_STATUS_DESC& desc)
        {
            Set_Values(desc);
        });

    // 이벤트 : UI_BOSS_STATUS_DESC
    Get_Component<CEventListener>()->Add_Listener<UI_BOSS_STATUS_DESC>([&](const UI_BOSS_STATUS_DESC& desc)
        {
            Set_Values(desc);
        });

    return S_OK;
}

void CUI_BattleHUD::Awake()
{
    // 루트 UI의 0번 애니메이션 재생 (FadeIn)
    if (m_hRoot.isValid())
        m_hRoot.Get()->Set_Animation(0);

    Set_Alive(Child::BOSS, true);
}

void CUI_BattleHUD::Update(_float dt)
{
    // boss몬스터 이벤트 테스트
    if (InputDevice()->Key_Down('P'))
    {
        UI_BOSS_STATUS_DESC desc = {};
        desc.hp.fCurValue = 50.f;
        desc.hp.fMaxValue = 100.f;
        desc.iGroggy = 5;
        EventSystem()->Broadcast<UI_BOSS_STATUS_DESC>({ desc });
    }
}

CUI_Object* CUI_BattleHUD::Ready_Prefab()
{
    auto pGameInstance = CGameInstance::GetInstance();
    const string& strLevelKey = pGameInstance->Get_LevelMgr()->Get_NowLevelKey();

    // Battle HUD 프리팹 (json) 로드 후 UI 트리 루트(CanvasPanel) 생성
    CUI_Object* pRoot = Builder::Create_UIObject({ strLevelKey, "Proto_GameObject_CanvasPanel" })
        .Asset("hud_battle.json")
        .Build("prefab");

    if (!pRoot)
        return nullptr;

    Add_PartObject(pRoot, strLevelKey, "Proto_GameObject_Decibel", "decibel", Child::ULTIMATE1, _float2(50.f, 136.f));
    Add_PartObject(pRoot, strLevelKey, "Proto_GameObject_BattleHUDAction", "action", Child::ACTION, _float2(1178.f, 655.f));

    // 생성된 루트 UI를 uiMgr에 등록
    if (FAILED(pGameInstance->Get_UIMgr()->Add_UIObject(pRoot, strLevelKey)))
        return nullptr;

    // UI 트리 기준으로 주요 핸들 캐싱 (root / chidlren)
    Cache_Handles(pRoot);

    return pRoot;
}

void CUI_BattleHUD::Add_PartObject(CUI_Object* pRoot, const string& strLevelKey, const string& strPrototypeTag, const string& strInstanceName, Child child, _float2 vOffset)
{
    CUI_Object* pObj = Builder::Create_UIObject({ strLevelKey, strPrototypeTag })
        .Offset(vOffset)
        .Build(strInstanceName);

    if (!pObj)
        return;

    pRoot->Get_Component<CObjectContainer>()->Add_Child(pObj);

    UI_HANDLE handle = pObj->Get_Handle();
    if (!handle.isValid())
        return;

    m_handles[child] = handle;
}

void CUI_BattleHUD::Cache_Handles(CUI_Object* pRoot)
{
    // 루트 핸들 캐싱
    m_hRoot = pRoot->Get_Handle();

    // 자식 핸들 캐싱
    for (_int i = 0; i < 3; ++i)
    {
        m_handles[ROLE_CHILD[i]] = pRoot->Get_DescendantHandle("role" + to_string(i + 1));
        m_handles[ICON_CHILD[i]] = pRoot->Get_DescendantHandle("icon" + to_string(i + 1));
        m_handles[HPBACK_CHILD[i]] = pRoot->Get_DescendantHandle("hpBack" + to_string(i + 1));
        m_handles[HPFRONT_CHILD[i]] = pRoot->Get_DescendantHandle("hpFront" + to_string(i + 1));
        m_handles[SPECIAL_CHILD[i]] = pRoot->Get_DescendantHandle("special" + to_string(i + 1));
        m_handles[SPECIALARROW_CHILD[i]] = pRoot->Get_DescendantHandle("specialArrow" + to_string(i + 1));
        m_handles[ULTIMATE_CHILD[i]] = pRoot->Get_DescendantHandle("ultimate" + to_string(i + 1));
        m_handles[ULTIMATEICON_CHILD[i]] = pRoot->Get_DescendantHandle("ultimateIcon" + to_string(i + 1));
    }

    m_handles[Child::CUR_HP_TEXT] = pRoot->Get_DescendantHandle("curHpText");
    m_handles[Child::MAX_HP_TEXT] = pRoot->Get_DescendantHandle("maxHpText");

    m_handles[Child::BOSS] = pRoot->Get_DescendantHandle("boss");
    m_handles[Child::BOSS_ICON] = pRoot->Get_DescendantHandle("bossIcon");
    m_handles[Child::BOSS_HP_BACK] = pRoot->Get_DescendantHandle("bossHpBack");
    m_handles[Child::BOSS_HP_FRONT] = pRoot->Get_DescendantHandle("bossHpFront");
    m_handles[Child::BOSS_GROGGY] = pRoot->Get_DescendantHandle("bossGroggy");
    m_handles[Child::BOSS_GROGGY_TEXT] = pRoot->Get_DescendantHandle("bossGroggyText");
}

void CUI_BattleHUD::Set_Values(UI_PLAYER_STATUS_DESC desc)
{
    const _uint iIndex = ENUM(desc.eOwner);

    // Icon
    Set_Texture(ICON_CHILD[iIndex], ICONTEXTURES[ENUM(desc.eCharacter)]);

    // HP
    Set_GaugeFill(HPFRONT_CHILD[iIndex], desc.hp.fCurValue / desc.hp.fMaxValue);

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
        Set_NumberText(Child::CUR_HP_TEXT, static_cast<_int>(desc.hp.fCurValue), m_iPlayerHPWidth);
        Set_NumberText(Child::MAX_HP_TEXT, static_cast<_int>(desc.hp.fMaxValue), m_iPlayerHPWidth);
    }
}

void CUI_BattleHUD::Set_Values(UI_BOSS_STATUS_DESC desc)
{ 
    Set_GaugeFill(Child::BOSS_HP_FRONT, desc.hp.fCurValue / max(desc.hp.fMaxValue, 1.f));
    Set_GaugeFill(Child::BOSS_GROGGY, desc.iGroggy / 100.f);
    Set_NumberText(Child::BOSS_GROGGY_TEXT, desc.iGroggy, 2);
}

void CUI_BattleHUD::Set_Special(_int iIndex, _float fRatio, _float fThresRatio)
{
    if (0 > iIndex || iIndex > 2)
        return;

    // special gauge, 기준점 색깔 변경
    if (fRatio >= fThresRatio)
    {
        Set_Color(SPECIAL_CHILD[iIndex], Helper::HexToColor("#FBC3D6"));
        Set_Color(SPECIALARROW_CHILD[iIndex], Helper::HexToColor("#FF0607"));
    }
    else
    {
        Set_Color(SPECIAL_CHILD[iIndex], UI_GRAY_LIGHTEST);
        Set_Color(SPECIALARROW_CHILD[iIndex], UI_GRAY_LIGHTEST);
    }

    // special 기준점 위치 변경
    ForChild(SPECIALARROW_CHILD[iIndex], [&](CUI_Object* ui) {
        ui->Set_AnchorOffsetX(fThresRatio * SPECIAL_THRESHOLD[iIndex]);
        });
}

void CUI_BattleHUD::Set_UltimateIcon(_int iIndex, _float fRatio)
{
    if (0 >= iIndex || iIndex > 2)
        return;

    _bool isAlive = (fRatio >= 1.f);
    if (isAlive && !Is_Alive(ULTIMATEICON_CHILD[iIndex]))
    {
        Set_Alive(ULTIMATEICON_CHILD[iIndex], true);
        Set_Animation(ULTIMATEICON_CHILD[iIndex], 0); 
    }
    else if(!isAlive && Is_Alive(ULTIMATEICON_CHILD[iIndex]))
        Set_Alive(ULTIMATEICON_CHILD[iIndex], false);
}

_bool CUI_BattleHUD::Is_Alive(Child child)
{
    _bool isAlive = {};

    ForChild(child, [&isAlive](CUI_Object* ui) {
        isAlive = ui->Is_Alive();
        });
    return isAlive;
}

void CUI_BattleHUD::Set_Alive(Child child, _bool isAlive)
{
    ForChild(child, [isAlive](CUI_Object* ui) {
        ui->Set_Alive(isAlive);
        });
}

void CUI_BattleHUD::Set_Color(Child child, _float4 vColor)
{
    ForChild(child, [vColor](CUI_Object* ui) {
        ui->Set_Color(vColor);
        });
}

void CUI_BattleHUD::Set_Animation(Child child, _int iIndex)
{
    ForChild(child, [iIndex](CUI_Object* ui) {
        ui->Set_Animation(iIndex);
        });
}

void CUI_BattleHUD::Set_Texture(Child child, const string& strTextureKey)
{
    ForChild(child, [&](CUI_Object* ui) {
        auto pSprite = ui->Get_Component<CSprite2D>();
        if (!pSprite)
            return;

        pSprite->Change_Texture(0, G_GlobalLevelKey, strTextureKey);
        });
}

void CUI_BattleHUD::Set_GaugeFill(Child child, _float fFillAmount)
{
    ForChild(child, [fFillAmount](CUI_Object* ui) {
        auto pGauge = dynamic_cast<CGaugeUI*>(ui);
        if (!pGauge)
            return;

        pGauge->Set_FillAmount(fFillAmount);
        });
}

void CUI_BattleHUD::Set_NumberText(Child child, _int iNum, _int iWidth)
{
    ForChild(child, [iNum, iWidth](CUI_Object* ui)         {
        auto pTextSlot = ui->Get_Component<CTextSlot>();
        if (!pTextSlot)
            return;

        wchar_t buf[32];
        Helper::Format_FixedZeroPad(buf, _countof(buf), iNum, iWidth);
        pTextSlot->Set_Text(buf);
        });
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