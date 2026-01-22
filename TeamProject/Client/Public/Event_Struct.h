#pragma once

NS_BEGIN(Client)

#pragma region BattleHUD_Stat
// ---------- Battle HUD : Stat ----------
typedef struct tagUIStatusValue {
	_float			fCurValue = {};
	_float			fMaxValue = {};
}UI_STATUS_VALUE;

/* UI로 전달되는 상태 정보를 정의하는 구조체 : player의 status 일괄 */
typedef struct tagUIPlayerStatusDesc {
	UI_STATUS_OWNER eOwner = {};
	CHARACTER		eCharacter = {};
	UI_STATUS_VALUE hp = {};
	UI_STATUS_VALUE special = {};
	UI_STATUS_VALUE ultimate = {};
	_float specialThreshold = {};
	// EX 관련 값 추가 필요
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

#pragma region TargetLockOn
typedef struct tagTargetLockDesc {
	_bool	bLock = false;
	OBJECT_HANDLE	tHandle{};
}TARGET_LOCK_DESC;

#pragma endregion

#pragma region StageChange
typedef struct tagStageChangedDesc {
	class CStage* pStage = {};

}STAGE_CHANGED_DESC;

#pragma endregion

#pragma region Interactable & Dialogue
// ---------- UI : Interactable ----------
/* 인터랙션 가능 범위 진입/이탈 시 UI에 대상 이름과 상호작용 가능 여부를 전달 */
typedef struct tagUIInteractableDesc{
	wstring strName = {};
	_bool isInteractable = {};
}UI_INTERACTABLE_DESC;

// ---------- UI : Dialogue ----------
/* 인터랙션 키 입력 시 실행할 대화(Dialogue)와 시퀀스를 UI에 요청 */
typedef struct tagUIDialogueRequestDesc {
	string strDialogueID = {};
	_uint iSequenceID = {};
}UI_DIALOGUE_REQUEST_DESC;

//-----------Npc : 실행함수 ---------------
typedef struct tagNpcInteractDesc {
	wstring strName = {};
	_uint iCurSequenceID = {};
	_uint iNextSequenceID = {};
	DialogueResult eResult = {};
}NPC_INTERACT_DESC;
#pragma endregion

NS_END