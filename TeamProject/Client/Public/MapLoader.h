#pragma once
#include "Base.h"
#include "MapData_Defines.h"


NS_BEGIN(Client)
class CMapLoader final : public CBase
{
public:
	enum class MAPOBJ_TYPE { PLACED, TRIGGER, ENTITY, END };

	using ObjFields = vector<FIELD_DATA>;					// 한 데이터 묶음(ObjID, 변수명, 값)
	using ObjFieldMap = unordered_map<int, ObjFields>;		// ObjID별로 매핑된 데이터 묶음
	using SlotFormatData = unordered_map<string, ObjFieldMap>;	// slotFormat별로 매핑된 데이터 묶음

private:
	CMapLoader();
	virtual ~CMapLoader() = default;

public:
	HRESULT	Initialize(const string& TagLevel, const string& TagArea);

private:
	void			Place_PlacedObjectFromLoadData(MapData_Object* pData);
	void			Place_TriggerObjectFromLoadData(MapData_Object* pData);
	void			Place_EntityFromLoadData(ENTITY_INIT* pData);
	MAPOBJ_TYPE		Check_LayerTag(const string& TagLayer);
	HRESULT			LoadMapBaseData(const MapData_Path_Packet* pPacket);
	HRESULT			LoadEntityBaseData(const MapData_Path_Packet* pPacket);
	HRESULT			CacheSlotDataFile(const string& DataFormat, const string& SlotDataFilePath);

	_bool			isThereFormat(const string& TagSlotFormat);

private:
	string	m_TagLevel = {};
	vector<string>	m_TagLayers;

	MapData_Header	m_MapBaseData = {};
	Entity_Header	m_EntityBaseData = {};
	//SlotFormatData	m_MapSlotFormatData = {};
	/* Format -> ObjID -> Data
	unordered_map<Format유형(effect, physics...), unordered_map<오브젝트 ID, vector<(ObjID, 변수명, 값)>>>*/
	unordered_map<string, unordered_map<_int, vector<FIELD_DATA>>>	m_MapSlotFormatData;
	unordered_map<string, unordered_map<_int, vector<FIELD_DATA>>>	m_EntitySlotFormatData;

	_bool			m_hasColliderData = {};


public:
	static CMapLoader* Create(const string& TagLevel, const string& TagArea);
	virtual void Free() override;
};

NS_END