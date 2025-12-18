#pragma once
#include "Base.h"

NS_BEGIN(Client)
class CMapLoader final : public CBase
{
public:
	enum class MAPOBJ_TYPE {PLACED, FLOOR, TRIGGER, END};

private:
	CMapLoader();
	virtual ~CMapLoader() = default;

public:
	HRESULT	Initialize(const string TagLevel, const string MapDataPath);
	
private:
	void			Place_PlacedObjectFromLoadData(MapData_Object* pData);
	MAPOBJ_TYPE		Check_LayerTag(const string& TagLayer);

private:
	_int	m_iVersion = { 1 };
	string	m_TagLevel = {};
	vector<string>	m_TagLayers;

public:
	static CMapLoader* Create(const string& TagLevel, const string& MapDataPath);
	virtual void Free() override;
};

NS_END