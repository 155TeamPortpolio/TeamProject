#pragma once
#include "Room.h"

NS_BEGIN(Client)

class CRoom_Party :
    public CRoom
{
private:
    CRoom_Party(const ROOM_DESC& desc);
    ~CRoom_Party() DEFAULT;

public:
    virtual void Enter();
    virtual void Exit();
    virtual void Update();
    virtual void OnResumeFromOverlay();

public:
    static CRoom_Party* Create(const ROOM_DESC& desc);
    virtual void Free() override;
};

NS_END