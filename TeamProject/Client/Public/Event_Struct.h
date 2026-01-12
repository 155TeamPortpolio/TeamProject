#pragma once

NS_BEGIN(Client)

#pragma region BattleHUD_Stat

// ---------- Battle HUD : Stat ----------

typedef struct tagUIStatusValue {
	_float			fCurValue = {};
	_float			fMaxValue = {};
}UI_STATUS_VALUE;

/* UI로 전달되는 상태 정보를 정의하는 구조체 : status type 마다 개별적으로 */
typedef struct tagUIStatusDesc {
	UI_STATUS_OWNER	eOwner = {};
	UI_STATUS_TYPE	eType = {};
	UI_STATUS_VALUE	value = {};
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
#pragma endregion

#pragma region BattleHUD_Action

// ---------- Battle HUD : Action ----------

/* UI로 전달되는 액션 상태 정보를 정의하는 구조체 */
typedef struct tagUIActionDesc {
	UI_ACTION_TYPE		eType = {};			// 액션 종류 (all, primary, evade, special, switch, ultimate)
	UI_ACTION_STATE		eState = {};		// 액션 현재 상태 (dissable, enable, available, execute)
	_float				fFillAmount = {};	// 진행도 (0 ~ 1)
}UI_ACTION_DESC;

/* UI로 전달되는 primary 액션 모드 정보를 정의하는 구조체*/
typedef struct tagUIActionPrimaryDesc {
	UI_ACTION_PRIMARY_MODE eMode = {};		// 현재 primary의 액션 모드 (attack, interact)
}UI_ACTION_PRIMARY_DESC;
#pragma endregion

NS_END