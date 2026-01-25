#pragma once
#include "Base.h"
#include "BattleSystem_Struct.h"

NS_BEGIN(Client)
class CBattlePlayer;
enum class BATTLE_VFX_TYPE { EVADE, END };

#pragma region Struct
// TimeScale 실행 시 시간관리용(저장용X)
typedef struct tagTimeScale 
{
	_bool	isRunning = { false };
	_float	fDuration = {};
	_float	fCurPos = {};
	_float	fScaleValue = { 1.f };
	
	_float2	vStartLerpTime = {};	/* x : duration, y : curpos */
	_float2	vEndLerpTime = {};		/* x : duration, y : curpos */
	//EaseType eEaseType = EaseType::None;

	_float	GetTimeRatio() 
	{ 
		if (0 > fCurPos)	
			return 0.f;		// 0 나누기 방지
		return clamp(fCurPos / fDuration, 0.f, 1.f); 
	}              
}TIME_SCALING;

typedef struct tagTimeScaleData
{
	_float	fDuration = {};
	_float	fValue = { 1.f };			// dt에 스케일 할 값 (dt * fValue)
	_float	fStartLerpTime = { 0.f };	// 스케일 시작 보간 비율 (0~1)
	_float	fEndLerpTime = { 0.2f };	// 스케일 종료 보간 비율 (0~1)
	//EaseType eEaseType = EaseType::None;
}TIME_SCALE_DATA;

/* fDuration, fValue, fStartLerpTime */  
typedef struct tagBattleVFX
{
	BATTLE_VFX_TYPE eVFXType = BATTLE_VFX_TYPE::END;
	_bool	isRunning = {};
	_float	fDuration = {};
	_float	fCurPos = {};
	_float3 vLerpColor = {};
	_float	GetTimeRatio()				
	{
		if (0 > fCurPos)
			return 0.f;		// 0 나누기 방지
		return clamp(fCurPos / fDuration, 0.f, 1.f);
	}
}BATTLE_VFX;

typedef struct tagBattleVFXData
{
	_float			fVFXDuration = {};			/* 화면 색 먹이는 시간 */
	_float			fBlurDuration = {};			/* 블러 먹이는 시간 */
	TIME_SCALE_DATA tPlayerTimeScale = {};		/* Duration, ScaleValue(0 < value < 1), StartLerpTime, EndLerpTime */
	TIME_SCALE_DATA tMonsterTimeScale = {};		/* Duration, ScaleValue(0 < value < 1), StartLerpTime, EndLerpTime */
	TIME_SCALE_DATA tEffectTimeScale = {};		/* Duration, ScaleValue(0 < value < 1), StartLerpTime, EndLerpTime */
}BATTLE_VFX_DATA;
#pragma endregion

class CBattleSystem final : public CBase
{
	DECLARE_SINGLETON(CBattleSystem)
public:
	enum class BATTLE_OBJ_TYPE { PLAYER, MONSTER, ENVOBJECT, END };
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
	void	SetActive(_bool is) { m_isActive = is; }
	
	/* PrefabIndex는 같은 몬스터 스폰포인트를 사용하지만 다른 몬스터 종류를 사용할 때를 위해 빼놓음. 1xxx,2xxx 등으로 나눠서 MonsterSpawn.csv에 저장된걸 로드함*/
	void	ReadyBattle(const string& tagArea, _uint iPrefabIndex = 1);
	
	void	SpawnMosnter(const string& MonsterProtoTag, _float3 vSpawnPos, _float3 vRot = {});
	// 보류
	void	SpawnMosnterFromPool(const string& MonsterProtoTag, _float3 vSpawnPos, _float3 vRot);
	_bool	ExitBattleObject(BATTLE_OBJ_TYPE eObjType, OBJECT_HANDLE hObject);
	// 플레이어(캐릭터들) 로직 정해지기 전까지 임시
	void	SetPlayer(vector<OBJECT_HANDLE> hPlayers);
	// UI에서 캐릭터 선택시 부를 함수
	void	SetBattleCharacters(vector<CHARACTER> battleCharacters);
	void	SetBattlePlayer(class CBattlePlayer* pBattlePlayer) { m_pBattlePlayer = pBattlePlayer; }
	// 기믹 성공 시 호출 함수(Evade, 지원공격 등)
	void	StartGimmick(BATTLE_VFX_TYPE eVFXType);

	// 아래 함수들은 추후에 private로 넘길 예정
	void	StartTimeScale(BATTLE_OBJ_TYPE eObjType, _float fDuration, _float fScale, _float fStartLerpTime = 0.f, _float fEndLerpTime = 0.f);
	void	StartShaderVFX(BATTLE_VFX_TYPE eVFXType);


public:
	void	TakeAreaDamage(const _float3& vCenter, _float fRadius, const HitDesc& hitDesc);
	void	TakeAllDamage(const HitDesc& hitDesc);

public:
	_bool isMonsterCleared();

private:
	void	Update_BattleInfo();
	void	ClearBattleStage();
	void	CheckTimeScale(const _float dt);
	void	CheckVFX(const _float dt);


public:
	vector<TIME_SCALING>* GetTimeScales() { return &m_TimeScaling; }/* 테스트용 */

private:
	class CBattlePlayer* m_pBattlePlayer = { nullptr };// 배틀 플레이어


private:
	_bool	m_isReady = { false };
	_bool	m_isActive = { false };

	unordered_map<BATTLE_OBJ_TYPE, vector<OBJECT_HANDLE>>	m_Handles;// BATTLE_OBJ_TYPE 별로 생성된 오브젝트의 핸들 모음
	unordered_map<BATTLE_OBJ_TYPE, vector<BATTLEOBJ_INFO>>	m_BattleObjInfos;// 매 업데이트때 1번씩 생성된 Battle 오브젝트의 정보를 담아둠
	vector<BATTLE_VFX_DATA>		m_BattleVFXData;// BATTLE_VFX_TYPE 별로 쉐이더 효과 시작 시 필요한 데이터
	vector<TIME_SCALING>		m_TimeScaling;// BATTLE_OBJ_TYPE 별로 타임 스케일 관리용

	/**/
	BATTLE_VFX					m_BattleVFX;//쉐이더 효과 시간 관리용
	BATTLE_FIELD_DATA			m_BattleFieldData = {};// 현재 Stage에 셋팅 된 BattlePoint 정보
	const _char* m_LayerTag[2] = { "Model_Layer", "Enemy_Layer" };

private:
	vector<OBJECT_HANDLE>		m_SpawnerHandles;// 스포너 핸들
	
public:
	virtual void Free() override;
};

inline CBattleSystem* BattleSystem() { return CBattleSystem::GetInstance(); }


NS_END
