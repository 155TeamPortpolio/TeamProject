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
	//Npc
	if (FAILED(LoadNpcDialogueData("../../Resources/Data/Npc/NPC_Dialogue.csv")))
		return E_FAIL;
	if (FAILED(LoadNpcChoiceData("../../Resources/Data/Npc/NPC_Choice.csv")))
		return E_FAIL;
	if (FAILED(LoadNpcIDData("../../Resources/Data/Npc/Npc_ID.csv")))
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
		"ProtoTag", "DisplayName",
		"CCT_fHeight", "CCT_fRadius", "MaxHP"
	);

	string	ProtoTag{}, DisplayName{};
	_float	CCT_fHeight{}, CCT_fRadius{}, MaxHP{};

	while (in.read_row(
		ProtoTag, DisplayName,
		CCT_fHeight, CCT_fRadius, MaxHP
	))
	{
		if (ProtoTag.empty())
			continue;

		MonsterCreationDesc desc = {};
		desc.ProtoTag = ProtoTag;
		desc.DisplayName = DisplayName;
		desc.CCT_fHeight = CCT_fHeight;
		desc.CCT_fRadius = CCT_fRadius;
		desc.iMaxHP = MaxHP;

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

			if (false == (tokens[0] == "MapData" || tokens[0] == "EntityData"))
				continue;

			_int iVersion = {};
			// version을 string -> int로 변환
			auto [ptr, ec] = std::from_chars(tokens[3].data(), tokens[3].data() + tokens[3].size(), iVersion);
			if (ec != std::errc{})   // 숫자 파싱 실패
				continue;

			// 현재 클라이언트 버전보다 높으면 패스
			//if (iVersion > g_iMapDataVersion)
			//	continue;

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
		7,
		io::trim_chars<' ', '\t'>,
		io::double_quote_escape<',', '"'>
	>in(csvPath);

	in.read_header(
		io::ignore_extra_column | io::ignore_missing_column,
		"ChoiceID", "Text", "Result", "NextID", "NextSequence", "ValueType", "Value"
	);
	string			ChoiceID, NextID, ValueType;
	string			Text;
	_int			NextSequence;
	string			Result;
	string			Value;

	while (in.read_row(ChoiceID, Text, Result, NextID, NextSequence, ValueType, Value))
	{
		if (ChoiceID.empty()) continue;

		ChoiceDesc desc = {};
		desc.ChoiceID = ChoiceID;
		desc.Text = Helper::ConvertToWideString(Text);
		desc.Result = StringToDialogueResult(Result);
		desc.Next_DialogueID = NextID;
		desc.Next_SequeceID = NextSequence;
		desc.ValueType = ValueType;
		if (ValueType == "int" || ValueType == "Int") desc.Value = stoi(Value);
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
