#pragma once

NS_BEGIN(Client)

enum class UIEventType
{ 
	Enter_Monitor, Exit_Monitor
};

enum class UI_STATUS_OWNER { ROLE1, ROLE2, ROLE3, BOSS, END };
enum class UI_STATUS_TYPE { HP, HP_BACK, SPECIAL, ULTIMATE, EX_GAUGE, EX_SKILL, GROGGY, END };

typedef struct tagUIStatusValue {
	_float			fCurValue = {};
	_float			fMaxValue = {};
}UI_STATUS_VALUE;

/* UI로 전달되는 상태 정보를 정의하는 구조체 : status type 마다 개별적으로 */
typedef struct tagUIStatusDesc {
	UI_STATUS_OWNER	eOwner = {};
	UI_STATUS_TYPE	eType = {};
	_float			fCurValue = {};
	_float			fMaxValue = {};
}UI_STATUS_DESC;

/* UI로 전달되는 상태 정보를 정의하는 구조체 : status 일괄 */
typedef struct tagUIStatusBulkDesc {
	UI_STATUS_OWNER eOwner = {};
	vector<pair<UI_STATUS_TYPE, UI_STATUS_VALUE>> statusValues;
}UI_STATUS_BULK_DESC;

/* UI로 전달되는 상태 정보를 정의하는 구조체 : player의 status 일괄 */
typedef struct tagUIPlayerStatusDesc {
	UI_STATUS_OWNER eOwner = {};
	UI_STATUS_VALUE hp = {};
	UI_STATUS_VALUE special = {};
	UI_STATUS_VALUE ultimate = {};
}UI_PLAYER_STATUS_DESC;

/* UI로 전달되는 캐릭터 순서를 정의하는 구조체 */
typedef struct tagUICharacterOrder {
	_uint				iCount = {};
	array<CHARACTER, 3>	characters = {};
}UI_CHARACTER_ORDER;

NS_END