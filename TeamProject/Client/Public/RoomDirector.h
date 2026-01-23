#pragma once
#include "Base.h"

NS_BEGIN(Client)
class CRoomDirector :
    public CBase
{
    enum class ROOM_STATE {Unloaded,Preloading,Loaded,Active,Exiting};
    struct RoomEntry
    {
        ROOM_DESC desc;
        ROOM_STATE state = ROOM_STATE::Unloaded;
        _uint preloadTaskId = 0;

        //unique_ptr<IRoomScript> script;
        vector<OBJECT_HANDLE> spawnedHandles;
    };

private:
    CRoomDirector();
    ~CRoomDirector() DEFAULT;

public:
    HRESULT Initialize();
    void Update();
public:
    bool RegisterRoom(const ROOM_DESC& desc);
    bool RequestEnter(const string& roomKey, _bool overlay = true);
    bool RequestExitTop();

public:
    const string& GetCityRoomKey() const;
    const vector<string>& GetActiveRoomStack() const;

private:
    unordered_map<string, ROOM_DESC> m_roomDescByKey; /*로드 정보*/
    unordered_map<string, RoomEntry> m_roomStageByKey; /*런타임 정보*/

public:
    static CRoomDirector* Create();
    virtual void Free();
};
NS_END

