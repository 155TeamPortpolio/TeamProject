#include "pch.h"
#include "BattlePlayer.h"

#include "GameObject.h"

#include "GameInstance.h"
#include "BattleSystem.h"
#include "DataBase.h"
#include "Helper_Func.h"
#include "CharacterController.h"
#include "StateMachine.h"

#include "Character.h"
#include "CharacterAttackCollider.h"
#include "CharacterParryCollider.h"
#include "Corin.h"
#include "JaneDoe.h"
#include "Miyabi.h"

#include "Camera.h"
#include "DisplayGate.h"

CBattlePlayer::CBattlePlayer()
{
}

HRESULT CBattlePlayer::Initialize()
{
    CBattleSystem::GetInstance()->SetBattlePlayer(this);
    Initialize_CharacterPrototype();

    vector<CHARACTER> BattleCharacters = {CHARACTER::Miyabi,CHARACTER::JaneDoe, CHARACTER::Corin, };
    SetBattleCharacters(BattleCharacters);

    return S_OK;
}

void CBattlePlayer::Awake()
{
    UI_ACTION_PRIMARY_DESC desc;
    desc.eMode = UI_ACTION_PRIMARY_MODE::ATTACK;
    EventSystem()->Broadcast<UI_ACTION_PRIMARY_DESC>({ desc });

    m_bAwaked = true;
}

void CBattlePlayer::Priority_Update(_float dt)
{
    if (!m_bAwaked)
        Awake();

    if (m_pCurrentCharacter == nullptr)
        return;

    m_pCurrentCharacter->Reset_Interact();

    if (Can_Input())
        Update_Input(dt);

    Update_Target();
}

void CBattlePlayer::Update(_float dt)
{
    if (m_fSwitchCooldown > 0.f)
    {
        m_fSwitchCooldown -= dt;
        if (m_fSwitchCooldown <= 0.f)
            m_fSwitchCooldown = 0.f;
    }

    if (m_fLockOnCooldown > 0.f)
    {
        m_fLockOnCooldown -= dt;
        if (m_fLockOnCooldown <= 0.f)
            m_fLockOnCooldown = 0.f;
    }

    UI_ACTION_DESC desc{};

    // Evade & EvadePerfect
    if (m_pCurrentCharacter->Get_EvadeCooldown() > 0.f)
    {
        desc.eType = UI_ACTION_TYPE::EVADEPERFECT;
        desc.eState = UI_ACTION_STATE::ENABLE;
        desc.fFillAmount = 1.0f - m_pCurrentCharacter->Get_EvadeCooldown() / 1.0f;
        EventSystem()->Broadcast<UI_ACTION_DESC>({ desc });
    }
    else
    {
        desc.eType = UI_ACTION_TYPE::EVADE;
        desc.eState = UI_ACTION_STATE::EXECUTING;
        desc.fFillAmount = 1.0f - m_pCurrentCharacter->Get_EvadeTimer() / 1.0f;
        EventSystem()->Broadcast<UI_ACTION_DESC>({ desc });
    }

    // Energy
    desc.eType = UI_ACTION_TYPE::SPECIAL;
    CCharacter::EnergyDesc tEnergy = m_pCurrentCharacter->Get_EnergyDesc();
    if (tEnergy.fCurrentEnergy >= tEnergy.fSpecialEnergy &&
        tEnergy.fPrevEnergy < tEnergy.fSpecialEnergy)
    {
        desc.eState = UI_ACTION_STATE::AVAILABLE;
        EventSystem()->Broadcast<UI_ACTION_DESC>({ desc });
    }

    // Ultimate
    desc.eType = UI_ACTION_TYPE::ULTIMATE;
    _float fCurrent = m_pCurrentCharacter->Get_CurrentDecibel();
    _float fPrev = m_pCurrentCharacter->Get_PrevDecibel();
    if (fCurrent >= m_pCurrentCharacter->Get_MaxDecibel() &&
        fPrev < m_pCurrentCharacter->Get_MaxDecibel())
    {
        desc.eState = UI_ACTION_STATE::AVAILABLE;
        EventSystem()->Broadcast<UI_ACTION_DESC>({ desc });
    }

    Update_Status();
}

void CBattlePlayer::Late_Update(_float dt)
{
}

void CBattlePlayer::Render_GUI()
{
    if (nullptr == m_pCurrentCharacter)
        return;

    // Current Character Info
    if (ImGui::CollapsingHeader("Current Character", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Name : %s", Helper::EnumToString(m_pCurrentCharacter->Get_CharacterName()));
        ImGui::Text("Is Main : %s", m_pCurrentCharacter->Is_MainCharacter() ? "TRUE" : "FALSE");
        ImGui::Text("ParryAid : %s", m_pCurrentCharacter->Can_Parry() ? "TRUE" : "FALSE");
        ImGui::Text("PerfectDodge : %s", m_pCurrentCharacter->Is_Perfect() ? "TRUE" : "FALSE");
        ImGui::Text("Can Move : %s", m_pCurrentCharacter->Can_Move() ? "TRUE" : "FALSE");
        ImGui::Text("Is Invincible : %s", m_pCurrentCharacter->Is_Invincible() ? "TRUE" : "FALSE");
    }

    // Input State
    if (ImGui::CollapsingHeader("Input State"))
    {
        ImGui::Text("Direction : (%.2f, %.2f, %.2f)", m_input.direction.x, m_input.direction.y, m_input.direction.z);
        ImGui::Text("Current Key : (%d, %d)", m_input.current.x, m_input.current.z);
        ImGui::Text("Buffer Timer : %.3f", m_input.bufferTimer);
        ImGui::Text("Lock Input : %s", m_bLockInput ? "TRUE" : "FALSE");
    }

    // Switch State
    if (ImGui::CollapsingHeader("Switch State"))
    {
        ImGui::Text("Can Switch : %s", Can_Switch() ? "TRUE" : "FALSE");
        ImGui::Text("Switch Cooldown : %.2f", m_fSwitchCooldown);
        ImGui::Text("Parrying Count : %d / 6", m_iParryingCount);
        ImGui::Text("Reserve Parry : %s", m_bReserveParry ? "TRUE" : "FALSE");
        ImGui::Text("Combo Select : %s", m_bComboSelect ? "TRUE" : "FALSE");
        if (m_bComboSelect)
            ImGui::Text("Combo Timer : %.2f / %.2f", m_fComboSelectTimer, COMBO_SELECT_DURATION);
    }

    // Target Info
    if (ImGui::CollapsingHeader("Target"))
    {
        ImGui::Text("Lock On : %s", m_bLockOn ? "TRUE" : "FALSE");
        ImGui::Text("Lock On Cooldown : %.2f", m_fLockOnCooldown);
        if (m_TargetHandle.isValid())
            ImGui::Text("Target : %s", m_TargetHandle.Get()->Get_InstanceName().c_str());
        else
            ImGui::Text("Target : None");
    }

    // Battle Characters List
    if (ImGui::CollapsingHeader("Battle Characters", ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (_uint i = 0; i < m_BattleCharacters.size(); ++i)
        {
            CCharacter* pChar = m_BattleCharacters[i];
            _bool bIsMain = pChar->Is_MainCharacter();
            _bool bCanSwitchIn = pChar->Can_SwitchIn();
            _bool bCCTActive = pChar->Get_CCT()->Get_CompActive();

            ImGui::PushID(i);

            // 상태에 따른 색상 표시
            // 초록 : 현재 활성 캐릭터 (인덱스 0)
            // 빨강 : 교체 불가 상태
            // 흰색 : 대기 중 (정상)
            if (i == 0)
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 1.0f, 0.2f, 1.0f));
            else if (!bCanSwitchIn)
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
            else
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

            ImGui::Text("[%d] %s", i, pChar->Get_Name().c_str());
            ImGui::PopStyleColor();

            ImGui::SameLine();
            ImGui::Text("| Main:%s CCT:%s SwitchIn:%s",
                bIsMain ? "O" : "X",
                bCCTActive ? "O" : "X",
                bCanSwitchIn ? "O" : "X");

            // 비정상 상태 감지 및 강제 비활성화 버튼
            // 케이스 1: 메인이 아닌데 CCT가 활성화됨
            // 케이스 2: 메인인데 CCT가 비활성화됨 (인덱스 0인 경우)
            _bool bAbnormal = (!bIsMain && bCCTActive) || (bIsMain && !bCCTActive && i == 0);
            if (bAbnormal)
            {
                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
                if (ImGui::SmallButton("Force Fix"))
                {
                    if (!bIsMain && bCCTActive)
                        pChar->DeActive_Character();
                    else if (bIsMain && !bCCTActive)
                        pChar->Active_Character();
                }
                ImGui::PopStyleColor();
            }

            ImGui::PopID();
        }

        ImGui::Separator();

        // 전체 강제 정리 버튼
        if (ImGui::Button("Force Sync All Characters"))
        {
            for (_uint i = 0; i < m_BattleCharacters.size(); ++i)
            {
                CCharacter* pChar = m_BattleCharacters[i];
                if (i == 0)
                {
                    pChar->Set_MainCharacter(true);
                    pChar->Active_Character();
                }
                else
                {
                    pChar->Set_MainCharacter(false);
                    pChar->DeActive_Character();
                }
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Force DeActive All Non-Main"))
        {
            for (_uint i = 1; i < m_BattleCharacters.size(); ++i)
            {
                m_BattleCharacters[i]->Set_MainCharacter(false);
                m_BattleCharacters[i]->DeActive_Character();
            }
        }
    }
}

OBJECT_HANDLE CBattlePlayer::GetCurCharacterHandle()
{
    return m_pCurrentCharacter->Get_Handle();
}

void CBattlePlayer::SetBattleCharacters(vector<CHARACTER> battleCharacters)
{
    for (auto& character : battleCharacters)
    {
        auto newCharacter = dynamic_cast<CCharacter*>(CreateBattleCharacter(character));
        newCharacter->Set_MainCharacter(false);
        newCharacter->DeActive_Character();
        m_BattleCharacters.push_back(newCharacter);
        m_CharacterHandles.push_back(newCharacter->Get_Handle());
    }

    m_iCurrentIndex = 0;
    m_pCurrentCharacter = m_BattleCharacters[m_iCurrentIndex];
    m_pCurrentCharacter->SetRenderLayer(RENDER_LAYER::Default);
    m_pCurrentCharacter->Set_MainCharacter(true);
    m_pCurrentCharacter->Active_Character();

    BattleSystem()->SetPlayer(m_CharacterHandles);
}

void CBattlePlayer::Active_Battle()
{
    if (!m_BattleCharacters.empty())
        m_BattleCharacters[m_iCurrentIndex]->Active_Character();
}

void CBattlePlayer::DeActive_Battle()
{
    for (auto* pCharacter : m_BattleCharacters)
        pCharacter->DeActive_Character();
}

void CBattlePlayer::QuestStart()
{
    m_pCurrentCharacter->On_Start();
}

HRESULT CBattlePlayer::SwitchCharacter(_int iTargetIndex)
{
    if (iTargetIndex < 0 || iTargetIndex >= (_int)m_BattleCharacters.size())
        return E_FAIL;

    NotifyCharacterSwitchOut();

    rotate(m_BattleCharacters.begin(),
        m_BattleCharacters.begin() + iTargetIndex,
        m_BattleCharacters.end());

    m_pCurrentCharacter = m_BattleCharacters[0];

    NotifyCharacterSwitchIn();
    Sync_ActionUI();
    return S_OK;
}

HRESULT CBattlePlayer::ClearCharacters()
{
    m_BattleCharacters.clear();
    m_iCurrentIndex = 0;
    m_pCurrentCharacter = nullptr;
    return S_OK;
}

void CBattlePlayer::Set_Move(_vector3 vPos, _vector3 vRot)
{
    m_pCurrentCharacter->Get_Component<CCharacterController>()->Set_Position(vPos);
    m_pCurrentCharacter->Get_Component<CTransform>()->Rotation(vRot);
}

void CBattlePlayer::Add_Gauge(_float fEnergy, _float fDecibel)
{
    for (_uint i = 0; i < m_BattleCharacters.size(); ++i)
    {
        CCharacter* pCharacter = m_BattleCharacters[i];
        CCharacter::EnergyDesc tEnergy = pCharacter->Get_EnergyDesc();
        _float fCurrentDecibel = pCharacter->Get_CurrentDecibel();

        // 현재 활성 캐릭터는 100%, 대기 캐릭터는 10%만 획득
        if (i == m_iCurrentIndex)
        {
            tEnergy.fCurrentEnergy += fEnergy;
            fCurrentDecibel += fDecibel;
        }
        else
        {
            tEnergy.fCurrentEnergy += fEnergy * 0.1f;
            fCurrentDecibel += fDecibel * 0.1f;
        }

        if (tEnergy.fCurrentEnergy > pCharacter->Get_MaxEnergy())
            tEnergy.fCurrentEnergy = pCharacter->Get_MaxEnergy();
        if (fCurrentDecibel > pCharacter->Get_MaxDecibel())
            fCurrentDecibel = pCharacter->Get_MaxDecibel();

        pCharacter->Set_CurrentEnergy(tEnergy.fCurrentEnergy);
        pCharacter->Set_Decibel(fCurrentDecibel);
    }
}

void CBattlePlayer::Request_ComboAttack()
{
    if (!Can_Switch())
        return;
    if (m_bComboSelect)
        return;

    m_bComboSelect = true;
    // 타임스케일 2초간 느리게 하기 몬스터, 캐릭터
    BattleSystem()->StartGimmick(BATTLE_VFX_TYPE::SWITCH);
    // UI 방송
}

void CBattlePlayer::Execute_ComboAttack(_bool bNext)
{
    _int iTargetIndex = Find_SwitchIndex(bNext);
    if (iTargetIndex == -1)
    {
        Cancel_ComboAttack();
        return;
    }

    NotifyCharacterSwitchOut();

    rotate(m_BattleCharacters.begin(),
        m_BattleCharacters.begin() + iTargetIndex,
        m_BattleCharacters.end());
    m_pCurrentCharacter = m_BattleCharacters[0];

    // 콤보 어택 전용 SwitchIn
    m_pCurrentCharacter->Set_MainCharacter(true);
    m_pCurrentCharacter->Active_Character();
    m_pCurrentCharacter->Get_Component<CCharacterController>()->Set_Position(m_vSwitchPosition);
    m_pCurrentCharacter->Get_Component<CTransform>()->Set_Look(m_vSwitchLook);
    m_pCurrentCharacter->Set_TargetHandle(m_TargetHandle);
    m_pCurrentCharacter->On_SwitchIn(CCharacter::SWITCH::ATTACK);

    Sync_ActionUI();

    m_fSwitchCooldown = SWITCH_COOLDOWN;
    m_fComboSelectTimer = 0.f;
    m_bComboSelect = false;
}

void CBattlePlayer::Cancel_ComboAttack()
{
    m_fComboSelectTimer = 0.f;
    m_bComboSelect = false;
}

void CBattlePlayer::Start_ChainParry()
{
    if (m_bChainParry)
        return;

    m_bChainParry = true;
}

void CBattlePlayer::End_ChainParry()
{
    if (!m_bChainParry)
        return;
    
    m_bChainParry = false;
}

void CBattlePlayer::Update_Input(_float dt)
{
    // 콤보 테스트
    //if (InputDevice()->Key_Tap('G'))
    //    Request_ComboAttack();
    if (m_bComboSelect)
    {
        Process_ComboSelect(dt);
        return;
    }

    m_input.prevDirection = m_input.direction;
    m_input.previous = m_input.current;

    KeyInput key;
    if (InputDevice()->Key_Hold('W')) key.z += 1;
    if (InputDevice()->Key_Hold('S')) key.z -= 1;
    if (InputDevice()->Key_Hold('D')) key.x += 1;
    if (InputDevice()->Key_Hold('A')) key.x -= 1;

    m_input.current = key;

    if (!key.IsZero())
    {
        if (m_input.lastValid.IsZero() || m_input.bufferTimer <= 0.f)
            m_input.lastValid = key;

        m_input.bufferTimer = KEY_BUFFER_TIME;

        if (key != m_input.currentMove)
        {
            m_input.previousMove = m_input.currentMove;
            m_input.currentMove = key;
        }
    }
    else
    {
        m_input.bufferTimer -= dt;
        if (m_input.bufferTimer < 0.f)
        {
            m_input.bufferTimer = 0.f;
            m_input.lastValid.Reset();
            m_input.previousMove.Reset();
            m_input.currentMove.Reset();
        }
    }

    // 카메라 기준 이동 방향 계산
    m_input.direction = {};
    if (!key.IsZero())
    {
        auto cam = CameraManager()->Get_ActiveCam();
        auto camTf = cam->Get_Owner()->Get_Component<CTransform>();
        _vector3 look = camTf->Dir(STATE::LOOK);
        look.y = 0.f;
        look.Normalize();
        _vector3 right = _vector3::Up.Cross(look);
        right.Normalize();
        m_input.direction = look * (_float)key.z + right * (_float)key.x;
        m_input.direction.Normalize();
    }

    if (!m_pCurrentCharacter) return;

    Process_Movement(dt);
    Process_Attack();
    Process_Evade();
    Process_Switch();
    Process_Ultimate();
    Process_Energy();
    Process_Interact();

    // 락온 토글
    if (InputDevice()->Mouse_Tap(MOUSE_BTN::MB) && m_fLockOnCooldown <= 0.f)
    {
        m_bLockOn = !m_bLockOn;
        m_fLockOnCooldown = LOCKON_COOLDOWN;

        if (!m_TargetHandle.isValid()) return;

        TARGET_LOCK_DESC desc;
        desc.bLock = m_bLockOn;
        desc.tHandle = m_TargetHandle;
        EventSystem()->Broadcast<TARGET_LOCK_DESC>({ desc });
    }
}

void CBattlePlayer::Process_Movement(_float dt)
{
    CCharacter::InputInfo inputInfo;
    inputInfo.direction = m_input.direction;
    inputInfo.prevDirection = m_input.prevDirection;
    inputInfo.bufferTimer = m_input.bufferTimer;
    inputInfo.prevMoveX = m_input.previousMove.x;
    inputInfo.prevMoveZ = m_input.previousMove.z;
    inputInfo.curMoveX = m_input.currentMove.x;
    inputInfo.curMoveZ = m_input.currentMove.z;

    m_pCurrentCharacter->On_Move(inputInfo);

    if (m_pCurrentCharacter->Get_InputReset())
    {
        m_input.previousMove = m_input.currentMove;
        m_pCurrentCharacter->Set_ResetMove(false);
    }
}

void CBattlePlayer::Process_Attack()
{
    if (InputDevice()->Mouse_Tap(MOUSE_BTN::LB))
    {
        m_pCurrentCharacter->On_Attack();
    }
}

void CBattlePlayer::Process_SpecialAttack()
{
}

void CBattlePlayer::Process_Evade()
{
    if (InputDevice()->Mouse_Tap(MOUSE_BTN::RB))
    {
        if (m_pCurrentCharacter->Can_Evade())
        {
            m_pCurrentCharacter->On_Evade();
            m_pCurrentCharacter->Buffer_Evade();
        }
    }
}

void CBattlePlayer::Process_Switch()
{
    // ParryCount
    UI_ACTION_DESC desc;
    desc.eType = UI_ACTION_TYPE::SWITCH;
    desc.eState = Can_Switch() ? UI_ACTION_STATE::AVAILABLE : desc.eState = UI_ACTION_STATE::ENABLE;

    _bool bForward = InputDevice()->Key_Tap(VK_SPACE);
    _bool bBackward = InputDevice()->Key_Tap('C');
    if (bForward || bBackward)
    {
        if (Can_Switch())
        {
            _int iTargetIndex = Find_SwitchIndex(bForward);
            if (iTargetIndex != -1)
            {
                desc.eState = UI_ACTION_STATE::EXECUTING;
                SwitchCharacter(iTargetIndex);
            }
        }
        else if (m_bChainParry && m_pCurrentCharacter->Can_Parry())
        {
            // 교체 불가지만 체인 모드에서 패링 가능하면 현재 캐릭터로 재패링
            m_ParryHandle = m_pCurrentCharacter->Calculate_Parry();
            m_vSwitchPosition = _vector4{ m_pCurrentCharacter->Get_ParryPos() };
            m_vSwitchLook = _vector4{ m_pCurrentCharacter->Get_ParryLook() };

            m_pCurrentCharacter->Get_CCT()->Set_Position(m_vSwitchPosition);
            m_pCurrentCharacter->Get_Component<CTransform>()->Set_Look(m_vSwitchLook);
            m_pCurrentCharacter->Set_ParryHandle(m_ParryHandle);
            m_pCurrentCharacter->On_ChainParry();
        }
    }

    desc.fFillAmount = m_iParryingCount / 6.f;
    EventSystem()->Broadcast<UI_ACTION_DESC>({ desc });
}

void CBattlePlayer::Process_Ultimate()
{
    if (InputDevice()->Key_Tap('Q'))
    {
        if (m_pCurrentCharacter->Can_Ultimate())
        {
            m_pCurrentCharacter->On_Ultimate();
        }
    }
}

void CBattlePlayer::Process_Energy()
{
    if (InputDevice()->Key_Down('E'))
    {
        m_pCurrentCharacter->On_Special();
    }
}

void CBattlePlayer::Process_Interact()
{
    if (InputDevice()->Key_Tap('F'))
    {
        m_pCurrentCharacter->On_Interact();
    }
}

void CBattlePlayer::Process_ComboSelect(_float dt)
{
    m_fComboSelectTimer += dt;
    if (m_fComboSelectTimer >= COMBO_SELECT_DURATION)
    {
        Cancel_ComboAttack();
        return;
    }

    if (InputDevice()->Mouse_Tap(MOUSE_BTN::LB))
    {
        Execute_ComboAttack(true);
    }
    else if (InputDevice()->Mouse_Tap(MOUSE_BTN::RB))
    {
        Execute_ComboAttack(false);
    }
    else if (InputDevice()->Mouse_Tap(MOUSE_BTN::MB))
    {
        Cancel_ComboAttack();
    }
}

void CBattlePlayer::NotifyCharacterSwitchIn()
{
    m_pCurrentCharacter->Set_MainCharacter(true);
    m_pCurrentCharacter->Active_Character();
    m_pCurrentCharacter->Get_Component<CCharacterController>()->Set_Position(m_vSwitchPosition);
    m_pCurrentCharacter->Get_Component<CTransform>()->Set_Look(m_vSwitchLook);
    m_pCurrentCharacter->Set_TargetHandle(m_TargetHandle);

    if (m_bReserveParry)
    {
        m_pCurrentCharacter->On_SwitchIn(CCharacter::SWITCH::PARRYAID);
        m_pCurrentCharacter->Set_ParryHandle(m_ParryHandle);
        m_bReserveParry = false;
    }
    else
        m_pCurrentCharacter->On_SwitchIn(CCharacter::SWITCH::NORMAL);
}

void CBattlePlayer::NotifyCharacterSwitchOut()
{
    auto vRight = m_pCurrentCharacter->Get_Component<CTransform>()->Dir(STATE::RIGHT);
    m_vSwitchLook = m_pCurrentCharacter->Get_Component<CTransform>()->Dir(STATE::LOOK);
    m_vSwitchPosition = m_pCurrentCharacter->Get_Component<CCharacterController>()->Get_FootPosition()
        + XMVectorScale(vRight, 0.7f)
        - XMVectorScale(m_vSwitchLook, 1.5f)
        + XMVectorSet(0.f, 0.5f, 0.f, 0.f);

    // 패링 가능 상태면 패링 위치로 전환
    if (m_pCurrentCharacter->Can_Parry())
    {
        m_bReserveParry = true;
        m_ParryHandle = m_pCurrentCharacter->Calculate_Parry();
        m_vSwitchPosition = _vector4{ m_pCurrentCharacter->Get_ParryPos() };
        m_vSwitchLook = _vector4{ m_pCurrentCharacter->Get_ParryLook() };
    }
    else if (m_bComboSelect)
    {
        m_vSwitchPosition = m_pCurrentCharacter->Get_Component<CCharacterController>()->Get_FootPosition()
            + XMVectorScale(vRight, 0.5f)
            + XMVectorSet(0.f, 1.f, 0.f, 0.f);
    }

    m_pCurrentCharacter->Set_MainCharacter(false);
    m_pCurrentCharacter->On_SwitchOut();
    m_input.ResetBuffer();

    if (!m_bChainParry)
    {
        m_iParryingCount--;
        if (m_iParryingCount == 0) m_iParryingCount = 6;
        m_fSwitchCooldown = SWITCH_COOLDOWN;
    }
}

_bool CBattlePlayer::Can_Switch() const
{
    if (m_fSwitchCooldown > 0.f) return false;   
    if (m_BattleCharacters.size() <= 1) return false;
    if (m_pCurrentCharacter->Can_SwitchIn()) return false;  // 메인이 비활성화면 교체 불가

    // 교체 가능한 캐릭터가 하나라도 있는지 확인
    return Find_SwitchIndex(true) != -1 || Find_SwitchIndex(false) != -1;
}

_bool CBattlePlayer::Can_SwitchTo(_uint iIndex) const
{
    if (iIndex >= m_BattleCharacters.size()) return false;
    if (iIndex == 0) return false;  // 현재 캐릭터(인덱스 0)로는 교체 불가

    return m_BattleCharacters[iIndex]->Can_SwitchIn();
}

_int CBattlePlayer::Find_SwitchIndex(_bool bNext) const
{
    if (m_BattleCharacters.size() <= 1) return -1;

    // 2명일 경우 인덱스 1만 확인
    if (m_BattleCharacters.size() == 2)
    {
        return Can_SwitchTo(1) ? 1 : -1;
    }

    // 3명일 경우 방향에 따라 우선순위 결정
    if (bNext)
    {
        if (Can_SwitchTo(1)) return 1;
        if (Can_SwitchTo(2)) return 2;
    }
    else
    {
        _uint iLastIndex = (_uint)m_BattleCharacters.size() - 1;
        if (Can_SwitchTo(iLastIndex)) return iLastIndex;
        if (Can_SwitchTo(1)) return 1;
    }

    return -1;
}

void CBattlePlayer::Update_Target()
{
    // 현재 타겟이 유효하고 사거리 내면 유지
    if (m_TargetHandle.isValid())
    {
        _float fMaxDistance = (m_TargetHandle.Get()->Get_Tag() == "Boss")
            ? TARGET_BOSS_MAXDISTANCE
            : TARGET_MAXDISTANCE;

        if ((m_TargetHandle.Get()->Get_WorldPos() - m_pCurrentCharacter->Get_WorldPos()).Length()
            < fMaxDistance)
            return;
    }

    // 가장 가까운 몬스터 탐색
    auto Monsters = CBattleSystem::GetInstance()->GetBattleObjects(CBattleSystem::BATTLE_OBJ_TYPE::MONSTER);
    _float fminDistance = FLT_MAX;

    for (auto& monster : Monsters)
    {
        if (!monster.hObject.isValid())  continue;
        _vector3 vToMonster = monster.vPos - m_pCurrentCharacter->Get_WorldPos();
        _float fDistance = vToMonster.Length();

        _float fDetectDistance = (monster.hObject.Get()->Get_Tag() == "Boss")
            ? TARGET_BOSS_MAXDISTANCE
            : TARGET_MAXDISTANCE;

        if (fDistance < fDetectDistance && fDistance < fminDistance)
        {
            fminDistance = fDistance;
            m_TargetHandle = monster.hObject;
        }
    }

    m_pCurrentCharacter->Set_TargetHandle(m_TargetHandle);
}

void CBattlePlayer::Update_Status()
{
    UI_PLAYER_INIT_DESC initdesc;
    initdesc.iCount = m_BattleCharacters.size();
    EventSystem()->Broadcast<UI_PLAYER_INIT_DESC>({ initdesc });

    for (_uint i = 0; i < m_BattleCharacters.size() && i < 3; ++i)
    {
        CCharacter* pCharacter = m_BattleCharacters[i];
        UI_STATUS_OWNER eOwner = static_cast<UI_STATUS_OWNER>(ENUM(UI_STATUS_OWNER::ROLE1) + i);

        UI_PLAYER_STATUS_DESC desc;
        desc.eOwner = eOwner;
        desc.eCharacter = pCharacter->Get_CharacterName();
        desc.hp = { pCharacter->Get_HP(), pCharacter->Get_MaxHP() };
        desc.special = { pCharacter->Get_EnergyDesc().fCurrentEnergy, pCharacter->Get_MaxEnergy() };
        desc.specialThreshold = pCharacter->Get_EnergyDesc().fSpecialEnergy;
        desc.ultimate = { pCharacter->Get_CurrentDecibel(), pCharacter->Get_MaxDecibel() };

        EventSystem()->Broadcast<UI_PLAYER_STATUS_DESC>({ desc });
    }
}

void CBattlePlayer::Sync_ActionUI()
{
    UI_ACTION_DESC desc;

    // Energy
    desc.eType = UI_ACTION_TYPE::SPECIAL;
    CCharacter::EnergyDesc tEnergy = m_pCurrentCharacter->Get_EnergyDesc();
    desc.eState = (tEnergy.fCurrentEnergy >= tEnergy.fSpecialEnergy)
        ? UI_ACTION_STATE::AVAILABLE
        : UI_ACTION_STATE::ENABLE;
    EventSystem()->Broadcast<UI_ACTION_DESC>({ desc });

    // Ultimate
    desc.eType = UI_ACTION_TYPE::ULTIMATE;
    desc.eState = (m_pCurrentCharacter->Get_CurrentDecibel() >= m_pCurrentCharacter->Get_MaxDecibel())
        ? UI_ACTION_STATE::AVAILABLE
        : UI_ACTION_STATE::DISABLE;
    EventSystem()->Broadcast<UI_ACTION_DESC>({ desc });
}

HRESULT CBattlePlayer::Initialize_CharacterPrototype()
{
    auto pProto = PrototypeManager();

    if (FAILED(pProto->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_Corin", CCorin::Create())))
        return E_FAIL;
    if (FAILED(pProto->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_JaneDoe", CJaneDoe::Create())))
        return E_FAIL;
    if (FAILED(pProto->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_Miyabi", CMiyabi::Create())))
        return E_FAIL;
    if (FAILED(pProto->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_CharacterAttackCollider", CCharacterAttackCollider::Create())))
        return E_FAIL;
    if (FAILED(pProto->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_CharacterParryCollider", CCharacterParryCollider::Create())))
        return E_FAIL;

    return S_OK;
}

CGameObject* CBattlePlayer::CreateBattleCharacter(CHARACTER character)
{
    CCT_DESC characterCCT;
    characterCCT.eGroup = COLLISION_GROUP::PLAYER;
    characterCCT.iCollisionMask = 0xFFFFFFFF - 
        (ENUM(COLLISION_GROUP::PLAYER) | ENUM(COLLISION_GROUP::PLAYER_ATTACK));
    characterCCT.bAutoFit = false;
    characterCCT.fHeight = 1.13;
    characterCCT.fRadius = 0.3f;
    characterCCT.vPos = { 0.f, 1.5f, 0.f };

    switch (character)
    {
    case CHARACTER::JaneDoe:
    {
        auto JaneDoe = Builder::Create_Object({ G_GlobalLevelKey , "Proto_GameObject_JaneDoe" })
            .Position(_float3(3.f, 0.f, 0.f))
            .CharacterController(characterCCT)
            .Build("JaneDoe");
        ObjectManager()->Add_Object(JaneDoe, { LevelManager()->Get_NowLevelKey(), "Model_Layer" });
        return JaneDoe;
    }
    case CHARACTER::Corin:
    {
        characterCCT.fHeight = 1.17f;
        characterCCT.fRadius = 0.27f;
        auto Corin = Builder::Create_Object({ G_GlobalLevelKey, "Proto_GameObject_Corin" })
            .Position(_float3(3.f, 0.f, 0.f))
            .CharacterController(characterCCT)
            .Build("Corin");
        ObjectManager()->Add_Object(Corin, { LevelManager()->Get_NowLevelKey(), "Model_Layer" });
        return Corin;
    }
    case CHARACTER::Miyabi:
    {
        characterCCT.fHeight = 0.73f;
        characterCCT.fRadius = 0.26f;
        auto Miyabi = Builder::Create_Object({ G_GlobalLevelKey , "Proto_GameObject_Miyabi" })
            .Position(_float3(3.f, 0.f, 0.f))
            .CharacterController(characterCCT)
            .Build("Miyabi");
        ObjectManager()->Add_Object(Miyabi, { LevelManager()->Get_NowLevelKey(), "Model_Layer" });
        return Miyabi;
    }
    }

    return nullptr;
}

CBattlePlayer* CBattlePlayer::Create()
{
    CBattlePlayer* Instance = new CBattlePlayer();
    return Instance;
}

void CBattlePlayer::Free()
{
    __super::Free();
}