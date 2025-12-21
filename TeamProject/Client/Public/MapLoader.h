#pragma once
#include "Base.h"
#include "MapData_Defines.h"


NS_BEGIN(Client)
class CMapLoader final : public CBase
{
public:
	enum class MAPOBJ_TYPE {PLACED, FLOOR, TRIGGER, END};

	using ObjFields			= vector<FIELD_DATA>;								// 한 데이터 묶음(ObjID, 변수명, 값)
	using ObjFieldMap		= unordered_map<int32_t, ObjFields>;				// ObjID별로 매핑된 데이터 묶음
	using SlotFormatData	= unordered_map<std::string, ObjFieldMap>;	// slotFormat별로 매핑된 데이터 묶음

private:
	CMapLoader();
	virtual ~CMapLoader() = default;

public:
	HRESULT	Initialize(const string& TagLevel, class CMapDataCloud* pMapDataCloud, const string& TagArea);
	
private:
	void			Place_PlacedObjectFromLoadData(MapData_Object* pData);
	MAPOBJ_TYPE		Check_LayerTag(const string& TagLayer);
	HRESULT			LoadBaseData(const MapData_Path_Packet* pPacket);
	HRESULT			LoadSlotData(const MapData_Path_Packet* pPacket);
	HRESULT			CacheSlotDataFile(const string& SlotDataFilePath);
	

private:
	string	m_TagLevel = {};
	vector<string>	m_TagLayers;

	MapData_Header	m_MapBaseData = {};
	SlotFormatData	m_SlotFormatData = {};

	_bool			m_hasColliderData = {};


public:
	static CMapLoader* Create(const string& TagLevel, class CMapDataCloud* pMapDataCloud, const string& TagArea);
	virtual void Free() override;
};

NS_END