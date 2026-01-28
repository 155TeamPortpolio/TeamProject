#pragma once
#include "Base.h"

NS_BEGIN(Client)
class CRoomDirector :
    public CBase
{
private:
    CRoomDirector();
    ~CRoomDirector() DEFAULT;

public:
    HRESULT Initialize();
    void Update();

public:
    _bool RegisterRoom(class CRoom* pRoom);
    _bool RequestEnter(const string& roomKey, _bool overlay = true);
    _bool RequestExitTop();

    HRESULT ClearRooms();

public:
    const vector<string>& GetActiveRoomStack() const;
    
private:
    void DoEnter(const string& key, _bool overlay);

private:
    unordered_map<string, class CRoom*> m_Rooms;
    vector<string> m_ActiveStacks;

    string m_pendingEnterKey;
    _bool m_pendingOverlay = true;
public:
    static CRoomDirector* Create();
    virtual void Free();
};
NS_END

