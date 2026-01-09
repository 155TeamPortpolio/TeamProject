#pragma once

NS_BEGIN(Client)

enum class UIEventType
{ 
	Enter_Monitor, Exit_Monitor
};

enum class GAUGE_OWNER { ROLE1, ROLE2, ROLE3, BOSS, END };
enum class GAUGE_TYPE { HP, HP_BACK, SPECIAL, ULTIMATE, EX_GAUGE, EX_SKILL, GROGGY, END };

/*게이지 정보를 정의하는 구조체. owner : 소유자, type : 게이지 타입, fillAmount : 현재 비율 값 ( 0 ~ 1 사이)*/
typedef struct tagGaugeDesc {
	GAUGE_OWNER	owner;
	GAUGE_TYPE	type;
	_float		fFillAmount;
}GAUGE_DESC;

NS_END