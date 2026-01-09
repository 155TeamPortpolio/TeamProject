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
	if(FAILED(LoadPlayerCreationTable("../../Resources/Data/PlayerTable/PlayerTableCSV.csv"))) 
		return E_FAIL;
	if (FAILED(LoadMonsterCreationTable("../../Resources/Data/MonsterTable/MonsterTable.csv"))) 
		return E_FAIL;
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
		6,
		io::trim_chars<' ', '\t'>,
		io::double_quote_escape<',', '"'>
	>in(csvPath);

	/*
	헤더 이름으로 매핑(컬럼 순서 바뀌어도 무관)
	파일에 다른 컬럼이 더 있거나 누락된 컬럼이 있어도 무시함
	*/
	in.read_header(
		io::ignore_extra_column | io::ignore_missing_column,
		"Name", "MaxHp",
		"Attack", "ExType", "Ultimate","SkillE"
	);

	string	Name{}, ExType{};
	_float	Attack{}, MaxHp{}, Ultimate{}, SkillE{};

	while (in.read_row(Name,
		MaxHp, Attack, ExType, Ultimate, SkillE 
	))
	{
		if (Name.empty())
			continue;

		PlayerDesc desc = {};
		desc.strPlayerName = Name;
		desc.MaxHp = MaxHp;
		desc.Attack = Attack;
		desc.ExType = ExType;
		desc.Ultimate = Ultimate;
		desc.SpecialAttack = SkillE;

		auto [iter, inserted] = m_PlayerTables.emplace(desc.strPlayerName, move(desc));
		if (false == inserted) {
			wstring ErrorMsg = L"Duplicate PlayerKey in CSV : " + Helper::ConvertToWideString(Name);
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
