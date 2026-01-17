#include "pch.h"
#include "BattleSystem.h"
#include "GameInstance.h"
#include "Helper_Func.h"
#include "CharacterController.h"
#include "BattlePlayer.h"
#include "DataBase.h"
#include "FieldSystem.h"

IMPLEMENT_SINGLETON(CBattleSystem)

CBattleSystem::CBattleSystem()
{
	// 빈 값 채우기
	for (_int i = 0; i < static_cast<_int>(BATTLE_OBJ_TYPE::END); ++i) {
		auto eType = static_cast<BATTLE_OBJ_TYPE>(i);
		m_BattleObjInfos.emplace(eType, vector<BATTLEOBJ_INFO>{});
	}

	m_BattleVFX.resize(ENUM(BATTLE_VFX_TYPE::END), {});
	m_BattleVFX[ENUM(BATTLE_VFX_TYPE::EVADE)].fVFXDuration = 5.f;
	//m_BattleVFX[ENUM(BATTLE_VFX_TYPE::EVADE)].fLerpTimeRatio = 0.3f;

}

void CBattleSystem::Update()
{
	if (false == m_isActive)
		return;

	const _float dt = CGameInstance::GetInstance()->Get_EngineDeltaTime();

	CheckTimeScale(dt);

	Update_BattleInfo();
}


OBJECT_HANDLE CBattleSystem::GetCurCharacterHandle() const
{
	return m_pBattlePlayer->GetCurCharacterHandle();
}

const vector<BATTLEOBJ_INFO>& CBattleSystem::GetBattleObjects(BATTLE_OBJ_TYPE eType) const
{
	return m_BattleObjInfos.at(eType);
}

vector<BATTLEOBJ_INFO> CBattleSystem::CopyBattleObjects(BATTLE_OBJ_TYPE eType)
{
	return m_BattleObjInfos[eType];
}

_int CBattleSystem::GetPlayerParryingCount()
{
	if (nullptr == m_pBattlePlayer)
		return -1;

	return m_pBattlePlayer->GetParryingCount();
}

void CBattleSystem::SpawnMosnter(const string& MonsterProtoTag, _float3 vSpawnPos)
{
	MonsterCreationDesc MonsterTableDesc = CDataBase::GetInstance()->GetMonsterDesc(MonsterProtoTag);
	if (true == MonsterTableDesc.ProtoTag.empty())
		return;

	CCT_DESC MonsterCCT;
	MonsterCCT.eGroup = COLLISION_GROUP::MONSTER;
	MonsterCCT.iCollisionMask = ENUM(COLLISION_GROUP::PLAYER) | ENUM(COLLISION_GROUP::COMMON) | ENUM(COLLISION_GROUP::PLAYER_ATTACK);
	MonsterCCT.bAutoFit = false;
	MonsterCCT.fHeight = MonsterTableDesc.CCT_fHeight;
	MonsterCCT.fRadius = MonsterTableDesc.CCT_fRadius;
	MonsterCCT.vPos = vSpawnPos;
	MonsterCCT.vPos.y += MonsterCCT.fHeight;
	
	const string NowLevel = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey();

	auto pMonster = Builder::Create_Object({ NowLevel,MonsterTableDesc.ProtoTag })
		.CharacterController(MonsterCCT)
		.Build(MonsterTableDesc.DisplayName);

	if (nullptr == pMonster)
		return;
	
	CGameInstance::GetInstance()->Get_ObjectMgr()->Add_Object(pMonster, { NowLevel, "Enemy_Layer"});

	m_Handles[BATTLE_OBJ_TYPE::MONSTER].push_back(pMonster->Get_Handle());
}

void CBattleSystem::SetPlayer(vector<OBJECT_HANDLE> hPlayers)
{
	for (auto& hPlayer : hPlayers)
	{
		if (hPlayer.isValid())
			m_Handles[BATTLE_OBJ_TYPE::PLAYER].push_back(hPlayer);
	}
}

void CBattleSystem::SetBattleCharacters(vector<CHARACTER> battleCharacters)
{
	m_pBattlePlayer->SetBattleCharacters(battleCharacters);
}

void CBattleSystem::StartTimeScale(BATTLE_OBJ_TYPE eObjType, _float fDuration, _float fScale)
{
	// PLAYER, MONSTER 가 아닐때, 예외
	if (ENUM(BATTLE_OBJ_TYPE::ENVOBJECT) <= ENUM(eObjType))
		return;

	// 값들이 음수일때, 예외
	if (fDuration < 0.f || fScale < 0.f)
		return;

	string tagNowLevel = LevelManager()->Get_NowLevelKey();

	ObjectManager()->Set_LayerTimeScale({ tagNowLevel , m_LayerTag[ENUM(eObjType)] }, fScale);

	m_TimeScales[eObjType].isScaled = true;
	m_TimeScales[eObjType].fCurPos = 0.f;
	m_TimeScales[eObjType].fDuration = fDuration;
	m_TimeScales[eObjType].fScaleValue = fScale;

}

void CBattleSystem::StartShaderVFX(BATTLE_VFX_TYPE eVFXType, _float fDuration)
{
	// 처음에
	//RenderSystem()->Apply_RadialBlur(fDuration);
	//RenderSystem()->Register_AddictiveColor(&m_vLerpColor);
	//여기서 m_vLerpColor를 멤버변수로 가지고 업데이트때 색상을 보간주든 해서 효과를 부여.
	//끝날때
	//RenderSystem()->UnRegister_AddictiveColor();
	
	if (true == m_BattleVFX[ENUM(eVFXType)].isRunning)
		return;

	//StartTimeScale(BATTLE_OBJ_TYPE::PLAYER)
	






	m_BattleVFX[ENUM(eVFXType)].isRunning = true;

}

void CBattleSystem::Update_BattleInfo()
{
	for (_int i = 0; i < static_cast<_int>(BATTLE_OBJ_TYPE::END); ++i) {
		auto eType = static_cast<BATTLE_OBJ_TYPE>(i);
		m_BattleObjInfos[eType].clear();

		size_t test = m_Handles[eType].size();

		for (size_t j = 0; j < m_Handles[eType].size(); ++j) {
			auto handle = m_Handles[eType][j];
			if (false == handle.isValid())
				continue;

			CGameObject* pObject = m_Handles[eType][j].Get();

			_float4x4 mObjWorld = pObject->Get_Component<CTransform>()->Get_WorldMatrix();

			BATTLEOBJ_INFO info = {};
			info.TagInstanceName = m_Handles[eType][j].Get()->Get_InstanceName();
			info.hObject = m_Handles[eType][j];
			info.vPos = { mObjWorld._41, mObjWorld._42,mObjWorld._43 };
			info.fRadius = pObject->Get_Component<CCharacterController>()->Get_Radius();
			info.isOnField = true;

			m_BattleObjInfos[eType].push_back(info);
		}
	}
}

void CBattleSystem::ClearBattleStage()
{
	for (auto& Pair : m_Handles) 
		Pair.second.clear();

	for (auto& Pair : m_BattleObjInfos)
		Pair.second.clear();
}

void CBattleSystem::CheckTimeScale(const _float dt)
{
	for (auto& Pair : m_TimeScales) 
	{
		if (ENUM(BATTLE_OBJ_TYPE::ENVOBJECT) <= ENUM(Pair.first))
			continue;
		
		auto& TimeScale = Pair.second;

		if (true == TimeScale.isScaled)
		{
			TimeScale.fCurPos += dt;

			if (TimeScale.fCurPos >= TimeScale.fDuration)
			{
				ObjectManager()->Reset_LayerTimeScale({ LevelManager()->Get_NowLevelKey(), m_LayerTag[ENUM(Pair.first)] });
				TimeScale.isScaled = false;
				TimeScale.fCurPos = 0.f;
			}
		}
	}
}

void CBattleSystem::CheckVFX(const _float dt)
{
	for (size_t i = 0; i < ENUM(BATTLE_VFX_TYPE::END); i++)
	{
		if (false == m_BattleVFX[i].isRunning)
			continue;

		ComputeVFXValue(dt,i);

	}


}

void CBattleSystem::ComputeVFXValue(const _float dt, _uint iVFXIndex)
{
	BATTLE_VFX_TYPE eType = static_cast<BATTLE_VFX_TYPE>(iVFXIndex);

	m_BattleVFX[iVFXIndex].fVFXCurrentPos += dt;

	_float fRatio = m_BattleVFX[iVFXIndex].GetVFXTimeRatio();

	if (fRatio > 0.99f)
	{
		RenderSystem()->UnRegister_AddictiveColor();
		m_BattleVFX[iVFXIndex].fVFXCurrentPos = {};
		m_BattleVFX[iVFXIndex].isRunning = false;

	}


	switch (eType)
	{
	case Client::CBattleSystem::BATTLE_VFX_TYPE::EVADE:
	{
		_float normalizedT = 1 - fRatio;
		_float pingpongT = (normalizedT < 0.5f) ? (normalizedT * 2.f) : (2.f - normalizedT * 2.f);
		_float EaseT = Math::ApplyEase(EaseType::InOutSine, pingpongT);

		_float3 vTarget = { 0.1, 0.54, 0.58 };

		_vector vZero = XMVectorZero();
		_vector	vTargetColor = XMLoadFloat3(&vTarget);
		_float3 vLerpColor{};
		XMStoreFloat3(&vLerpColor, XMVectorLerp(vZero, vTargetColor, EaseT));

		RenderSystem()->Register_AddictiveColor(&vLerpColor);


		break;
	}
	}
}

void CBattleSystem::Free()
{
	__super::Free();

}
