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
		_float3 CCT_vPos = {};

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
}