#include "pch.h"
#include "MapDataCloud.h"
#include "GameInstance.h"
#include "Helper_Func.h"
#include "MapData_Defines.h"

#include <charconv>

CMapDataCloud::CMapDataCloud()
{
}

HRESULT CMapDataCloud::Initialize(const string& MapDataFolderPath)
{
	Rake_MapData(MapDataFolderPath);

    return S_OK;
}

const vector<MapData_Path_Packet>* CMapDataCloud::Get_MapDataPacket(const string& tagArea)
{
	auto iter = m_MapAreaDatas.find(tagArea);
	if (iter == m_MapAreaDatas.end())
		return nullptr;
	
	return &iter->second;
}

void CMapDataCloud::Rake_MapData(const string& MapDataFolderPath)
{
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

			auto& vecPacket = m_MapAreaDatas[packet.TagArea];

			auto it = std::find_if(vecPacket.begin(), vecPacket.end(),
				[&](const MapData_Path_Packet& p) { return p.TagSlotFormat == packet.TagSlotFormat; });

			// 버전이 높다면 높은 버전으로 교체
			if (it == vecPacket.end())
				vecPacket.push_back(packet);
			else if (packet.iVersion > it->iVersion)
				*it = packet;
		}
	}
}

vector<string_view> CMapDataCloud::SplitFileName(string_view s, char delim)
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

CMapDataCloud* CMapDataCloud::Create(const string& MapDataFolderPath)
{
    CMapDataCloud* instance = new CMapDataCloud();

	if (FAILED(instance->Initialize(MapDataFolderPath))) {
        Safe_Release(instance);
        instance = nullptr;
        MSG_BOX("Failed to Create : CMapLoader");
    }

    return instance;
}

void CMapDataCloud::Free()
{
    __super::Free();
}
