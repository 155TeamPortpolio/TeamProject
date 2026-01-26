#pragma once
#include "Base.h"
#include "MapData_Defines.h"

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
    NpcIDDesc               GetNpcIDData(const wstring& strName);
    NpcDialogueDesc         GetNpcDialogueDesc(pair<string, _uint> dialogueID);
    ChoiceDesc              GetNpcChoiceDesc(const string& strName);
    const vector<MapData_Path_Packet>*  GetMapDataPacket(const string& tagArea);
    const MapData_Path_Packet*          GetBattleFieldDataPacket(const string& tagArea);
    const vector<MONSTER_SPAWN_DESC>*   GetMonsterSpawnData(const string& tagArea);

public:
    HRESULT LoadPlayerCreationTable(const string& csvPath);
    HRESULT LoadPlayerLVTable(const string& csvPath);
    HRESULT LoadMonsterCreationTable(const string& csvPath);
    HRESULT LoadMonsterSpawnData(const string& csvPath);
    HRESULT LoadUICreationTable(const string& csvPath);
    HRESULT LoadMapData(const string& MapDataFolderPath);
    HRESULT LoadNpcIDData(const string& csvPath);
    HRESULT LoadNpcDialogueData(const string& csvPath);
    HRESULT LoadNpcChoiceData(const string& csvPath);

public:
    const CASHED_OBJ_DATA* Get_CashedData(const string& AreaTag);
    void Update_CashedData(const string& AreaTag, const CASHED_OBJ_DATA& Data);
    void Clear_CashedData();

private:
    vector<string_view> SplitFileName(string_view s, _char delim);

//enum 변환함수
private:
    DayPhase        StringToDayPhase(const string& str);
    DialogueType    StringToDialogueType(const string& str);
    DialogueResult  StringToDialogueResult(const string& str);
    Speaker         StringToSpeaker(const string& str);
    wstring         StringToWString(const string& str);
private:
    // 몬스터 세팅 테이블(CCT 정보, 각종 Status(HP, 공격력 등))
    unordered_map<string, MonsterCreationDesc>				m_MonsterCreationTables;
    // 플레이어 테이블
    unordered_map<string, PlayerDesc>				        m_PlayerTables;
    unordered_map<_uint, PlayerLVDesc>				        m_PlayerLVTables;
    // 지형별로 묶어놓은 맵 데이터 경로
    unordered_map<string, vector<MapData_Path_Packet>>	    m_MapAreaData;
    //NpcData
    unordered_map<wstring, NpcIDDesc>                       m_NpcIDTables;
    map<pair<string, _uint>, NpcDialogueDesc>		        m_DialogueTables;
    unordered_map<string, ChoiceDesc>				        m_DialgoueChoiceTables;
    // BattleField Data
    unordered_map<string, MapData_Path_Packet>	            m_BattleFieldData;
    // tagArea >>> vector<MONSTER_SPAWN_DESC>
    unordered_map<string, vector<MONSTER_SPAWN_DESC>>       m_MonsterSpawnDesc;

    //맵 데이터 <-> 런타임 데이터 연결용 (현재 작동중인 레벨에서만)
    unordered_map<string, CASHED_OBJ_DATA>                  m_CashedData;

public:
    virtual void Free() override;
};

NS_END