#pragma once
#include "Base.h"
NS_BEGIN(Client)
class CRoom :
    public CBase
{
protected:
    CRoom(const ROOM_DESC& desc);
    ~CRoom() DEFAULT;

public:
    virtual const string& Key() const { return m_RoomKey; };
    virtual bool IsPersistent() const { return false; }

    virtual void Enter() PURE;
    virtual void Exit() PURE;
    virtual void Update() PURE;
    virtual void OnResumeFromOverlay() PURE;

private:
    string m_RoomKey = {};
    _bool m_isPersistent = { false };

public:
    virtual void Free() override;
};

NS_END