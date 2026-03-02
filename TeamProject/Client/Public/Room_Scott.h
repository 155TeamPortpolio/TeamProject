#pragma once
#include "Room.h"

NS_BEGIN(Client)

class CRoom_Scott :
    public CRoom
{
private:
    CRoom_Scott(const ROOM_DESC& desc);
    ~CRoom_Scott() DEFAULT;

public:
    virtual void Enter();
    virtual void Exit();
    virtual void Update();
    virtual void OnResumeFromOverlay();
    virtual void OnResumeToOverlay();

private:
    class CMapLoader* m_pLoader = { nullptr };

public:
    static CRoom_Scott* Create(const ROOM_DESC& desc);
    virtual void Free() override;
};

NS_END