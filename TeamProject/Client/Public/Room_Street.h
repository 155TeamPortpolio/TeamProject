#pragma once
#include "Room.h"
NS_BEGIN(Client)
class CRoom_Street :
    public CRoom
{
private:
    CRoom_Street(const ROOM_DESC& desc);
    ~CRoom_Street() DEFAULT;

public:
    virtual void Enter() ;
    virtual void Exit() ;
    virtual void Update() ;
    virtual void OnResumeFromOverlay() ;

private:
    class CMapLoader* m_pLoader = { nullptr };

public:
    static CRoom_Street* Create(const ROOM_DESC& desc);
    virtual void Free();
};

NS_END