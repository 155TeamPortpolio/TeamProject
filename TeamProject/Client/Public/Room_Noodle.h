#pragma once
#include "Room.h"

NS_BEGIN(Client)

class CRoom_Noodle :
    public CRoom
{
private:
    CRoom_Noodle(const ROOM_DESC& desc);
    ~CRoom_Noodle() DEFAULT;

public:
    virtual void Enter();
    virtual void Exit();
    virtual void Update();
    virtual void OnResumeFromOverlay();

public:
    static CRoom_Noodle* Create(const ROOM_DESC& desc);
    virtual void Free() override;
};

NS_END
