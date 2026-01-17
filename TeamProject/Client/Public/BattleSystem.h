#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Client)
class CBattlePlayer;

// TimeScale 실행 시 시간관리용(저장용X)
typedef struct tagTimeScale 
{
	_bool	isScaled = { false };
	_float	fDuration = {};
	_float	fCurPos = {};
	_float	fLerpTimeRatio = {};
	_float	fScaleValue = { 1.f };
}TIME_SCALE;

typedef struct tagBattleVFX 
{
	_bool	isRunning = { false };
	_float	fVFXDuration = {};
	_float	fVFXCurrentPos = {};

	_float	fPlayerTimeScaleDuration = {};
	_float	fPlayerTimeScaleValue = {};
	_float	fMonsterTimeScaleDuration = {};
	_float	fMonsterTimeScaleValue = {};
	_float	fEffectTimeScaleDuration = {};
	_float	fEffectTimeScaleValue = {};

	_float	GetVFXTimeRatio() { return fVFXCurrentPos / fVFXDuration; }
}BATTLE_VFX;

class CBattleSystem final : public CBase
{
	DECLARE_SINGLETON(CBattleSystem)
public:
	enum class BATTLE_OBJ_TYPE { PLAYER, MONSTER, ENVOBJECT, END };
	enum class BATTLE_VFX_TYPE { EVADE , END};
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
	unordered_map<BATTLE_OBJ_TYPE, TIME_SCALE>* GetTimeScales() { return &m_TimeScales; }


public: //setter
	void	SetActive(_bool is) { m_isActive = is; }
	void	SpawnMosnter(const string& MonsterProtoTag, _float3 vSpawnPos);
	// 플레이어(캐릭터들) 로직 정해지기 전까지 임시
	void	SetPlayer(vector<OBJECT_HANDLE> hPlayers);
	// UI에서 캐릭터 선택시 부를 함수
	void	SetBattleCharacters(vector<CHARACTER> battleCharacters);
	void	SetBattlePlayer(class CBattlePlayer* pBattlePlayer) { m_pBattlePlayer = pBattlePlayer; }
	void	StartTimeScale(BATTLE_OBJ_TYPE eObjType, _float fDuration, _float fScale);
	void	StartShaderVFX(BATTLE_VFX_TYPE eVFXType, _float fDuration);

private:
	void	Update_BattleInfo();
	void	ClearBattleStage();
	void	CheckTimeScale(const _float dt);
	void	CheckVFX(const _float dt);
	void	ComputeVFXValue(const _float dt, _uint iVFXIndex);

private:
	_bool	m_isActive = { false };

	// 몬스터 세팅 테이블(CCT 정보, 각종 Status(HP, 공격력 등))
	unordered_map<string, MonsterCreationDesc>				m_MonsterCreationTables;
	// BATTLE_OBJ_TYPE 별로 생성된 오브젝트의 핸들 모음
	unordered_map<BATTLE_OBJ_TYPE, vector<OBJECT_HANDLE>>	m_Handles;
	// 매 업데이트때 1번씩 생성된 Battle 오브젝트의 정보를 담아둠
	unordered_map<BATTLE_OBJ_TYPE, vector<BATTLEOBJ_INFO>>	m_BattleObjInfos;
	// BATTLE_OBJ_TYPE 별로 타임 스케일
	unordered_map<BATTLE_OBJ_TYPE, TIME_SCALE>				m_TimeScales;
	// 쉐이더 효과
	vector<BATTLE_VFX>										m_BattleVFX;

	const _char* m_LayerTag[2] = { "Model_Layer", "Enemy_Layer" };

	// 배틀 플레이어
	class CBattlePlayer* m_pBattlePlayer = { nullptr };

public:
	virtual void Free() override;
};

inline CBattleSystem* BattleSystem() { return CBattleSystem::GetInstance(); }


NS_END
