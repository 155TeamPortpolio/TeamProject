#pragma once
#include "Base.h"
NS_BEGIN(Client)
class CFieldSystem :
    public CBase
{
  DECLARE_SINGLETON(CFieldSystem)

  struct DayTimer {
	  DayPhase DayTime = { DayPhase::EarlyMorning };
	  DayPhase PreDayTime = { DayPhase::LateNight };
	  _float currentHour = { 0.f };
	  _float timeScale = {0.2f};

	  _bool IsTransition = false;
	  _float TransitionTime = 0.f;
	  _float TransitionDuration = 2.0f;

	  FOG_DESC StartFog{}, TargetFog{};
	  CLOUD_DESC StartCloud{}, TargetCloud{};

	  void Update_Timer(_float dt);
	  void Update_Transition(_float dt);
	  void Set_DayPhase(DayPhase ePhase);
	  void Notify_DayPhaseEvent();
  };

private:
	CFieldSystem();
	virtual ~CFieldSystem() DEFAULT;

public:
	void Update();

public:
	void	SetActive(_bool is);
	_bool	IsActive() const { return m_isActive; }
	
public:
	void	SetFieldPlayer(class CFieldPlayer* pFieldPlayer);
	CFieldPlayer*					GetFieldPlayer();

	void	SetInteractHandle(OBJECT_HANDLE InteractHandle, OBJECT_HANDLE InteractPartnerHandle);
	void	ResetInteractHandle();
	OBJECT_HANDLE					GetInteractHandle() const;
	OBJECT_HANDLE					GetInteractPartnerHandle() const;

public:
	DayPhase Get_DayPhase() const { return m_DayTime.DayTime; }
	void Set_DayPahse(DayPhase ePhase) { m_DayTime.Set_DayPhase(ePhase); }

public:
	_bool RegisterRoom(class CRoom* pRoom);
	_bool RequestEnter(const string& roomKey, _bool overlay = true);
	_bool RequestExitTop();

public:
	void PreLoadRoom();

private:
	_bool	m_isActive = { false }; 
	class CFieldPlayer* m_pFieldPlayer = { nullptr };
	class CRoomDirector* m_pRoomDirector = { nullptr };

private:
	DayTimer m_DayTime = {};
	OBJECT_HANDLE m_InteractHandle;
	OBJECT_HANDLE m_InteractPartnerHandle;

public:
	virtual void Free() override;
};

inline CFieldSystem* FieldSystem() { return CFieldSystem::GetInstance(); }
NS_END