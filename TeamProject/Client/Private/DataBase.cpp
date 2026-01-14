#include "pch.h"
#include "DataBase.h"
#include "GameInstance.h"
#include "Helper_Func.h"

#include "MapLoader_Helper.h"
#include <charconv>

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
	// 맵
	if (FAILED(LoadMapData("../../Resources/Data/Map")))
		return E_FAIL;

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

const vector<MapData_Path_Packet>* CDataBase::GetMapDataPacket(const string& tagArea)
{
	auto iter = m_MapAreaData.find(tagArea);
	if (iter == m_MapAreaData.end())
		return nullptr;

	return &iter->second;
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

HRESULT CDataBase::LoadMapData(const string& MapDataFolderPath)
{
	Helper::EnsureDirectoryExist(MapDataFolderPath);

	for (const auto& entry : filesystem::recursive_directory_iterator(MapDataFolderPath))
	{
		if (entry.is_regular_file() && entry.path().extension() == ".json")
		{
			filesystem::path FilePath = entry.path();
			// ../MapData.MainCity.Base.1.json
			// ../MapData.MainCity.SlotA.1.json
			// ../MapData.MainCity.SlotB.1.json
			// ../MapData.Ocean.Base.1.json
			// ../MapData.Ocean.SlotA.1.json

			//if (entry.path().filename().string().find("MapData.") != string::npos)
			//	continue;

			const string stem = FilePath.stem().string();
			auto tokens = SplitFileName(stem, '.');

			if (tokens.size() < 4)
				continue;

			if (tokens[0] != "MapData")
				continue;

			_int iVersion = {};
			// version을 string -> int로 변환
			auto [ptr, ec] = std::from_chars(tokens[3].data(), tokens[3].data() + tokens[3].size(), iVersion);
			if (ec != std::errc{})   // 숫자 파싱 실패
				continue;

			// 현재 클라이언트 버전보다 높으면 패스
			if (iVersion > g_iMapDataVersion)
				continue;

			MapData_Path_Packet packet = {};
			packet.TagDataFileKey = FilePath.filename().string();
			packet.TagDataFilePath = FilePath.string();
			packet.TagArea = tokens[1];
			packet.TagSlotFormat = tokens[2];
			packet.iVersion = iVersion;

			auto& vecPacket = m_MapAreaData[packet.TagArea];

			auto it = std::find_if(vecPacket.begin(), vecPacket.end(),
				[&](const MapData_Path_Packet& p) { return p.TagSlotFormat == packet.TagSlotFormat; });

			// 버전이 높다면 높은 버전으로 교체
			if (it == vecPacket.end())
				vecPacket.push_back(packet);
			else if (packet.iVersion > it->iVersion)
				*it = packet;
		}
	}

	return S_OK;
}

vector<string_view> CDataBase::SplitFileName(string_view s, _char delim)
{
	vector<string_view> out;
	size_t pos = 0;
	while (true)
	{
		size_t next = s.find(delim, pos);
		if (next == string_view::npos) { out.emplace_back(s.substr(pos)); break; }
		out.emplace_back(s.substr(pos, next - pos));
		pos = next + 1;
	}
	return out;
}

void CDataBase::Free()
{
	__super::Free();
}
