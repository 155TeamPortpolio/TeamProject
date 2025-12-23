#pragma once
#include "Base.h"
#include "MapData_Defines.h"

NS_BEGIN(Client)

class CMapDataCloud final : public CBase
{
private:
	CMapDataCloud();
	virtual ~CMapDataCloud() = default;

public:
	HRESULT Initialize(const string& MapDataFolderPath);

public:
	const vector<MapData_Path_Packet>*	Get_MapDataPacket(const string& tagArea);

private:
	void	Rake_MapData(const string& MapDataFolderPath);
	vector<string_view> SplitFileName(string_view s, _char delim);
private:
	/* unordered_mpa<지형, unordered_map<파일이름.json, 파일경로>>*/
	unordered_map<string, vector<MapData_Path_Packet>>	m_MapAreaDatas;

public:
	static CMapDataCloud* Create(const string& MapDataFolderPath);
	virtual void Free() override;
};


NS_END