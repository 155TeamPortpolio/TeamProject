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

	struct BattleObjIndex /*인덱스 관리용 구조체*/
	{
		BATTLE_OBJ_TYPE objType = BATTLE_OBJ_TYPE::END; 
		_uint           indexInVector = 0;
	};
	

public:
	using BATTLE_OBJ_TYPE = ::BATTLE_OBJ_TYPE; /*이전 함수 호출*/
	using TypeVector = vector<BATTLEOBJ_INFO>; /*이전 함수 호출*/

private:
	CBattleSystem();
	virtual ~CBattleSystem() = default;

public:
	void Update();

public: //getter  
	_bool							GetActive() { return m_isActive; }
	_int							GetPlayerParryingCount();
	CBattlePlayer*					GetBattlePlayer() const {return m_pBattlePlayer;}
	OBJECT_HANDLE					GetCurCharacterHandle() const;
	const vector<BATTLEOBJ_INFO>&	GetBattleObjects(BATTLE_OBJ_TYPE eType) const;
	vector<BATTLEOBJ_INFO>			CopyBattleObjects(BATTLE_OBJ_TYPE eType);/*복사반환*/
	_bool							isMonsterCleared();

public: //setter
	void	SetActive(_bool is);
	void	SetPlayer(vector<OBJECT_HANDLE> hPlayers);
	void	SetBattlePlayer(class CBattlePlayer* pBattlePlayer) { m_pBattlePlayer = pBattlePlayer; }
	
public:
	void	StartGimmick(BATTLE_VFX_TYPE eVFXType);

public:
	void	TakeAreaDamage(const _float3& vCenter, _float fRadius, const HitDesc& hitDesc);
	void	TakeAreaDamage(const _float3& vCenter, _float fRadius, const _float3& vDir, _float fAngle, const HitDesc& hitDesc);
	void	TakeBoxDamage(const _float3& vCenter, const _float3& vHalfExtents, const _quaternion& qRotation, const HitDesc& hitDesc);
	void	TakeAllDamage(const HitDesc& hitDesc);

private:
	void CleanUp_Data();
	void Update_BattleInfo();
	void CheckVFX(const _float dt);

public:
	_bool	ExitBattleObject(BATTLE_OBJ_TYPE eObjType, OBJECT_HANDLE hObject);
	void	EnterBattleObject(BATTLE_OBJ_TYPE eObjType, OBJECT_HANDLE hObject);
	void	ExcludeBattleObject(BATTLE_OBJ_TYPE eObjType, OBJECT_HANDLE hObject);
	void	ClearBattleStage();

private:
	BATTLEOBJ_INFO*				FindBattleObjInfo(OBJECT_HANDLE objectHandle);
	vector<BATTLEOBJ_INFO>&		FindBattleType(BATTLE_OBJ_TYPE eObjType);
	_bool RemoveFromListSwapPop(TypeVector& infoList, _uint removeIndex, BATTLE_OBJ_TYPE objType);

private:
	_bool	m_isReady = { false };
	_bool	m_isActive = { false };

private:
	class CBattlePlayer* m_pBattlePlayer = { nullptr };// 배틀 플레이어
	class CBattleFXFlow* m_pFXFlow = { nullptr };
	unordered_map<OBJECT_HANDLE, BattleObjIndex, hOBJECT_HASH, hOBJECT_FUNCTOR> m_BattleObjIndex;
	unordered_map<BATTLE_OBJ_TYPE, TypeVector>	m_BattleObjInfos;
	unordered_map<BATTLE_OBJ_TYPE, TypeVector>	m_BattleSnapShots;

public:
	_bool	IsUseInspector() { return m_isUseInspector; }
	void	SetUseInspector(_bool is) { m_isUseInspector = is; }

private:
	_bool	m_isUseInspector = { false };

public:
	virtual void Free() override;
	/*For Debug*/
	void	SpawnMosnter(const string& MonsterProtoTag, _float3 vSpawnPos, _float3 vRot = {});
};

inline CBattleSystem* BattleSystem() { return CBattleSystem::GetInstance(); }


NS_END
