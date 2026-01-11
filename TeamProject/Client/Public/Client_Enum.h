#ifndef Client_Enum_h__
#define Client_Enum_h__

namespace Client {
	
	enum class CHARACTER { JaneDoe, Corin, END };

	/* UI */
	enum class UI_STATUS_OWNER { ROLE1, ROLE2, ROLE3, BOSS, END };
	enum class UI_STATUS_TYPE { HP, HP_BACK, SPECIAL, ULTIMATE, EX_GAUGE, EX_SKILL, GROGGY, END };
}

#endif