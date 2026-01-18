#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

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
	/* PLAYER, MONSTER, ENVOBJECT
	* 현재 BattleSystem에 등록되어있는 오브젝트의 기본적인 정보들을 반환함 (읽기전용) */
	const vector<BATTLEOBJ_INFO>&	GetBattleObjects(BATTLE_OBJ_TYPE eType) const;
	/* PLAYER, MONSTER, ENVOBJECT
	* 현재 BattleSystem에 등록되어있는 오브젝트의 기본적인 정보 컨테이너를 복사반환함 */
	vector<BATTLEOBJ_INFO>			CopyBattleObjects(BATTLE_OBJ_TYPE eType);
	_int							GetPlayerParryingCount();


	/* 테스트용! 금방 지울예정 - 경인 */
	vector<TIME_SCALING>* GetTimeScales() { return &m_TimeScaling; }


public: //setter
	void	SetActive(_bool is) { m_isActive = is; }
	void	SpawnMosnter(const string& MonsterProtoTag, _float3 vSpawnPos);
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

public:	/* 광역 데미지 함수 : 안성빈 */
	void	TakeAreaDamage(const _float3& vCenter, _float fRadius, const HitDesc& hitDesc);
	void	TakeAllDamage(const HitDesc& hitDesc);

private:
	void	Update_BattleInfo();
	void	ClearBattleStage();
	void	CheckTimeScale(const _float dt);
	void	CheckVFX(const _float dt);

private:
	_bool	m_isActive = { false };

	// 몬스터 세팅 테이블(CCT 정보, 각종 Status(HP, 공격력 등))
	unordered_map<string, MonsterCreationDesc>				m_MonsterCreationTables;
	// BATTLE_OBJ_TYPE 별로 생성된 오브젝트의 핸들 모음
	unordered_map<BATTLE_OBJ_TYPE, vector<OBJECT_HANDLE>>	m_Handles;
	// 매 업데이트때 1번씩 생성된 Battle 오브젝트의 정보를 담아둠
	unordered_map<BATTLE_OBJ_TYPE, vector<BATTLEOBJ_INFO>>	m_BattleObjInfos;
	// BATTLE_VFX_TYPE 별로 쉐이더 효과 시작 시 필요한 데이터
	vector<BATTLE_VFX_DATA>		m_BattleVFXData;
	// BATTLE_OBJ_TYPE 별로 타임 스케일 관리용
	vector<TIME_SCALING>		m_TimeScaling;
	//쉐이더 효과 시간 관리용
	BATTLE_VFX					m_BattleVFX;
	//vector<BATTLE_VFX>			m_BattleVFX;

	const _char* m_LayerTag[2] = { "Model_Layer", "Enemy_Layer" };

	// 배틀 플레이어
	class CBattlePlayer* m_pBattlePlayer = { nullptr };

public:
	virtual void Free() override;
};

inline CBattleSystem* BattleSystem() { return CBattleSystem::GetInstance(); }


NS_END
