#pragma once
#include "Base.h"
NS_BEGIN(Client)
class CFieldSystem :
    public CBase
{
  DECLARE_SINGLETON(CFieldSystem)

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
private:
	_bool	m_isActive = { false }; 
	class CFieldPlayer* m_pFieldPlayer = { nullptr };

public:
	virtual void Free() override;
};

NS_END