#pragma once
#include "Base.h"
NS_BEGIN(Client)
class CFieldSystem :
    public CBase
{
  DECLARE_SINGLETON(CFieldSystem)

  struct DayTimer {
	  DayPhase m_eDayTime = { DayPhase::Morning };
	  _float currentHour = { 0.f };
	  _float timeScale = {0.2f};
	  void Update_Timer(_float dt) {
		  currentHour += dt * timeScale;

		  if (currentHour > 24.f)
			  currentHour -= 24.f;

		  if (currentHour >= 6.0f && currentHour < 12.0f)
			  m_eDayTime= DayPhase::Morning;
		  else if (currentHour >= 12.0f && currentHour < 18.0f)
			  m_eDayTime = DayPhase::Afternoon;
		  else if (currentHour >= 18.0f && currentHour < 24.0f)
			  m_eDayTime= DayPhase::LateNight;
		  else
			  m_eDayTime= DayPhase::EarlyMorning;
	  }
	  void Set_DayPhase(DayPhase ePhase) {
		  m_eDayTime = ePhase;
		  switch (ePhase)
		  {
		  case Client::DayPhase::EarlyMorning:
			  currentHour = 0.f;
			  break;
		  case Client::DayPhase::Morning:
			  currentHour = 6.0f;
			  break;
		  case Client::DayPhase::Afternoon:
			  currentHour = 12.0f;
			  break;
		  case Client::DayPhase::LateNight:
			  currentHour = 18.0f;
			  break;
		  default:
			  break;
		  }
	  }
  };

private:
	CFieldSystem();
	virtual ~CFieldSystem() DEFAULT;

public:
	void Update();

public:
	void	SetActive(_bool is) { m_isActive = is; }

public:
	void	SetFieldPlayer(class CFieldPlayer* pFieldPlayer);
	OBJECT_HANDLE					GetCurCharacterHandle() const;

	/*여기서 대화를 진행하고 / 미니맵을 진행*/
public:
	DayPhase Get_DayPhase() { return m_DayTime.m_eDayTime; }
	void Set_DayPahse(DayPhase ePhase) { m_DayTime.Set_DayPhase(ePhase); }

private:
	_bool	m_isActive = { false }; 
	class CFieldPlayer* m_pFieldPlayer = { nullptr };

	DayTimer m_DayTime = {};
public:
	virtual void Free() override;
};

NS_END