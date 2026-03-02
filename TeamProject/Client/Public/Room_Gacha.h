#pragma once
#include "Room.h"

NS_BEGIN(Client)

class CRoom_Gacha :
    public CRoom
{
private:
    CRoom_Gacha(const ROOM_DESC& desc);
    ~CRoom_Gacha() DEFAULT;

public:
    virtual void Enter();
    virtual void Exit();
    virtual void Update();
    virtual void OnResumeFromOverlay();

public:
    static CRoom_Gacha* Create(const ROOM_DESC& desc);
    virtual void Free() override;
};

NS_END