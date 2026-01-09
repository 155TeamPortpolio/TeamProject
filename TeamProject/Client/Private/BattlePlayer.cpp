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

		m_BattleCharacters[strCharacter] =
			dynamic_cast<CCharacter*>(CreateBattleCharacter(character));
		m_CharacterHandles.push_back(
			m_BattleCharacters[strCharacter]->Get_Handle());
	}
	CBattleSystem::GetInstance()->SetPlayer(m_CharacterHandles);
}

HRESULT CBattlePlayer::Initialize()
{
	CBattleSystem::GetInstance()->SetBattlePlayer(this);
	Initialize_CharacterPrototype();
	//auto PlayerDesc = CDataBase::GetInstance()->GetPlayerDesc("Miyabi");
	//auto LVDesc = CDataBase::GetInstance()->GetLevelDesc(10);

	vector<CHARACTER> BattleCharacters = {CHARACTER::JaneDoe/*, CHARACTER::Corin*/};
	SetBattleCharacters(BattleCharacters);

	m_pCurrentCharacter = m_BattleCharacters["JaneDoe"];
	return S_OK;
}

void CBattlePlayer::Priority_Update(_float dt)
{
}

void CBattlePlayer::Update(_float dt)
{
}

void CBattlePlayer::Late_Update(_float dt)
{
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
