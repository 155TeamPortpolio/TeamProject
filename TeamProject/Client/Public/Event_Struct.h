#pragma once

NS_BEGIN(Client)

enum class UIEventType
{ 
	Enter_Monitor, Exit_Monitor
};

enum class UI_STATUS_OWNER { ROLE1, ROLE2, ROLE3, BOSS, END };
enum class UI_STATUS_TYPE { HP, HP_BACK, SPECIAL, ULTIMATE, EX_GAUGE, EX_SKILL, GROGGY, END };

/* UI로 전달되는 상태 정보를 정의하는 구조체 */
typedef struct tagUIStatusDesc {
	UI_STATUS_OWNER	eOwner;
	UI_STATUS_TYPE	eType;
	_float			fCurValue;
	_float			fMaxValue;
}UI_STATUS_DESC;

NS_END