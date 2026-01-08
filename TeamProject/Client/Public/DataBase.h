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
    PlayerDesc          GetPlayerDesc(const string& strName);
    MonsterCreationDesc GetMonsterDesc(const string& strName);

public:
    HRESULT LoadPlayerCreationTable(const string& csvPath);
    HRESULT LoadMonsterCreationTable(const string& csvPath);
    HRESULT LoadUICreationTable(const string& csvPath);

private:
    // 몬스터 세팅 테이블(CCT 정보, 각종 Status(HP, 공격력 등))
    unordered_map<string, MonsterCreationDesc>				m_MonsterCreationTables;
    // 플레이어 테이블
    unordered_map<string, PlayerDesc>				        m_PlayerTables;

public:
    virtual void Free() override;
};

NS_END