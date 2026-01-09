#include "pch.h"
#include "BattlePlayer.h"

//gameobject
#include "GameObject.h"

//helper class
#include "GameInstance.h"
#include "BattleSystem.h"
#include "DataBase.h"
#include "Helper_Func.h"

//character class
#include "Character.h"
#include "Corin.h"
#include "JaneDoe.h"


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
	m_pCurrentCharacter->Update_Input(dt);
	Update_Input(dt);
}

void CBattlePlayer::Update(_float dt)
{
}

void CBattlePlayer::Late_Update(_float dt)
{
}

void CBattlePlayer::Update_Input(_float dt)
{
	if (InputDevice()->Key_Tap(VK_SPACE))
	{
		if (true/*m_bCanSwitch()*/)
		{
			NotifyCharacterSwitchOut();
			SwitchCharacter();
			NotifyCharacterSwitchIn();
		}
	}
}

HRESULT CBattlePlayer::Initialize_CharacterPrototype()
{
	auto pProto = PrototypeManger();

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
		ObjectManger()->Add_Object(JaneDoe, { "Test_Level", "Model_Layer" });
		return JaneDoe;
	}
	case CHARACTER::Corin:
	{
		auto Corin = Builder::Create_Object({ "Test_Level", "Proto_GameObject_Corin" })
			.Position(_float3(3.f, 0.f, 0.f))
			.CharacterController(characterCCT)
			.Build("Corin");
		ObjectManger()->Add_Object(Corin, { "Test_Level", "Model_Layer" });
		return Corin;
	}
	}
	return nullptr;
}

void CBattlePlayer::NotifyCharacterSwitchIn()
{
	//m_pCurrentCharacter->SwitchIn();
}

void CBattlePlayer::NotifyCharacterSwitchOut()
{
	//m_pCurrentCharacter->SwitchOut();
}

void CBattlePlayer::RotateCharacterQueue()
{
	auto ReplacedPlayer = m_BattleCharacters.front();
	m_BattleCharacters.pop();
	m_BattleCharacters.push(ReplacedPlayer);
}

HRESULT CBattlePlayer::SwitchCharacter(CHARACTER character)
{
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
