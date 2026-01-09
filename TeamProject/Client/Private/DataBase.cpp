#include "pch.h"
#include "DataBase.h"
#include "GameInstance.h"
#include "Helper_Func.h"

IMPLEMENT_SINGLETON(CDataBase)

CDataBase::CDataBase()
{
}

HRESULT CDataBase::CreateTable()
{
	//플레이어
	if(FAILED(LoadPlayerCreationTable("../../Resources/Data/PlayerTable/PlayerTableCSV.csv"))) 
		return E_FAIL;
	if (FAILED(LoadPlayerLVTable("../../Resources/Data/PlayerTable/PlayerLVCSV.csv")))
		return E_FAIL;
	//몬스터
	if (FAILED(LoadMonsterCreationTable("../../Resources/Data/MonsterTable/MonsterTable.csv"))) 
		return E_FAIL;
	//UI
	//LoadUICreationTable();
	return S_OK;
}

PlayerDesc CDataBase::GetPlayerDesc(const string& strName)
{
	auto iter = m_PlayerTables.find(strName);
	if (iter == m_PlayerTables.end())
		return PlayerDesc{};

	return iter->second;
}

PlayerLVDesc CDataBase::GetLevelDesc(_uint lv)
{
	auto iter = m_PlayerLVTables.find(lv);
	if (iter == m_PlayerLVTables.end())
		return PlayerLVDesc{};

	return iter->second;
}

MonsterCreationDesc CDataBase::GetMonsterDesc(const string& strName)
{
	auto iter = m_MonsterCreationTables.find(strName);
	if (iter == m_MonsterCreationTables.end())
		return MonsterCreationDesc{};

	return iter->second;
}

HRESULT CDataBase::LoadPlayerCreationTable(const string& csvPath)
{
	/*
		column_count = read_header에 넣는 컬럼 수(헤더 개수)
		trim_chars = 앞 뒤 공백 제거
		double_quote_escape = "..." 안의 쉼표 및 따옴표 처리 */
	io::CSVReader<
		5,
		io::trim_chars<' ', '\t'>,
		io::double_quote_escape<',', '"'>
	>in(csvPath);

	/*
	헤더 이름으로 매핑(컬럼 순서 바뀌어도 무관)
	파일에 다른 컬럼이 더 있거나 누락된 컬럼이 있어도 무시함
	*/
	in.read_header(
		io::ignore_extra_column | io::ignore_missing_column,
		"Name", "LV",
		"SpecialAttack", "ExType", "Ultimate"
	);

	string	Name{}, ExType{};
	_uint	LV{};
	_float	SpecialAttack{}, Ultimate{};

	while (in.read_row(Name, LV, SpecialAttack, ExType, Ultimate))
	{
		if (Name.empty()) continue;

		PlayerDesc desc = {};
		desc.strPlayerName = Name;
		desc.LV = LV;
		desc.SpecialAttack = SpecialAttack;
		desc.ExType = ExType;
		desc.Ultimate = Ultimate;

		auto [iter, inserted] = m_PlayerTables.emplace(desc.strPlayerName, move(desc));
		if (false == inserted) 
		{
			wstring ErrorMsg = L"Duplicate PlayerKey in CSV : " + Helper::ConvertToWideString(Name);
			MessageBox(NULL, ErrorMsg.c_str(), L"System Message", MB_OK);
		}
	}

	return S_OK;
}

HRESULT CDataBase::LoadPlayerLVTable(const string& csvPath)
{
	io::CSVReader<
		4,
		io::trim_chars<' ', '\t'>,
		io::double_quote_escape<',', '"'>
	>in(csvPath);

	in.read_header(
		io::ignore_extra_column | io::ignore_missing_column,
		"LV",
		"HP", "Attack", "Defend"
	);

	_uint	LV{};
	_float	HP{}, Attack{}, Defend{};

	while (in.read_row(LV, HP, Attack, Defend))
	{
		if (LV <= 0) continue;

		PlayerLVDesc desc = {};
		desc.LV = LV;
		desc.MaxHP = HP;
		desc.Attack = Attack;
		desc.Defend = Defend;

		auto [iter, inserted] = m_PlayerLVTables.emplace(desc.LV, move(desc));
		if (false == inserted) 
		{
			wstring ErrorMsg = L"Duplicate PlayerLVKey in CSV : " + to_wstring(LV);
			MessageBox(NULL, ErrorMsg.c_str(), L"System Message", MB_OK);
		}
	}

	return S_OK;
}

HRESULT CDataBase::LoadMonsterCreationTable(const string& csvPath)
{
	/*  
	column_count = read_header에 넣는 컬럼 수(헤더 개수)
	trim_chars = 앞 뒤 공백 제거
	double_quote_escape = "..." 안의 쉼표 및 따옴표 처리 */
	io::CSVReader<
		4,
		io::trim_chars<' ', '\t'>,
		io::double_quote_escape<',', '"'>
	>in(csvPath);

	/*
	헤더 이름으로 매핑(컬럼 순서 바뀌어도 무관)
	파일에 다른 컬럼이 더 있거나 누락된 컬럼이 있어도 무시함
	*/
	in.read_header(
		io::ignore_extra_column | io::ignore_missing_column,
		"ProtoTag", "DisplayName",
		"CCT_fHeight", "CCT_fRadius"/*, "CCT_vPos_X", "CCT_vPos_Y", "CCT_vPos_Z"*/
	);

	string	ProtoTag{}, DisplayName{};
	_float	CCT_fHeight{}, CCT_fRadius{};
	//_float	CCT_vPos_X{}, CCT_vPos_Y{}, CCT_vPos_Z{};

	while (in.read_row(
		ProtoTag, DisplayName,
		CCT_fHeight, CCT_fRadius/*, CCT_vPos_X, CCT_vPos_Y, CCT_vPos_Z*/
	))
	{
		if (ProtoTag.empty())
			continue;

		MonsterCreationDesc desc = {};
		desc.ProtoTag = ProtoTag;
		desc.DisplayName = DisplayName;
		desc.CCT_fHeight = CCT_fHeight;
		desc.CCT_fRadius = CCT_fRadius;
		//desc.CCT_vPos = { CCT_vPos_X, CCT_vPos_Y, CCT_vPos_Z };

		auto [iter, inserted] = m_MonsterCreationTables.emplace(desc.ProtoTag, move(desc));
		if (false == inserted) {
			wstring ErrorMsg = L"Duplicate MonsterKey in CSV : " + Helper::ConvertToWideString(ProtoTag);
			MessageBox(NULL, ErrorMsg.c_str(), L"System Message", MB_OK);
		}
	}

	return S_OK;
}

HRESULT CDataBase::LoadUICreationTable(const string& csvPath)
{
	return S_OK;
}

void CDataBase::Free()
{
	__super::Free();
}
