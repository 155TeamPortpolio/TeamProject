#include "pch.h"
#include "FieldPlayer.h"

#include "GameObject.h"
#include "GameInstance.h"
#include "Helper_Func.h"
#include "CharacterController.h"
#include "StateMachine.h"

#include "FieldSystem.h"

#include "FieldCharacter.h"
#include "Belle.h"

#include "Camera.h"


CFieldPlayer::CFieldPlayer()
{
}

OBJECT_HANDLE CFieldPlayer::GetCurCharacterHandle()
{
	return m_pCurrentCharacter->Get_Handle(); 
}

void CFieldPlayer::Active_Field()
{
	m_pCurrentCharacter->Active_Character();
}

void CFieldPlayer::DeActive_Field()
{
	m_pCurrentCharacter->DeActive_Character();
}

HRESULT CFieldPlayer::Initialize()
{
	Initialize_CharacterPrototype();
	m_pCurrentCharacter = Create_Character();

	CFieldSystem::GetInstance()->SetFieldPlayer(this);
	return S_OK;
}

void CFieldPlayer::Priority_Update(_float dt)
{
	if (m_pCurrentCharacter == nullptr)
		return;
	Reset_State(dt);
}

void CFieldPlayer::Update(_float dt)
{
	Update_Input(dt);
}

void CFieldPlayer::Late_Update(_float dt)
{
}

HRESULT CFieldPlayer::Initialize_CharacterPrototype()
{
	auto pProto = PrototypeManager();
	if (FAILED(pProto->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_Belle", CBelle::Create())))
		return E_FAIL;

	return S_OK;
}

CFieldCharacter* CFieldPlayer::Create_Character()
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

	auto Belle = Builder::Create_Object({ G_GlobalLevelKey, "Proto_GameObject_Belle" })
		.Position(_float3(3.f, 37.f, 0.f))
		.CharacterController(characterCCT)
		.Build("Belle");
	ObjectManager()->Add_Object(Belle, {LevelManager()->Get_NowLevelKey(), "Model_Layer"});

	return dynamic_cast<CFieldCharacter*>(Belle);
}

HRESULT CFieldPlayer::Clear_Character()
{
	m_pCurrentCharacter = nullptr;
	return S_OK;
}

void CFieldPlayer::Reset_State(_float dt)
{
	m_pCurrentCharacter->Reset_State();
}

void CFieldPlayer::Update_Input(_float dt)
{
	if (!m_pCurrentCharacter || m_bLockInput) return;
	Update_Movement(dt);
	Update_Interact(dt);
}

void CFieldPlayer::Update_Interact(_float dt)
{
	if (InputDevice()->Key_Tap('F')) m_pCurrentCharacter->On_Interact();
}

void CFieldPlayer::Update_Movement(_float dt)
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
	Process_Movement(dt);
}

void CFieldPlayer::Process_Movement(_float dt)
{
	CFieldCharacter::InputInfo inputInfo;
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

CFieldPlayer* CFieldPlayer::Create()
{
	CFieldPlayer* Instance = new CFieldPlayer();
	return Instance;
}

void CFieldPlayer::Free()
{
	__super::Free();
}
