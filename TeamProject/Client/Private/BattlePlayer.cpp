#include "pch.h"
#include "BattlePlayer.h"

//gameobject
#include "GameObject.h"

//helper class
#include "GameInstance.h"
#include "BattleSystem.h"
#include "DataBase.h"
#include "Helper_Func.h"
#include "CharacterController.h"
#include "StateMachine.h"

//character class
#include "Character.h"
#include "CharacterAttackCollider.h"
#include "CharacterParryCollider.h"
#include "Corin.h"
#include "JaneDoe.h"

#include "Camera.h"


CBattlePlayer::CBattlePlayer()
{
}

void CBattlePlayer::Set_Move(_vector3 vPos, _vector3 vRot)
{
	m_pCurrentCharacter->Get_Component<CCharacterController>()->Set_Position(vPos);
	m_pCurrentCharacter->Get_Component<CTransform>()->Rotation(vRot);
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

	CBattleSystem::GetInstance()->SetPlayer(m_CharacterHandles);
}

void CBattlePlayer::QuestStart()
{
	m_pCurrentCharacter->On_Start();
}

HRESULT CBattlePlayer::Initialize()
{
	CBattleSystem::GetInstance()->SetBattlePlayer(this);
	Initialize_CharacterPrototype();

	vector<CHARACTER> BattleCharacters = {CHARACTER::JaneDoe, CHARACTER::Corin};
	SetBattleCharacters(BattleCharacters);

	UI_ACTION_PRIMARY_DESC desc;
	desc.eMode = UI_ACTION_PRIMARY_MODE::ATTACK;
	EventSystem()->Broadcast<UI_ACTION_PRIMARY_DESC>({ desc });

	return S_OK;
}

void CBattlePlayer::Priority_Update(_float dt)
{
	if (m_pCurrentCharacter == nullptr)
		return;
	m_pCurrentCharacter->Reset_Interact();
	if(Can_Input())
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

	/* Evade & EvadePerfect */
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
	
	/* Energy */
	desc.eType = UI_ACTION_TYPE::SPECIAL;
	CCharacter::EnergyDesc tEnergy = m_pCurrentCharacter->Get_EnergyDesc();
	if (tEnergy.fCurrentEnergy >= tEnergy.fSpecialEnergy &&
		tEnergy.fPrevEnergy < tEnergy.fSpecialEnergy)
	{
		desc.eState = UI_ACTION_STATE::AVAILABLE;
		EventSystem()->Broadcast<UI_ACTION_DESC>({ desc });
	}

	/* Ultimate */
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
	ImGui::Text("Can Parry : %s", m_pCurrentCharacter->Can_Parry() ? "TRUE" : "FALSE");
	ImGui::Text("Can Move : %s", m_pCurrentCharacter->Can_Move() ? "TRUE" : "FALSE");
	ImGui::Text("Is Invicible : %s", m_pCurrentCharacter->Is_Invincible() ? "TRUE" : "FALSE");
}

void CBattlePlayer::Add_Gauge(_float fEnergy, _float fDecibel)
{
	for (_uint i = 0; i < m_BattleCharacters.size(); ++i)
	{
		CCharacter* pCharacter = m_BattleCharacters[i];
		CCharacter::EnergyDesc tEnergy = pCharacter->Get_EnergyDesc();
		_float fCurrentDecibel = pCharacter->Get_CurrentDecibel();

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
	NotifyCharacterSwitchOut();
	if (bNext)
		SwitchToNext();
	else
		SwitchToPrev();
	// 콤보 어택 전용 NotifyCharacterSwitchIn
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

void CBattlePlayer::Update_Input(_float dt)
{
	// 콤보 테스트
	if (InputDevice()->Key_Tap('C'))
		Request_ComboAttack();
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
	/* ParryCount */
	UI_ACTION_DESC desc;
	desc.eType = UI_ACTION_TYPE::SWITCH;
	desc.eState = Can_Switch() ? UI_ACTION_STATE::AVAILABLE : desc.eState = UI_ACTION_STATE::ENABLE;
	if (InputDevice()->Key_Tap(VK_SPACE))
	{
		if (Can_Switch())
		{
			desc.eState = UI_ACTION_STATE::EXECUTING;
			SwitchCharacter(m_bSwitchNext);
			m_iParryingCount--;
			if (m_iParryingCount == 0) m_iParryingCount = 6;
			m_fSwitchCooldown = SWITCH_COOLDOWN;
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
	if(InputDevice()->Key_Tap('F'))
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

_bool CBattlePlayer::Can_Switch() const
{
	if (m_fSwitchCooldown > 0.f) return false;
	if (m_BattleCharacters.size() <= 1) return false;
	return true;
}

void CBattlePlayer::Update_Target()
{
	if (m_TargetHandle.isValid())
	{
		_float fMaxDistance = (m_TargetHandle.Get()->Get_Tag() == "Boss")
			? TARGET_BOSS_MAXDISTANCE
			: TARGET_MAXDISTANCE;

		if ((m_TargetHandle.Get()->Get_WorldPos() - m_pCurrentCharacter->Get_WorldPos()).Length()
			< fMaxDistance)
			return;
	}

	auto Monsters = CBattleSystem::GetInstance()->GetBattleObjects(CBattleSystem::BATTLE_OBJ_TYPE::MONSTER);
	_float fminDistance = FLT_MAX;
	for (auto& monster : Monsters)
	{
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

HRESULT CBattlePlayer::Initialize_CharacterPrototype()
{
	auto pProto = PrototypeManager();
	if (FAILED(pProto->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_Corin", CCorin::Create())))
		return E_FAIL;
	if (FAILED(pProto->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_JaneDoe", CJaneDoe::Create())))
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
	characterCCT.iCollisionMask = 0xFFFFFFFF;
	//characterCCT.iCollisionMask = 0xFFFFFFFF & ~ENUM(COLLISION_GROUP::COMMON);
	characterCCT.bAutoFit = false;
	characterCCT.fHeight = 1.13;
	characterCCT.fRadius = 0.3f;
	characterCCT.eGroup = COLLISION_GROUP::PLAYER;
	//characterCCT.fBoundingMinY = -0.88f;
	characterCCT.vPos = { 0.f, 1.5f, 0.f };

	switch (character)
	{
	case CHARACTER::JaneDoe:
	{
		auto JaneDoe = Builder::Create_Object({ G_GlobalLevelKey , "Proto_GameObject_JaneDoe"})
			.Position(_float3(3.f, 0.f, 0.f))
			.CharacterController(characterCCT)
			.Build("JaneDoe");
		ObjectManager()->Add_Object(JaneDoe, { LevelManager()->Get_NowLevelKey(), "Model_Layer" });
		return JaneDoe;
	}
	case CHARACTER::Corin:
	{
		characterCCT.fHeight = 0.73f;
		characterCCT.fRadius = 0.27f;
		auto Corin = Builder::Create_Object({ G_GlobalLevelKey, "Proto_GameObject_Corin" })
			.Position(_float3(3.f, 0.f, 0.f))
			.CharacterController(characterCCT)
			.Build("Corin");
		ObjectManager()->Add_Object(Corin, { LevelManager()->Get_NowLevelKey(), "Model_Layer" });
		return Corin;
	}
	}
	return nullptr;
}

void CBattlePlayer::SwitchToNext()
{
	swap(m_BattleCharacters[0], m_BattleCharacters[1]);
	m_pCurrentCharacter = m_BattleCharacters[0];
}

void CBattlePlayer::SwitchToPrev()
{
	swap(m_BattleCharacters[0], m_BattleCharacters[m_BattleCharacters.size() - 1]);
	m_pCurrentCharacter = m_BattleCharacters[0];
}

HRESULT CBattlePlayer::ClearCharacters()
{
	m_BattleCharacters.clear();
	m_iCurrentIndex = 0;
	m_pCurrentCharacter = nullptr;
	return S_OK;
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
		+ XMVectorScale(vRight, 0.5f)
		- XMVectorScale(m_vSwitchLook, 1.f)
		+ XMVectorSet(0.f, 1.f, 0.f, 0.f);

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

HRESULT CBattlePlayer::SwitchCharacter(_bool bNext)
{
	NotifyCharacterSwitchOut();
	if (bNext)
	{
		SwitchToNext();
	}
	else
	{
		SwitchToPrev();
	}
	NotifyCharacterSwitchIn();
	Sync_ActionUI();
	return S_OK;
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
