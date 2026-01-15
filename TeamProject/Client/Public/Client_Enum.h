#ifndef Client_Enum_h__
#define Client_Enum_h__

namespace Client {
	
	enum class CHARACTER { JaneDoe, Corin, END };

	/* UI */
	// ---------- Battle HUD : Stat ----------
	enum class UI_STATUS_OWNER { ROLE1, ROLE2, ROLE3, BOSS, END };
	enum class UI_STATUS_TYPE { HP, HP_BACK, SPECIAL, ULTIMATE, EX_GAUGE, EX_SKILL, GROGGY, END };
	// ---------- Battle HUD : Action ----------
	enum class UI_ACTION_TYPE { ALL, PRIMARY, EVADE, EVADEPERFECT, SPECIAL, SWITCH, ULTIMATE, END };
	enum UI_ACTION_STATE { DISABLE, ENABLE, AVAILABLE, EXECUTING };
	enum class UI_ACTION_PRIMARY_MODE { ATTACK, INTERACT };

	enum class HIT_TYPE { ONCE, INTERVAL, COUNT };
	enum class DAMAGE_TYPE {NORMAL, POWER, AIRBONE};
}

#endif