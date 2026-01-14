#pragma once
#include "Base.h"

NS_BEGIN(Client)

class CDataBase :
    public CBase
{
    DECLARE_SINGLETON(CDataBase)

private:
    CDataBase();
    virtual ~CDataBase() = default;

public:
    HRESULT CreateTable();

public:
    PlayerDesc              GetPlayerDesc(const string& strName);
    PlayerLVDesc            GetLevelDesc(_uint lv);
    MonsterCreationDesc     GetMonsterDesc(const string& strName);
    const vector<MapData_Path_Packet>* GetMapDataPacket(const string& tagArea);

public:
    HRESULT LoadPlayerCreationTable(const string& csvPath);
    HRESULT LoadPlayerLVTable(const string& csvPath);
    HRESULT LoadMonsterCreationTable(const string& csvPath);
    HRESULT LoadUICreationTable(const string& csvPath);
    HRESULT LoadMapData(const string& MapDataFolderPath);

private:
    vector<string_view> SplitFileName(string_view s, _char delim);

private:
    // 몬스터 세팅 테이블(CCT 정보, 각종 Status(HP, 공격력 등))
    unordered_map<string, MonsterCreationDesc>				m_MonsterCreationTables;
    // 플레이어 테이블
    unordered_map<string, PlayerDesc>				        m_PlayerTables;
    unordered_map<_uint, PlayerLVDesc>				        m_PlayerLVTables;
    // 지형별로 묶어놓은 맵 데이터 경로
    unordered_map<string, vector<MapData_Path_Packet>>	    m_MapAreaData;


public:
    virtual void Free() override;
};

NS_END