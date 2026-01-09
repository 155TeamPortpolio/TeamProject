#pragma once

NS_BEGIN(Client)

enum class UIEventType
{ 
	Enter_Monitor, Exit_Monitor
};

enum class GAUGE_OWNER { ROLE1, ROLE2, ROLE3, BOSS, END };
enum class GAUGE_TYPE { HP, SPECIAL, ULTIMATE, EX_GAUGE, EX_SKILL, END };

typedef struct tagGaugeState {
	_float fMax;
	_float fCurrent;
}GAUGE_STATE;

typedef struct tagGaugeDesc {
	GAUGE_OWNER	owner;
	GAUGE_TYPE	type;
	GAUGE_STATE state;
}GAUGE_DESC;

NS_END