#pragma once
#include "Engine_Defines.h"

namespace Client {

	typedef struct BattleObjectInfo {
		string			TagInstanceName = {};	// 오브젝트 인스턴스 이름
		OBJECT_HANDLE	hObject;				// 오브젝트 핸들
		_bool			isOnField = { false };	// 현재 필드 위에 소환되어 있는지
		_float3			vPos = {};				// 현재 오브젝트의 위치
		_float			fRadius = {};			// CCT 로컬 원점으로부터 반지름
		void Reset() {
			TagInstanceName.clear();
			hObject.Reset();
			isOnField = false;
			vPos = {};
			fRadius = { 0.f };
		}
	}BATTLEOBJ_INFO;

	struct MonsterCreationDesc {
		string	ProtoTag = {};
		string	DisplayName = {};
		_uint	MonsterID = {};
		_float	CCT_fHeight = {};
		_float	CCT_fRadius = {};
		_float	iMaxHP = {};

		_uint	CCT_iCollisionMask = 0xFFFFFFFF;
		_bool	CCT_bAutoFit = { false };
		COLLISION_GROUP CCT_eGroup = COLLISION_GROUP::MONSTER;
	};

	struct PlayerDesc {
		string	strPlayerName = {};
		_float	MaxHP = {};
		_float	Attack = {};
		_float	Defend = {};
		_float	SpecialAttack = {};
		_float	Ultimate = {};
	};

	struct NpcIDDesc {
		wstring Name = {};
		string	StartDialogueID = {};
		_int DialogueNum = {};
		_int SequenceNum = {};
	};

	struct NpcDialogueDesc {
		string			DialogueID = {};
		wstring			Name = {};
		_uint			SequenceID = {};
		Speaker			Speaker;
		DayPhase		DayPhase;
		DialogueType	DialogueType;
		_bool			Repeat;
		wstring			Text;
		DialogueResult	Result;
		_uint			ChoiceNum;
		string			Choice_ID1;
		string			Choice_ID2;
		string			Choice_ID3;
		_uint			NextSequenceID = {};
	};

	struct ChoiceDesc
	{
		string			ChoiceID;
		wstring			Text;
		DialogueResult	Result;
		string			Next_DialogueID;
		_int			Next_SequeceID;
		string			ValueType;
		string			ValueName;
		variant<monostate, _bool, _int, _float, string> Value;
	};

	typedef struct tagMapDataFilePacket {
		string		TagDataFormat = {};			// MapData, EntityData, BattleData
		string		TagSlotFormat = {};			// Base, Physics, Effect, SlotA, SlotB ...
		string		TagArea = {};				// 지형 이름
		string		TagDataFileKey = {};		// 파일 이름
		string		TagDataFilePath = {};		// json 파일 경로
		int			iVersion = { -1 };
	}MapData_Path_Packet;

	// 타격 정보
	struct HitDesc
	{
		HIT_TYPE    eHitType = HIT_TYPE::ONCE;
		DAMAGE_TYPE eDamageType = DAMAGE_TYPE::NORMAL;
		_float      fDamage = 0.f;
		_float      fInterval = 0.f;
		_uint       iMaxCount = 1;
		_float      fEnergyCharge = 1.f;
		_float      fDecibelCharge = 10.f;

		HitDesc& Type(HIT_TYPE e) { eHitType = e; return *this; }
		HitDesc& Damage(_float f, DAMAGE_TYPE e = DAMAGE_TYPE::NORMAL) { fDamage = f; eDamageType = e; return *this; }
		HitDesc& Interval(_float f) { fInterval = f; return *this; }
		HitDesc& MaxCount(_uint i) { iMaxCount = i; return *this; }
		HitDesc& Charge(_float fEnergy, _float fDecibel) { fEnergyCharge = fEnergy; fDecibelCharge = fDecibel; return *this; }
	};

	// 게이지 지연및 보간 상태
	typedef struct tagGaugeDelayDesc {
		_float fCurRatio = { 1.f };
		_float fTargetRatio = { 1.f };
		_float fDelayTimer = {};
		_float fDelayTime = { 0.3f };
		_bool isDelay = {};
	}GAUGE_DELAY_DESC;

	typedef struct tagSpawnDesc
	{
		_int Colony{};
		_int MonsterID{};
		_int Count{};
	}SPAWN_MONSTER_DESC;
	using EncounterTable = unordered_map<_int, vector<SPAWN_MONSTER_DESC>>;
	using StageTypeTable = unordered_map<_int, EncounterTable>;

	typedef struct tagFieldRoomDesc {
		string roomKey;
		_bool persistent = { false };
	}ROOM_DESC;

	typedef struct tagRamenAttributeDesc {
		string	strAttributeID;
		wstring strAttributeName;
		_int	iAttributeValue;
	}RAMEN_ATTRIBUTE;

	typedef struct tagRamenDesc	{
		string	strID;
		wstring strName;
		_uint	iPrice;
		_int	iOrder;
		vector<RAMEN_ATTRIBUTE> attributes;
	}RAMEN_DESC;

	typedef struct tagWeaponDesc {
		_int		ID;
		GachaGrade	Grade;
		string		strModel;
		string		strMaterial;
		string		strTexture;
		_float		RotX;
		_float		RotY;
		_float		RotZ;
		_float		RotW;
	}WEAPON_DESC;

	typedef struct tagTVDesc {
		string		strName;
		_int		Col;
		_int		Row;
		_int		MaxFrame;
	}TV_DESC;

	typedef struct tagCloudDesc {
		_float3 skyColor = {};
		_float3 cloudColor = {};
	}CLOUD_DESC;
}