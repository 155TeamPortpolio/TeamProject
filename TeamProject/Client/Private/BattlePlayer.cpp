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
#include "Corin.h"
#include "JaneDoe.h"

#include "Camera.h"


CBattlePlayer::CBattlePlayer()
{
}

OBJECT_HANDLE CBattlePlayer::GetCurCharacterHandle()
{
	return m_pCurrentCharacter->Get_Handle(); 
}

void CBattlePlayer::SetBattleCharacters(vector<CHARACTER> battleCharacters)
{
	for (auto& character : battleCharacters)
	{
		string strCharacter = Helper::EnumToString(character);
		auto newCharacter = dynamic_cast<CCharacter*>(CreateBattleCharacter(character));

		m_BattleCharacters.push({ strCharacter,newCharacter });
		m_CharacterHandles.push_back(newCharacter->Get_Handle());
	}

	m_pCurrentCharacter = m_BattleCharacters.front().second;
	m_pCurrentCharacter->SetRenderLayer(RENDER_LAYER::Default);

	CBattleSystem::GetInstance()->SetPlayer(m_CharacterHandles);
}

HRESULT CBattlePlayer::Initialize()
{
	CBattleSystem::GetInstance()->SetBattlePlayer(this);
	Initialize_CharacterPrototype();

	vector<CHARACTER> BattleCharacters = {CHARACTER::JaneDoe, CHARACTER::Corin};
	SetBattleCharacters(BattleCharacters);

	return S_OK;
}

void CBattlePlayer::Priority_Update(_float dt)
{
	if (m_pCurrentCharacter == nullptr)
		return;

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

	_float CurHP = m_pCurrentCharacter->Get_HP();
	m_pCurrentCharacter->Process_HP(CurHP - 0.1);
}

void CBattlePlayer::Late_Update(_float dt)
{
}

void CBattlePlayer::Update_Input(_float dt)
{
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

	if (InputDevice()->Key_Down('T'))
	{
		// 테스트 코드
	}
}

void CBattlePlayer::Process_Movement(_float dt)
{
	//if ("Attack" == dynamic_cast<CJaneDoe*>(m_pCurrentCharacter)->Get_StateMachine()->Get_CurrentStateName())
	//	return;
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

void CBattlePlayer::Process_Evade()
{
	if (InputDevice()->Mouse_Tap(MOUSE_BTN::RB))
	{
		m_pCurrentCharacter->On_Evade();
		m_pCurrentCharacter->Buffer_Evade();
	}
}

void CBattlePlayer::Process_Switch()
{
	if (InputDevice()->Key_Tap(VK_SPACE))
	{
		if (Can_Switch())
		{
			SwitchCharacter();
			m_fSwitchCooldown = SWITCH_COOLDOWN;
		}
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
		if((m_TargetHandle.Get()->Get_WorldPos() - m_pCurrentCharacter->Get_WorldPos()).Length()
			< TARGET_MAXDISTANCE)
			return;
	}

	auto Monsters = CBattleSystem::GetInstance()->GetBattleObjects(CBattleSystem::BATTLE_OBJ_TYPE::MONSTER);

	_float fminDistance = FLT_MAX;
	for (auto& monster : Monsters)
	{
		_vector3 vToMonster = monster.vPos - m_pCurrentCharacter->Get_WorldPos();
		_float fDistance = vToMonster.Length();

		if (fDistance < fminDistance)
		{
			fminDistance = fDistance;
			m_TargetHandle = monster.hObject;
		}
	}
	m_pCurrentCharacter->Set_TargetHandle(m_TargetHandle);
}

HRESULT CBattlePlayer::Initialize_CharacterPrototype()
{
	auto pProto = PrototypeManager();

	if (FAILED(pProto->Add_ProtoType("Test_Level", "Proto_GameObject_Corin", CCorin::Create())))
		return E_FAIL;
	if (FAILED(pProto->Add_ProtoType("Test_Level", "Proto_GameObject_JaneDoe", CJaneDoe::Create())))
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
	characterCCT.fHeight = 1.28f;
	characterCCT.fRadius = 0.2f;
	characterCCT.eGroup = COLLISION_GROUP::PLAYER;
	//characterCCT.fBoundingMinY = -0.88f;
	characterCCT.vPos = { 0.f, 1.5f, 0.f };

	switch (character)
	{
	case CHARACTER::JaneDoe:
	{
		auto JaneDoe = Builder::Create_Object({ "Test_Level", "Proto_GameObject_JaneDoe" })
			.Position(_float3(3.f, 0.f, 0.f))
			.CharacterController(characterCCT)
			.Build("JaneDoe");
		ObjectManager()->Add_Object(JaneDoe, { "Test_Level", "Model_Layer" });
		return JaneDoe;
	}
	case CHARACTER::Corin:
	{
		auto Corin = Builder::Create_Object({ "Test_Level", "Proto_GameObject_Corin" })
			.Position(_float3(3.f, 0.f, 0.f))
			.CharacterController(characterCCT)
			.Build("Corin");
		ObjectManager()->Add_Object(Corin, { "Test_Level", "Model_Layer" });
		return Corin;
	}
	}
	return nullptr;
}

void CBattlePlayer::NotifyCharacterSwitchIn()
{
	m_pCurrentCharacter->Get_Component<CCharacterController>()->Set_Position(m_vSwitchPosition);
	m_pCurrentCharacter->Get_Component<CTransform>()->Set_Look(m_vSwitchLook);
	m_pCurrentCharacter->Set_TargetHandle(m_TargetHandle);

	if (m_pCurrentCharacter->Can_Parry())
	{
		m_pCurrentCharacter->On_SwitchIn(CCharacter::SWITCH::PARRYAID);
	}
	m_pCurrentCharacter->On_SwitchIn(CCharacter::SWITCH::NORMAL);
}

void CBattlePlayer::NotifyCharacterSwitchOut()
{
	auto vRight = m_pCurrentCharacter->Get_Component<CTransform>()->Dir(STATE::RIGHT);
	m_vSwitchLook = m_pCurrentCharacter->Get_Component<CTransform>()->Dir(STATE::LOOK);
	m_vSwitchPosition = m_pCurrentCharacter->Get_Component<CCharacterController>()->Get_FootPosition() 
		+ vRight* 0.5 - m_vSwitchLook * 4;

	m_pCurrentCharacter->On_SwitchOut();
}

void CBattlePlayer::RotateCharacterQueue()
{
	auto ReplacedPlayer = m_BattleCharacters.front();
	m_BattleCharacters.pop();
	m_BattleCharacters.push(ReplacedPlayer);
}

HRESULT CBattlePlayer::SwitchCharacter(CHARACTER character)
{
	NotifyCharacterSwitchOut();
	if (character == CHARACTER::END)
	{
		RotateCharacterQueue();
	}
	else
	{
		string targetName = Helper::EnumToString(character);
		while (m_BattleCharacters.front().first != targetName)
		{
			RotateCharacterQueue();
		}
	}
	m_pCurrentCharacter = m_BattleCharacters.front().second;
	NotifyCharacterSwitchIn();
	return S_OK;
}

CBattlePlayer* CBattlePlayer::Create()
{
	CBattlePlayer* Instance = new CBattlePlayer();
	if (FAILED(Instance->Initialize()))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

void CBattlePlayer::Free()
{
	__super::Free();
}
