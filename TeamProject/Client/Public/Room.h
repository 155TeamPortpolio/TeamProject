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
    virtual bool IsPersistent() const { return m_isPersistent; }

    virtual void Enter() PURE;
    virtual void Exit() PURE;
    virtual void Update() PURE;
    virtual void OnResumeFromOverlay() PURE;
    virtual void OnResumeToOverlay() {};

    virtual void ReserveMap(const string& LevelTag, const string& AreaTag);
    virtual void DeActiveMap();
    virtual void ActiveMap();
protected:
    string m_RoomKey = {};
    _bool m_isPersistent = { false };
    vector<OBJECT_HANDLE> m_MapObjects;
public:
    virtual void Free() override;
};

NS_END