#ifndef Client_Enum_h__
#define Client_Enum_h__

namespace Client {
	enum class GLOBAL_ID : _uint { FreeCam, OrbitCam, SeqCam, ShadowCam, Player, Cloud };
	enum class CHARACTER { JaneDoe, Corin, END };

	/* UI */
	// ---------- Battle HUD : Stat ----------
	enum class UI_STATUS_OWNER { ROLE1, ROLE2, ROLE3, BOSS, END };
	enum class UI_STATUS_TYPE { HP, HP_BACK, SPECIAL, ULTIMATE, EX_GAUGE, EX_SKILL, GROGGY, END };
	// ---------- Battle HUD : Action ----------
	enum class UI_ACTION_TYPE { ALL, PRIMARY, EVADE, EVADEPERFECT, SPECIAL, SWITCH, ULTIMATE, END };
	enum UI_ACTION_STATE { DISABLE, ENABLE, AVAILABLE, EXECUTING };
	enum class UI_ACTION_PRIMARY_MODE { ATTACK, INTERACT, END };

	enum class HIT_TYPE { ONCE, INTERVAL, COUNT };
	enum class DAMAGE_TYPE {NORMAL, HARD, AIRBORNE, ULTIMATE};

	//------------------- Field -------------------
	enum class DayPhase {EarlyMorning,Morning,Afternoon,LateNight,Any};
	enum class Speaker { Player, Npc, System };
	enum class DialogueType {Normal, Choice, Quest};
	enum class DialogueResult {Fail, Running, Success, None};


	//------------------- Battle -------------------
	enum class BATTLE_VFX_TYPE { EVADE,PARRY, SWITCH,ULTIMATE, END };
	enum class BATTLE_OBJ_TYPE { PLAYER, MONSTER, ENVOBJECT, END };

}

#endif