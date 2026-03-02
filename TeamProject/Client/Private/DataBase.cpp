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
	if (FAILED(LoadSpeechBubble("../../Resources/Data/Npc/SpeechBubble.csv")))
		return E_FAIL;
	//Shop
	if (FAILED(LoadRamenData("../../Resources/Data/Shop/Shop_Ramen.csv")))
		return E_FAIL;
	//Gacha
	if (FAILED(LoadGachaResultData("../../Resources/Data/Gacha/ResultID.csv")))
		return E_FAIL;
	if (FAILED(LoadGachaData("../../Resources/Data/Gacha/GachaRandom.csv")))
		return E_FAIL;
	if (FAILED(LoadTVData("../../Resources/Data/Gacha/TVImage.csv")))
		return E_FAIL;
	if (FAILED(LoadGachaChannelData("../../Resources/Data/Gacha/GachaChannel.csv")))
		return E_FAIL;

	// Party
	if (FAILED(LoadPartyData("../../Resources/Data/Party/Party.csv")))
		return E_FAIL;

	// Tutorial
	if (FAILED(LoadTutorialData("../../Resources/Data/Tutorial/Tutorial.csv")))
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

SpeechBubbleDesc CDataBase::GetNpcSpeechBubble(pair<string, _uint> speechBubbleID)
{
	if (Helper::Get_Random_Bool(0.6) == false)
		return SpeechBubbleDesc{};

	auto range = m_SpeechBubbleTables.equal_range(speechBubbleID);
	if (range.first == range.second)
		return SpeechBubbleDesc{};

	_uint count = (_uint)distance(range.first, range.second);
	auto& [shuffled, cursor] = m_ShuffledSpeechBubbleIndices[speechBubbleID];

	if (shuffled.empty() || cursor >= count)
	{
		shuffled.resize(count);
		iota(shuffled.begin(), shuffled.end(), 0);
		shuffle(shuffled.begin(), shuffled.end(), mt19937{ random_device{}() });
		cursor = 0;
	}

	auto iter = range.first;
	advance(iter, shuffled[cursor++]);
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

GACHA_RESULT_DESC CDataBase::GetGachaResultDesc(_int ID)
{
	auto iter = m_ResultTables.find(ID);
	if (iter == m_ResultTables.end())
		return GACHA_RESULT_DESC{};

	return iter->second;
}

vector<GACHA_RESULT_DESC> CDataBase::GetGachaGroup()
{
	array<_int, 10> IDs = m_GachaData[m_iCurrentGachaOrder++];

	vector<GACHA_RESULT_DESC> Results;
	for (auto& ID : IDs)
	{
		Results.push_back(GetGachaResultDesc(ID));
	}

	if (m_iCurrentGachaOrder >= m_iMaxGachaOrder)
		m_iCurrentGachaOrder = m_iMaxGachaOrder -1;

	return Results;
}

TV_DESC CDataBase::GetTVDesc(const string& strName)
{
	auto iter = m_TVTables.find(strName);
	if (iter == m_TVTables.end())
		return TV_DESC{};

	return iter->second;
}

const vector<GACHA_CHANNEL_DESC>& CDataBase::GeGachaChannels()
{
	return m_GachaChannels;
}

PARTY_DESC CDataBase::GetPartyData(CHARACTER eCharacter)
{
	auto iter = m_PartyTables.find(eCharacter);
	if (iter == m_PartyTables.end())
		return PARTY_DESC{};

	return iter->second;
}

vector<TUTORIAL_ACTION_DESC> CDataBase::GetTutorialActions(TUTORIAL_TYPE eType)
{
	auto iter = m_TutorialTables.find(eType);
	if (iter == m_TutorialTables.end())
		return {};

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
		if (StageType == "Start")		iStageType = ENUM(StageType::Start);
		else if (StageType == "Normal")	iStageType = ENUM(StageType::Normal);
		else if (StageType == "Elite")	iStageType = ENUM(StageType::Elite);
		else if (StageType == "Boss")	iStageType = ENUM(StageType::Boss);

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

HRESULT CDataBase::LoadGachaResultData(const string& csvPath)
{
	io::CSVReader<
		15,
		io::trim_chars<' ', '\t'>,
		io::double_quote_escape<',', '"'>
	>in(csvPath);

	in.read_header(
		io::ignore_extra_column | io::ignore_missing_column,
		"ID", "Type", "Grade", "Model", "Mat", "Texture", "Label", "RotX", "RotY", "RotZ", "RotW", "Meta", "Start", "Loop", "CamSequenceKey"
	);
	string			Type, Grade, Model, Material, Texture, Label, Meta, Start, Loop, CamSequenceKey;
	_int			ID;
	_float			RotX, RotY, RotZ, RotW;

	while (in.read_row(ID, Type, Grade, Model, Material,Texture, Label, RotX, RotY, RotZ, RotW, Meta, Start, Loop, CamSequenceKey))
	{
		if (ID == -1) continue;

		GACHA_RESULT_DESC desc = {};
		desc.ID = ID;
		desc.Type = StringToGachaType(Type);
		desc.Grade = StringToGachaGrade(Grade);
		desc.strModel = Model;
		desc.strMaterial = Material;
		desc.strTexture = Texture;
		desc.strLabel = Helper::ConvertToWideString(Label);
		desc.RotX = RotX;
		desc.RotY = RotY;
		desc.RotZ = RotZ;
		desc.RotW = RotW;
		desc.strMeta = Meta;
		desc.strStartAnim = Start;
		desc.strLoopAnim = Loop;
		desc.strCamSequenceKey = CamSequenceKey;

		auto [iter, inserted] = m_ResultTables.emplace(desc.ID, move(desc));
		if (false == inserted)
		{
			wstring ErrorMsg = L"Duplicate WeaponKey in CSV : " + desc.ID;
			MessageBox(NULL, ErrorMsg.c_str(), L"System Message", MB_OK);
		}
	}

	return S_OK;
}

HRESULT CDataBase::LoadGachaData(const string& csvPath)
{
	io::CSVReader<
		10,
		io::trim_chars<' ', '\t'>,
		io::double_quote_escape<',', '"'>
	>in(csvPath);

	in.read_header(
		io::ignore_extra_column | io::ignore_missing_column,
		"ID_0", "ID_1", "ID_2", "ID_3","ID_4","ID_5", "ID_6", "ID_7", "ID_8", "ID_9"
	);

	array<_int, 10> IDs;

	while (in.read_row(IDs[0], IDs[1], IDs[2], IDs[3], IDs[4], IDs[5], IDs[6], IDs[7], IDs[8], IDs[9]))
	{
		m_GachaData.push_back(IDs);
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

HRESULT CDataBase::LoadGachaChannelData(const string& csvPath)
{
	io::CSVReader<
		2,
		io::trim_chars<' ', '\t'>,
		io::double_quote_escape<',', '"'>
	>in(csvPath);

	in.read_header(
		io::ignore_extra_column | io::ignore_missing_column,
		"Label", "TextureKey"
	);

	string strLabel = {}, textureKey = {};

	while (in.read_row(strLabel, textureKey))
	{
		GACHA_CHANNEL_DESC desc = {};
		desc.strLabel = Helper::ConvertToWideString(strLabel);
		desc.strTextureKey = textureKey;

		m_GachaChannels.push_back(desc);
	}

	return S_OK;
}

HRESULT CDataBase::LoadPartyData(const string& csvPath)
{
	io::CSVReader<
		17,
		io::trim_chars<' ', '\t'>,
		io::double_quote_escape<',', '"'>
	>in(csvPath);

	in.read_header(
		io::ignore_extra_column | io::ignore_missing_column,
		"Character", "Name", "Level", "Attribute", "Specialty", "AttributeTexture", "SpecialtyTexture", "Model", "Material", "Meta", "Anim", "PosX", "PosY", "PosZ", "ColorR", "ColorG", "ColorB"
	);

	string strCharacter, strName, strAttribute, strSpecialty;
	string strAttributeTexture, strSpecialtyTexture;
	string strModelKey, strMaterialKey, strMetaKey, strAnimClipKey;
	_int iLevel = {};
	_float3 vPosition = {};
	_float3 vColor = {};

	while (in.read_row(strCharacter, strName, iLevel, strAttribute, strSpecialty, strAttributeTexture, strSpecialtyTexture, strModelKey, strMaterialKey, strMetaKey, strAnimClipKey, vPosition.x, vPosition.y, vPosition.z, vColor.x, vColor.y, vColor.z))
	{
		PARTY_DESC desc = {};
		desc.eCharacter = StringToCharacter(strCharacter);
		desc.strName = Helper::ConvertToWideString(strName);
		desc.iLevel = iLevel;
		desc.eAttribute = StringToAttribute(strAttribute);
		desc.eSpecialty = StringToSpecialty(strSpecialty);
		desc.strAttributeTexture = strAttributeTexture;
		desc.strSpecialtyTexture = strSpecialtyTexture;
		desc.strModelKey = strModelKey;
		desc.strMaterialKey = strMaterialKey;
		desc.strMetaKey = strMetaKey;
		desc.strAnimClipKey = strAnimClipKey;
		desc.vPosition = vPosition;
		desc.vColor = _float4(vColor.x / 255.f, vColor.y / 255.f, vColor.z / 255.f, 1.f);

		if(desc.eCharacter != CHARACTER::END)
			m_PartyTables.emplace(desc.eCharacter, desc);
	}

	return S_OK;
}

HRESULT CDataBase::LoadTutorialData(const string& csvPath)
{
	io::CSVReader<
		3,
		io::trim_chars<' ', '\t'>,
		io::double_quote_escape<',', '"'>
	>in(csvPath);

	in.read_header(
		io::ignore_extra_column | io::ignore_missing_column,
		"TutorialID", "ActionID", "Count"
	);

	string strTutorialID, strActionID;
	_uint iCount = {};

	while (in.read_row(strTutorialID, strActionID, iCount))
	{
		TUTORIAL_ACTION_DESC desc = {};
		desc.eAction = StringToTutorialAction(strActionID);
		desc.iCount = iCount;

		auto type = StringToTutorialType(strTutorialID);

		if (type != TUTORIAL_TYPE::END)
		{
			auto& actions = m_TutorialTables[type];

			auto iter = find_if(actions.begin(),actions.end(),[&](const TUTORIAL_ACTION_DESC& action)
				{
					return action.eAction == desc.eAction;
				});

			if (iter == actions.end())
				actions.push_back(desc);
		}
	}
	
	return S_OK;
}

HRESULT CDataBase::LoadSpeechBubble(const string& csvPath)
{
	io::CSVReader<
		3,
		io::trim_chars<' ', '\t'>,
		io::double_quote_escape<',', '"'>
	>in(csvPath);

	in.read_header(
		io::ignore_extra_column | io::ignore_missing_column,
		"Level", "Version", "Text"
	);
	string			Level, Text;
	_int			Version;

	while (in.read_row(Level, Version, Text))
	{
		if (Level.empty()) continue;

		SpeechBubbleDesc desc = {};
		desc.Level = Level;
		desc.Version = Version;
		desc.Text = StringToWString(Text);

		m_SpeechBubbleTables.emplace(make_pair(desc.Level, desc.Version), move(desc));
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

vector<CHARACTER> CDataBase::Get_EnableCharacters()
{
	vector<CHARACTER> results;

	for (int i = 0; i <= m_iCurrentGachaOrder; ++i) {
		results.push_back(static_cast<CHARACTER>(i));
	}
	return results;
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

GachaType CDataBase::StringToGachaType(const string& str)
{
	if (str == "Agent") return GachaType::Agent;
	if (str == "Engine") return GachaType::Engine;

	return GachaType::Engine;
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

CHARACTER CDataBase::StringToCharacter(const string& str)
{ 
	if (str == "Corin") return CHARACTER::Corin;
	if (str == "JaneDoe") return CHARACTER::JaneDoe;
	if (str == "Miyabi") return CHARACTER::Miyabi;

	return CHARACTER::END;
}

ATTRIBUTE CDataBase::StringToAttribute(const string& str)
{
	if (str == "Electric") return ATTRIBUTE::Electric;
	if (str == "Ether") return ATTRIBUTE::Ether;
	if (str == "Fire") return ATTRIBUTE::Fire;
	if (str == "Ice") return ATTRIBUTE::Ice;
	if (str == "Physical") return ATTRIBUTE::Physical;

	return ATTRIBUTE::END;
}

SPECIALTY CDataBase::StringToSpecialty(const string& str)
{
	if (str == "Attack") return SPECIALTY::Attack;
	if (str == "Anomaly") return SPECIALTY::Anomaly;
	if (str == "Defenses") return SPECIALTY::Defenses;
	if (str == "Rupture") return SPECIALTY::Rupture;
	if (str == "Stun") return SPECIALTY::Stun;
	if (str == "Support") return SPECIALTY::Support;

	return SPECIALTY::END;
}

TUTORIAL_TYPE CDataBase::StringToTutorialType(const string& str)
{
	if (str == "ExtremeEvade") return TUTORIAL_TYPE::EXTREME_EVADE;
	if (str == "ExtremeSupport") return TUTORIAL_TYPE::EXTREME_SUPPORT;
	if (str == "DecibelUltimate") return TUTORIAL_TYPE::DECIBEL_ULTIMATE;
	if (str == "GroggyCombo") return TUTORIAL_TYPE::GROGGY_COMBO;

	return TUTORIAL_TYPE::END;
}

TUTORIAL_ACTION CDataBase::StringToTutorialAction(const string& str)
{
	if (str == "PerfectDodge") return TUTORIAL_ACTION::DODGE;
	if (str == "PerfectDodgeCounter") return TUTORIAL_ACTION::DODGE_COUNTER;
	if (str == "PerfectAssist") return TUTORIAL_ACTION::ASSIST;
	if (str == "PerfectAssistCharge") return TUTORIAL_ACTION::ASSIST_CHARGE;
	if (str == "UltimateSkill") return TUTORIAL_ACTION::ULTIMATE;
	if (str == "ComboSkill") return TUTORIAL_ACTION::COMBO;

	return TUTORIAL_ACTION::END;
}

void CDataBase::Free()
{
	__super::Free();
}
