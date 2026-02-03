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
	if (FAILED(LoadPlayerCreationTable("../../Resources/Data/PlayerTable/PlayerTableCSV.csv"))) 
		return E_FAIL;

	//몬스터
	if (FAILED(LoadMonsterCreationTable("../../Resources/Data/MonsterTable/MonsterTable.csv"))) 
		return E_FAIL;
	if (FAILED(LoadMonsterSpawnData("../../Resources/Data/MonsterTable/MonsterSpawn.csv")))
		return E_FAIL;

	//UI
	//LoadUICreationTable();

	// 맵
	if (FAILED(LoadMapData("../../Resources/Data/Map")))
		return E_FAIL;

	/*Field*/
	//Npc
	if (FAILED(LoadNpcDialogueData("../../Resources/Data/Npc/NPC_Dialogue.csv")))
		return E_FAIL;
	if (FAILED(LoadNpcChoiceData("../../Resources/Data/Npc/NPC_Choice.csv")))
		return E_FAIL;
	if (FAILED(LoadNpcIDData("../../Resources/Data/Npc/Npc_ID.csv")))
		return E_FAIL;

	//Shop
	if (FAILED(LoadRamenData("../../Resources/Data/Shop/Shop_Ramen.csv")))
		return E_FAIL;
	//Gacha
	if (FAILED(LoadWeaponData("../../Resources/Data/Gacha/WeaponID.csv")))
		return E_FAIL;
	if (FAILED(LoadTVData("../../Resources/Data/Gacha/TVImage.csv")))
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

MonsterCreationDesc CDataBase::GetMonsterDesc(const string& strName)
{
	for (auto& Table : m_MonsterCreationTables) {
		if (Table.second.ProtoTag == strName)
			return Table.second;
	}

	return MonsterCreationDesc{};
}

MonsterCreationDesc CDataBase::GetMonsterDesc(_int ColonyIndex, _int MonsterID)
{
	_int FindIndex = ColonyIndex * 1000 + MonsterID;

	auto iter = m_MonsterCreationTables.find(FindIndex);
	if(iter == m_MonsterCreationTables.end())
		return MonsterCreationDesc();

	return iter->second;
}

NpcIDDesc CDataBase::GetNpcIDData(const wstring& strName)
{
	auto iter = m_NpcIDTables.find(strName);
	if (iter == m_NpcIDTables.end())
		return NpcIDDesc{};

	return iter->second;
}

NpcDialogueDesc CDataBase::GetNpcDialogueDesc(pair<string, _uint> dialogueID)
{
	auto iter = m_DialogueTables.find(dialogueID);
	if (iter == m_DialogueTables.end())
		return NpcDialogueDesc{};

	return iter->second;
}

ChoiceDesc CDataBase::GetNpcChoiceDesc(const string& strName)
{
	auto iter = m_DialgoueChoiceTables.find(strName);
	if (iter == m_DialgoueChoiceTables.end())
		return ChoiceDesc{};

	return iter->second;
}

const vector<MapData_Path_Packet>* CDataBase::GetMapDataPacket(const string& tagArea)
{
	auto iter = m_MapAreaData.find(tagArea);
	if (iter == m_MapAreaData.end())
		return nullptr;

	return &iter->second;
}

const EncounterTable* CDataBase::GetMonsterSpawnData(const string& tagArea, _uint iStageType)
{
	auto itArea = m_BattleSpawnData.find(tagArea);
	if (itArea == m_BattleSpawnData.end()) return nullptr;

	auto itStage = itArea->second.find(iStageType);
	if (itStage == itArea->second.end()) return nullptr;

	return &itStage->second;
}

RAMEN_DESC CDataBase::GetRamenDesc(const string& strName)
{
	auto iter = m_RamenTables.find(strName);
	if (iter == m_RamenTables.end())
		return RAMEN_DESC();

	return iter->second;
}

vector<const RAMEN_DESC*> CDataBase::GetRamenTable()
{
	vector<const RAMEN_DESC*> result;
	for (const auto& ramen : m_RamenTables)
		result.push_back(&ramen.second);

	sort(result.begin(), result.end(),
		[](const RAMEN_DESC* a, const RAMEN_DESC* b)
		{
			return a->iOrder < b->iOrder;
		});

	return result;
}

WEAPON_DESC CDataBase::GetWeaponDesc(_int WeaponID)
{
	auto iter = m_WeaponTables.find(WeaponID);
	if (iter == m_WeaponTables.end())
		return WEAPON_DESC{};

	return iter->second;
}

TV_DESC CDataBase::GetTVDesc(const string& strName)
{
	auto iter = m_TVTables.find(strName);
	if (iter == m_TVTables.end())
		return TV_DESC{};

	return iter->second;
}

vector<WEAPON_DESC> CDataBase::GetGachaResults(_int WeaponNum)
{
	vector<WEAPON_DESC> Results(10);
	if (WeaponNum <= 0 || WeaponNum > 10 || m_WeaponTables.empty())
		return Results;

	vector<_int> weaponKeys;
	for (const auto& pair : m_WeaponTables)
		weaponKeys.push_back(pair.first);

	if (WeaponNum > weaponKeys.size())
		WeaponNum = weaponKeys.size();

	random_device rd;
	mt19937 gen(rd());

	shuffle(weaponKeys.begin(), weaponKeys.end(), gen);

	vector<_int> positions(10);
	iota(positions.begin(), positions.end(), 0);
	shuffle(positions.begin(), positions.end(), gen);

	for (_int i = 0; i < WeaponNum; ++i)
	{
		Results[positions[i]] = m_WeaponTables[weaponKeys[i]];
	}

	return Results;
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
		"Name", "HP", "Attack", "Defend",
		"SpecialAttack", "Ultimate"
	);

	string	Name{};
	_float	HP{}, Attack{}, Defend{}, SpecialAttack{}, Ultimate{};

	while (in.read_row(Name, HP, Attack, Defend, SpecialAttack, Ultimate))
	{
		if (Name.empty()) continue;

		PlayerDesc desc = {};
		desc.strPlayerName = Name;
		desc.MaxHP = HP;
		desc.Attack = Attack;
		desc.Defend = Defend;
		desc.SpecialAttack = SpecialAttack;
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

HRESULT CDataBase::LoadMonsterCreationTable(const string& csvPath)
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
		"ProtoTag", "DisplayName",
		"CCT_fHeight", "CCT_fRadius", "MaxHP",
		"MonsterID"
	);

	string	ProtoTag{}, DisplayName{};
	_float	CCT_fHeight{}, CCT_fRadius{}, MaxHP{};
	_uint	MonsterID{};

	while (in.read_row(
		ProtoTag, DisplayName,
		CCT_fHeight, CCT_fRadius, MaxHP,
		MonsterID
	))
	{
		if (ProtoTag.empty())
			continue;

		MonsterCreationDesc desc = {};
		desc.ProtoTag = ProtoTag;
		desc.DisplayName = DisplayName;
		desc.MonsterID = MonsterID;
 		desc.CCT_fHeight = CCT_fHeight;
		desc.CCT_fRadius = CCT_fRadius;
		desc.iMaxHP = MaxHP;

		auto [iter, inserted] = m_MonsterCreationTables.emplace(desc.MonsterID, move(desc));
		if (false == inserted) {
			wstring ErrorMsg = L"Duplicate MonsterKey in CSV : " + Helper::ConvertToWideString(ProtoTag);
			MessageBox(NULL, ErrorMsg.c_str(), L"System Message", MB_OK);
		}
	}

	return S_OK;
}

HRESULT CDataBase::LoadMonsterSpawnData(const string& csvPath)
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
		"tagArea", "StageType",
		"Encounter",
		"Colony", "MonsterID", "Count"
	);

	string	tagArea{}, StageType{};
	_int	Encounter{};
	_int	Colony{}, MonsterID{}, Count{};

	while (in.read_row(
		tagArea, StageType,
		Encounter,
		Colony, MonsterID, Count
	))
	{
		if (tagArea.empty())
			continue;

		if (StageType.empty())
			continue;
		
		_uint iStageType{};
		if (StageType == "Normal")		iStageType = 0;
		else if (StageType == "Elite")	iStageType = 1;
		else if (StageType == "Boss")	iStageType = 2;

		SPAWN_MONSTER_DESC desc{};
		desc.Colony = Colony;
		desc.MonsterID = MonsterID;
		desc.Count = Count;

		m_BattleSpawnData[tagArea][iStageType][Encounter].push_back(desc);
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

			const string stem = FilePath.stem().string();
			auto tokens = SplitFileName(stem, '.');

			if (find(begin(MAP_DATA_TAGS), end(MAP_DATA_TAGS), tokens[0]) != end(MAP_DATA_TAGS)) {
				
				if (tokens[0] == "BattleData") {
					MapData_Path_Packet packet = {};
					packet.TagDataFormat = tokens[0];
					packet.TagDataFileKey = FilePath.filename().string();
					packet.TagDataFilePath = FilePath.string();
					packet.TagArea = tokens[1];

					m_MapAreaData[packet.TagArea].push_back(packet);
					continue;
				}

				_int iVersion = {};
				
				auto [ptr, ec] = std::from_chars(tokens[3].data(), tokens[3].data() + tokens[3].size(), iVersion);
				if (ec != std::errc{})
					continue;

				MapData_Path_Packet packet = {};
				packet.TagDataFormat = tokens[0];
				packet.TagDataFileKey = FilePath.filename().string();
				packet.TagDataFilePath = FilePath.string();
				packet.TagArea = tokens[1];
				packet.TagSlotFormat = tokens[2];
				packet.iVersion = iVersion;

				auto& vecPacket = m_MapAreaData[packet.TagArea];
				auto it = std::find_if(vecPacket.begin(), vecPacket.end(),
					[&](const MapData_Path_Packet& p) {
						if (p.TagDataFormat == packet.TagDataFormat)
							return p.TagSlotFormat == packet.TagSlotFormat;
						return false;
					});

				// 버전이 높다면 높은 버전으로 교체
				if (it == vecPacket.end())
					vecPacket.push_back(packet);
				else if (packet.iVersion > it->iVersion)
					*it = packet;

				continue;
			}

		}
	}

	return S_OK;
}

HRESULT CDataBase::LoadNpcIDData(const string& csvPath)
{
	io::CSVReader<
		4,
		io::trim_chars<' ', '\t'>,
		io::double_quote_escape<',', '"'>
	>in(csvPath);

	in.read_header(
		io::ignore_extra_column | io::ignore_missing_column,
		"Name", "StartID", "DialogueNum", "SequenceNum"
	);
	string			Name, StartID;
	_uint			DialogueNum, SequenceNum;

	while (in.read_row(Name, StartID, DialogueNum, SequenceNum))
	{
		if (Name.empty()) continue;

		NpcIDDesc desc = {};
		desc.Name = Helper::ConvertToWideString(Name);
		desc.StartDialogueID = StartID;
		desc.DialogueNum = DialogueNum;
		desc.SequenceNum = SequenceNum;

		auto [iter, inserted] = m_NpcIDTables.emplace(desc.Name, move(desc));
		if (false == inserted)
		{
			wstring ErrorMsg = L"Duplicate MonsterKey in CSV : " + desc.Name;
			MessageBox(NULL, ErrorMsg.c_str(), L"System Message", MB_OK);
		}
	}

	return S_OK;
}

HRESULT CDataBase::LoadNpcDialogueData(const string& csvPath)
{
	io::CSVReader<
		14,
		io::trim_chars<' ', '\t'>,
		io::double_quote_escape<',', '"'>
	>in(csvPath);

	in.read_header(
		io::ignore_extra_column | io::ignore_missing_column,
		"ID", "Name", "Sequence", "Speaker", "Time", "Type", "Repeat", "Text", "Result", "ChoiceNum",
		"Choice1", "Choice2", "Choice3", "NextSequence"
	);
	string			DialogueID, Choice_ID1, Choice_ID2, Choice_ID3;
	string			Name, Text;
	_uint			SequenceID, ChoiceNum, NextSequenceID;
	_uint			Repeat;
	string			Speaker;
	string			DayPhase;
	string			DialogueType;
	string			Result;

	while (in.read_row(DialogueID, Name, SequenceID, Speaker, DayPhase, DialogueType, Repeat, Text, Result,
		ChoiceNum, Choice_ID1, Choice_ID2, Choice_ID3, NextSequenceID))
	{
		if (DialogueID.empty()) continue;

		NpcDialogueDesc desc = {};
		desc.DialogueID = DialogueID;
		desc.Name = Helper::ConvertToWideString(Name);
		desc.Speaker = StringToSpeaker(Speaker);
		desc.SequenceID = SequenceID;
		desc.DayPhase = StringToDayPhase(DayPhase);
		desc.DialogueType = StringToDialogueType(DialogueType);
		desc.Repeat = static_cast<_bool>(Repeat);
		desc.Text = StringToWString(Text);
		desc.Result = StringToDialogueResult(Result);
		desc.ChoiceNum = ChoiceNum;
		desc.Choice_ID1 = Choice_ID1;
		desc.Choice_ID2 = Choice_ID2;
		desc.Choice_ID3 = Choice_ID3;
		desc.NextSequenceID = NextSequenceID;

		auto [iter, inserted] = m_DialogueTables.emplace(make_pair(desc.DialogueID, desc.SequenceID), move(desc));
		if (false == inserted)
		{
			string ErrorMsg = "Duplicate DialogueTable in CSV : " + DialogueID;
			std::wstring wErrorMsg(ErrorMsg.begin(), ErrorMsg.end());
			MessageBox(NULL, wErrorMsg.c_str(), L"System Message", MB_OK);
		}
	}

	return S_OK;
}

HRESULT CDataBase::LoadNpcChoiceData(const string& csvPath)
{
	io::CSVReader<
		8,
		io::trim_chars<' ', '\t'>,
		io::double_quote_escape<',', '"'>
	>in(csvPath);

	in.read_header(
		io::ignore_extra_column | io::ignore_missing_column,
		"ChoiceID", "Text", "Result", "NextID", "NextSequence", "ValueType", "ValueName", "Value"
	);
	string			ChoiceID, NextID, ValueType, ValueName;
	string			Text;
	_int			NextSequence;
	string			Result;
	string			Value;

	while (in.read_row(ChoiceID, Text, Result, NextID, NextSequence, ValueType, ValueName, Value))
	{
		if (ChoiceID.empty()) continue;

		ChoiceDesc desc = {};
		desc.ChoiceID = ChoiceID;
		desc.Text = Helper::ConvertToWideString(Text);
		desc.Result = StringToDialogueResult(Result);
		desc.Next_DialogueID = NextID;
		desc.Next_SequeceID = NextSequence;
		desc.ValueType = ValueType;
		desc.ValueName = ValueName;
		if (ValueType == "bool") {
			if (Value == "true" || Value == "TRUE")
				desc.Value = true;
			else if (Value == "false" || Value == "FALSE")
				desc.Value = false;
		}
		else if (ValueType == "int" || ValueType == "Int") desc.Value = stoi(Value);
		else if (ValueType == "float" || ValueType == "Float") desc.Value = stof(Value);
		else if (ValueType == "string" || ValueType == "String")desc.Value = Value;

		auto [iter, inserted] = m_DialgoueChoiceTables.emplace(ChoiceID, move(desc));
		if (false == inserted)
		{
			string ErrorMsg = "Duplicate DialogueChoiceTable in CSV : " + ChoiceID;
			std::wstring wErrorMsg(ErrorMsg.begin(), ErrorMsg.end());
			MessageBox(NULL, wErrorMsg.c_str(), L"System Message", MB_OK);
		}
	}

	return S_OK;
}

HRESULT CDataBase::LoadRamenData(const string& csvPath)
{
	io::CSVReader<
		11,
		io::trim_chars<' ', '\t'>,
		io::double_quote_escape<',', '"'>
	>in(csvPath);
	
	in.read_header(
		io::ignore_extra_column | io::ignore_missing_column,
		"ID", "Name", "Price", "Order", "AttributeCount", "AttributeID1", "AttributeName1", "AttributeValue1", "AttributeID2", "AttributeName2", "AttributeValue2"
	);
	string			strID;
	string			strName;
	string			strPrice;
	string			strOrder;
	string			strAttributeCount;
	string			strAttributeID1, strAttributeName1, strAttributeValue1;
	string			strAttributeID2, strAttributeName2, strAttributeValue2;
	
	while (in.read_row(strID, strName, strPrice, strOrder, strAttributeCount, strAttributeID1, strAttributeName1, strAttributeValue1, strAttributeID2, strAttributeName2, strAttributeValue2))
	{
		if (strID.empty()) continue;
	
		RAMEN_DESC desc = {};
		desc.strID = strID;
		desc.strName = StringToWString(strName);
		desc.iPrice = strPrice.empty()? 0 : stoi(strPrice);
		desc.iOrder = strOrder.empty() ? 999 : stoi(strOrder);

		_int iAttributeCount = strAttributeCount.empty() ? 0 : stoi(strAttributeCount);  
		desc.attributes.reserve(iAttributeCount);

		if (iAttributeCount >= 1)
		{
			_int iAttributeValue1 = strAttributeValue1.empty() ? 0 : stoi(strAttributeValue1);
			desc.attributes.push_back(RAMEN_ATTRIBUTE{ strAttributeID1, Helper::ConvertToWideString(strAttributeName1), iAttributeValue1 });
		} 

		if (iAttributeCount >= 2)
		{
			_int iAttributeValue2 = strAttributeValue2.empty() ? 0 : stoi(strAttributeValue2);
			desc.attributes.push_back(RAMEN_ATTRIBUTE{ strAttributeID2, Helper::ConvertToWideString(strAttributeName2), iAttributeValue2 });
		} 
	
		auto [iter, inserted] = m_RamenTables.emplace(strID, move(desc));
		if (false == inserted)
		{
			string ErrorMsg = "Duplicate RamenTable in CSV : ";
			std::wstring wErrorMsg(ErrorMsg.begin(), ErrorMsg.end());
			MessageBox(NULL, wErrorMsg.c_str(), L"System Message", MB_OK);
		}
	}

	return S_OK;
}

HRESULT CDataBase::LoadWeaponData(const string& csvPath)
{
	io::CSVReader<
		5,
		io::trim_chars<' ', '\t'>,
		io::double_quote_escape<',', '"'>
	>in(csvPath);

	in.read_header(
		io::ignore_extra_column | io::ignore_missing_column,
		"ID", "Grade", "Model", "Mat", "Texture"
	);
	string			Grade, Model, Material, Texture;
	_int			ID;

	while (in.read_row(ID, Grade, Model, Material,Texture))
	{
		if (ID == -1) continue;

		WEAPON_DESC desc = {};
		desc.ID = ID;
		desc.Grade = StringToGachaGrade(Grade);
		desc.strModel = Model;
		desc.strMaterial = Material;
		desc.strTexture = Texture;

		auto [iter, inserted] = m_WeaponTables.emplace(desc.ID, move(desc));
		if (false == inserted)
		{
			wstring ErrorMsg = L"Duplicate WeaponKey in CSV : " + desc.ID;
			MessageBox(NULL, ErrorMsg.c_str(), L"System Message", MB_OK);
		}
	}

	return S_OK;
}

HRESULT CDataBase::LoadTVData(const string& csvPath)
{
	io::CSVReader<
		4,
		io::trim_chars<' ', '\t'>,
		io::double_quote_escape<',', '"'>
	>in(csvPath);

	in.read_header(
		io::ignore_extra_column | io::ignore_missing_column,
		"Name", "Col", "Row", "MaxFrame"
	);
	string			Name;
	_int			Col, Row, MaxFrame;

	while (in.read_row(Name, Col, Row, MaxFrame))
	{
		if (Name.empty()) continue;

		TV_DESC desc = {};
		desc.strName = Name;
		desc.Col = Col;
		desc.Row = Row;
		desc.MaxFrame = MaxFrame;

		auto [iter, inserted] = m_TVTables.emplace(desc.strName, move(desc));
		if (false == inserted)
		{
			wstring ErrorMsg = L"Duplicate TVkey in CSV : " + Helper::ConvertToWideString(desc.strName);
			MessageBox(NULL, ErrorMsg.c_str(), L"System Message", MB_OK);
		}
	}

	return S_OK;
}

const CASHED_OBJ_DATA* CDataBase::Get_CashedData(const string& AreaTag)
{
	auto iter = m_CashedData.find(AreaTag);
	if (iter == m_CashedData.end()) return nullptr;
	return &iter->second;
}

void CDataBase::Update_CashedData(const string& AreaTag, const CASHED_OBJ_DATA& Data)
{
	auto iter = m_CashedData.find(AreaTag);
	if (iter != m_CashedData.end())
		iter->second = Data;
	else
		m_CashedData.emplace(AreaTag, Data);
}

void CDataBase::Clear_CashedData()
{
	m_CashedData.clear();
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

DayPhase CDataBase::StringToDayPhase(const string& str)
{
	if (str == "EarlyMorning") return DayPhase::EarlyMorning;
	if (str == "Morning") return DayPhase::Morning;
	if (str == "Afternoon") return DayPhase::Afternoon;
	if (str == "LateNight") return DayPhase::LateNight;
	if (str == "Any") return DayPhase::Any;

	return DayPhase::Any;
}

DialogueType CDataBase::StringToDialogueType(const string& str)
{
	if (str == "Normal") return DialogueType::Normal;
	if (str == "Choice") return DialogueType::Choice;
	if (str == "Quest") return DialogueType::Quest;

	return DialogueType::Normal;
}

DialogueResult CDataBase::StringToDialogueResult(const string& str)
{
	if (str == "Running") return DialogueResult::Running;
	if (str == "Success") return DialogueResult::Success;
	if (str == "Fail") return DialogueResult::Fail;

	return DialogueResult::Running;
}

Speaker CDataBase::StringToSpeaker(const string& str)
{
	if (str == "System") return Speaker::System;
	if (str == "Npc") return Speaker::Npc;
	if (str == "Player") return Speaker::Player;

	return Speaker::Npc;
}

GachaGrade CDataBase::StringToGachaGrade(const string& str)
{
	if (str == "S") return GachaGrade::S;
	if (str == "A") return GachaGrade::A;
	if (str == "B") return GachaGrade::B;

	return GachaGrade::B;
}

wstring CDataBase::StringToWString(const string& str)
{
	if (str.empty()) return wstring();

	string processed = str;
	size_t pos = 0;
	while ((pos = processed.find("\\n", pos)) != string::npos) {
		processed.replace(pos, 2, "\n");
		pos += 1;
	}
	int size = MultiByteToWideChar(CP_UTF8, 0, processed.c_str(), -1, NULL, 0);
	wstring wstr(size - 1, 0);
	MultiByteToWideChar(CP_UTF8, 0, processed.c_str(), -1, &wstr[0], size);
	return wstr;
}

void CDataBase::Free()
{
	__super::Free();
}
