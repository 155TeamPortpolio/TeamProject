#pragma once
#include "Room.h"

NS_BEGIN(Client)

class CRoom_Lottery :
    public CRoom
{
private:
    CRoom_Lottery(const ROOM_DESC& desc);
    ~CRoom_Lottery() DEFAULT;

public:
    virtual void Enter();
    virtual void Exit();
    virtual void Update();
    virtual void OnResumeFromOverlay();

public:
    static CRoom_Lottery* Create(const ROOM_DESC& desc);
    virtual void Free() override;
};

NS_END