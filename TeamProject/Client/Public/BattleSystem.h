#pragma once
#include "Base.h"
#include "MapData_Defines.h"
#include "BattleFXFlow.h"

NS_BEGIN(Client)
class CBattlePlayer;
#pragma region Struct
#pragma endregion

class CBattleSystem final : public CBase
{
	DECLARE_SINGLETON(CBattleSystem)
public:
	using BATTLE_OBJ_TYPE = ::BATTLE_OBJ_TYPE; /*이전 함수 호출*/
	using BattleInfo = vector<BATTLEOBJ_INFO>; /*이전 함수 호출*/

private:
	CBattleSystem();
	virtual ~CBattleSystem() = default;

public:
	void Update();

public: //getter  
	CBattlePlayer*					GetBattlePlayer() const {return m_pBattlePlayer;}
	OBJECT_HANDLE					GetCurCharacterHandle() const;
	_bool							GetActive() { return m_isActive; }
	const vector<BATTLEOBJ_INFO>&	GetBattleObjects(BATTLE_OBJ_TYPE eType) const;
	vector<BATTLEOBJ_INFO>			CopyBattleObjects(BATTLE_OBJ_TYPE eType);/*복사반환*/
	_int							GetPlayerParryingCount();


public: //setter
	void	SetActive(_bool is);
	/* PrefabIndex는 같은 몬스터 스폰포인트를 사용하지만 다른 몬스터 종류를 사용할 때를 위해 빼놓음. 1xxx,2xxx 등으로 나눠서 MonsterSpawn.csv에 저장된걸 로드함*/
	void	ReadyBattle(const string& tagArea, _uint iPrefabIndex = 1);
	void	ReadyBattle(const string& tagArea, _uint StageNumber, _uint iPrefabIndex);
	
	void	SpawnMosnter(const string& MonsterProtoTag, _float3 vSpawnPos, _float3 vRot = {});
	void	SpawnMosnterFromPool(const string& MonsterProtoTag, _float3 vSpawnPos, _float3 vRot);


	// 플레이어(캐릭터들) 로직 정해지기 전까지 임시
	void	SetPlayer(vector<OBJECT_HANDLE> hPlayers);
	void	SetBattlePlayer(class CBattlePlayer* pBattlePlayer) { m_pBattlePlayer = pBattlePlayer; }

public:
	// 기믹 성공 시 호출 함수(Evade, 지원공격 등)
	void	StartGimmick(BATTLE_VFX_TYPE eVFXType);
	void	StartTimeScale(BATTLE_OBJ_TYPE eObjType, 
			_float fDuration, 
			_float fScale, 
			_float fStartLerpTime = 0.f, 
			_float fEndLerpTime = 0.f);

public:
	void	TakeAreaDamage(const _float3& vCenter, _float fRadius, const HitDesc& hitDesc);
	void	TakeAllDamage(const HitDesc& hitDesc);
	_bool	ExitBattleObject(BATTLE_OBJ_TYPE eObjType, OBJECT_HANDLE hObject);
	void	EnterBattleObject(BATTLE_OBJ_TYPE eObjType, OBJECT_HANDLE hObject);

public:
	_bool isMonsterCleared();

private:
	void	ClearBattleStage();
	void	Update_BattleInfo();
	void	CheckVFX(const _float dt);

private:
	void StartVfx_Evade();

private:
	_bool	m_isReady = { false };
	_bool	m_isActive = { false };
private:
	class CBattlePlayer* m_pBattlePlayer = { nullptr };// 배틀 플레이어
	class CBattleFXFlow* m_pFXFlow = { nullptr };
	_float3 m_FXAddColor = {};

	unordered_map<BATTLE_OBJ_TYPE, BattleInfo>	m_BattleObjInfos;	// 매 업데이트때 1번씩 생성된 Battle 오브젝트의 정보를 담아둠
	
	/*Map 구조 변경 후 재적용 예정*/
	BATTLE_FIELD_DATA			m_BattleFieldData = {};// 현재 Stage에 셋팅 된 BattlePoint 정보
	vector<OBJECT_HANDLE>		m_SpawnerHandles;// 스포너 핸들
private: /*현재 사용 X*/
	const _char* m_LayerTag[3] = { "Model_Layer", "Enemy_Layer" ,"None"};
	unordered_map<BATTLE_OBJ_TYPE, vector<OBJECT_HANDLE>>	m_Handles;			// BATTLE_OBJ_TYPE 별로 생성된 오브젝트의 핸들 모음

public:
	virtual void Free() override;
};

inline CBattleSystem* BattleSystem() { return CBattleSystem::GetInstance(); }


NS_END
