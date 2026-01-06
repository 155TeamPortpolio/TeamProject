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



}