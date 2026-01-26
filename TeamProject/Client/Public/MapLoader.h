#pragma once
#include "Base.h"
#include "MapData_Defines.h"
#include "MapLoader_Helper.h"

NS_BEGIN(Client)
class CMapLoader final : public CBase
{
public:
	using ObjFields = vector<FIELD_DATA>;					// 한 데이터 묶음(ObjID, 변수명, 값)
	using ObjFieldMap = unordered_map<int, ObjFields>;		// ObjID별로 매핑된 데이터 묶음
	using SlotFormatData = unordered_map<string, ObjFieldMap>;	// slotFormat별로 매핑된 데이터 묶음

	struct LoadingQueue {
		MAPOBJ_TYPE Type;
		void* pData;
	};

private:
	CMapLoader();
	virtual ~CMapLoader() = default;

public:
	HRESULT Initialize(const string& TagLevel, const string& TagArea, _uint SplitLoadCount);
	void	Update_Load();

	template<typename T>
	optional<T> Get_EntitySlotDataValue(const string& TagFormat, _uint iEntityIndex, const string& TagName);

private:
	HRESULT			Load_BaseData(const string& TagArea, _bool* CheckMapBase, _bool* CheckEntityBase);
	void			Set_LoadingQueue();
	void			PlaceObjects_Once();
	_bool			PlaceObjects_Split();
	void			Update_Database();
	
private:
	void			Place_PlacedObjectFromLoadData(MapData_Object* pData);
	void			Place_TriggerObjectFromLoadData(MapData_Object* pData);
	void			Place_EntityFromLoadData(ENTITY_INIT* pData);
	MAPOBJ_TYPE		Check_LayerTag(const string& TagLayer);
	HRESULT			LoadMapBaseData(const MapData_Path_Packet* pPacket);
	HRESULT			LoadEntityBaseData(const MapData_Path_Packet* pPacket);
	HRESULT			LoadBattleData(const MapData_Path_Packet* pPacket);
	HRESULT			CacheSlotDataFile(const string& DataFormat, const string& SlotDataFilePath);

	_bool			isThereFormat(const string& TagSlotFormat);

private:
	string	m_TagLevel = {};
	string	m_TagArea = {};
	vector<string>	m_TagLayers;

	MapData_Header	m_MapBaseData = {};
	Entity_Header	m_EntityBaseData = {};
	
	//SlotFormatData	m_MapSlotFormatData = {};
	/* Format -> ObjID -> Data
	unordered_map<Format유형(effect, physics...), unordered_map<오브젝트 ID, vector<(ObjID, 변수명, 값)>>>*/
	unordered_map<string, unordered_map<_int, vector<FIELD_DATA>>>	m_MapSlotFormatData;
	unordered_map<string, unordered_map<_int, vector<FIELD_DATA>>>	m_EntitySlotFormatData;
	_bool m_hasColliderData = {};

	/*--------------------------------------------------------------------*/
	vector<CASHED_OBJECT>	m_MapObjectHandle;
	vector<CASHED_OBJECT>	m_TriggerObjectHandle;
	vector<CASHED_OBJECT>	m_EntityObjectHandle;
	queue<LoadingQueue>		m_LoadingQueue;

	_bool			m_bHasMapBase{}, m_bHasEntityBase{};
	_bool			m_bLoaded = { false };
	_uint			m_LoadDataCount{};
	_uint			m_iSplitLoadCount = { 0 };
	
public:
	static CMapLoader* Create(const string& TagLevel, const string& TagArea, _uint iSplitPlaceCount = 0);
	virtual void Free() override;
};

NS_END

template<typename T>
inline optional<T> CMapLoader::Get_EntitySlotDataValue(const string& TagFormat, _uint iEntityIndex, const string& TagName)
{
	auto Format = m_EntitySlotFormatData.find(TagFormat);
	if (Format == m_EntitySlotFormatData.end())
		return T();

	auto FileData = Format->second.find(iEntityIndex);
	if (FileData == Format->second.end())
		return T();

	for (auto& FieldData : FileData->second) {

		if (FieldData.TagName == TagName) {
			return GetSlotValue<T>(FieldData.defaultvalue);;
		}
	}

	return T();
}
