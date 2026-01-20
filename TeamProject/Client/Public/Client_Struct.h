#pragma once
#include "Engine_Defines.h"

namespace Client {

	typedef struct BattleObjectInfo {
		string			TagInstanceName = {};	// 오브젝트 인스턴스 이름
		OBJECT_HANDLE	hObject;				// 오브젝트 핸들
		_bool			isOnField = { false };	// 현재 필드 위에 소환되어 있는지
		_float3			vPos = {};				// 현재 오브젝트의 위치
		_float			fRadius = {};			// CCT 로컬 원점으로부터 반지름
	}BATTLEOBJ_INFO;

	struct MonsterCreationDesc {
		string	ProtoTag = {};
		string	DisplayName = {};
		_float	CCT_fHeight = {};
		_float	CCT_fRadius = {};
		_float	iMaxHP = {};

		_uint	CCT_iCollisionMask = 0xFFFFFFFF;
		_bool	CCT_bAutoFit = { false };
		COLLISION_GROUP CCT_eGroup = COLLISION_GROUP::MONSTER;
	};

	struct PlayerDesc {
		string	strPlayerName = {};
		_uint	LV = {};
		_float	SpecialAttack = {};
		string	ExType = {};
		_float	Ultimate = {};
	};

	struct PlayerLVDesc {
		_uint	LV = {};
		_float	MaxHP = {};
		_float	Attack = {};
		_float	Defend = {};
	};

	typedef struct tagMapDataFilePacket {
		string		TagDataFormat = {};			// MapData, EntityData
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
	};

	// 게이지 지연및 보간 상태
	typedef struct tagGaugeDelayDesc {
		_float fCurRatio = { 1.f };
		_float fTargetRatio = { 1.f };
		_float fDelayTimer = {};
		_float fDelayTime = { 0.3f };
		_bool isDelay = {};
	}GAUGE_DELAY_DESC;
}