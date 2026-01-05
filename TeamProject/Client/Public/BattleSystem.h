#pragma once
#include "Base.h"


NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Client)

class CBattleSystem final : public CBase
{
	DECLARE_SINGLETON(CBattleSystem)
public:
	enum class BATTLE_OBJ_TYPE { PLAYER, MONSTER, ENVOBJECT, END };

	struct MonsterCreationDesc {
		string	ProtoTag = {};
		string	DisplayName = {};
		_float	CCT_fHeight = {};
		_float	CCT_fRadius = {};
		_float3 CCT_vPos = {};

		_uint	CCT_iCollisionMask = 0xFFFFFFFF;
		_bool	CCT_bAutoFit = { false };
		COLLISION_GROUP CCT_eGroup = COLLISION_GROUP::MONSTER;
	};

	typedef struct BattleObjectInfo {
		string			TagInstanceName = {};	// 오브젝트 인스턴스 이름
		OBJECT_HANDLE	refHandle;				// 오브젝트 핸들
		_bool			isOnField = { false };	// 현재 필드 위에 소환되어 있는지
		_float3			vPos = {};				// 현재 오브젝트의 위치
	}BATTLEOBJ_INFO;

private:
	CBattleSystem();
	virtual ~CBattleSystem() = default;

public:
	void Update();

public:
	_bool	GetActive() { return m_isActive; }
	/* PLAYER, MONSTER, ENVOBJECT
	* 현재 BattleSystem에 등록되어있는 오브젝트의 기본적인 정보들을 반환함 (읽기전용) */
	const vector<BATTLEOBJ_INFO>&	GetBattleObjects(BATTLE_OBJ_TYPE eType) const;
	/* PLAYER, MONSTER, ENVOBJECT
	* 현재 BattleSystem에 등록되어있는 오브젝트의 기본적인 정보 컨테이너를 복사반환함 */
	vector<BATTLEOBJ_INFO>			CopyBattleObjects(BATTLE_OBJ_TYPE eType);

public:
	void	SetActive(_bool is) { m_isActive = is; }
	HRESULT	LoadMonsterCreationTable(const string& csvPath);
	void	SpawnMosnter(const string& MonsterProtoTag, _float3 vSpawnPos);
	
private:
	_bool	m_isActive = { false };

	// 몬스터 세팅 테이블(CCT 정보, 각종 Status(HP, 공격력 등))
	unordered_map<string, MonsterCreationDesc>				m_MonsterCreationTables;
	// BATTLE_OBJ_TYPE 별로 생성된 오브젝트의 핸들 모음
	unordered_map<BATTLE_OBJ_TYPE, vector<OBJECT_HANDLE>>	m_Handles;
	// 매 업데이트때 1번씩 생성된 Battle 오브젝트의 정보를 담아둠
	unordered_map<BATTLE_OBJ_TYPE, vector<BATTLEOBJ_INFO>>	m_BattleObjInfos;

public:
	virtual void Free() override;
};

NS_END
