#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(MapTool)

class CMapToolCore final : public CBase
{
	DECLARE_SINGLETON(CMapToolCore)
private:
	CMapToolCore();
	virtual ~CMapToolCore() = default;

public:
	_int*			Get_Version_Ptr()					{ return &m_iVersion; }
	_int			Get_Version()						{ return m_iVersion; }
	void			Set_Version(_int iVersion)			{ m_iVersion = iVersion; }
	string			Get_TagLayer(MAPOBJ_TYPE eObjType)	{ return m_TagLayers[ENUM(eObjType)]; }
	vector<string>* Get_TagLayers()						{ return &m_TagLayers; }
	_bool			IsAllDebugRender()					{ return m_isAllDebugRender; }

#ifdef _DEBUG
	void			Set_AllObjectDebugRender(_bool is);
#endif // _DEBUG

public:
	vector<LOADED_OBJECT>	Load_MapData();
	void					Clear_Layer(MAPOBJ_TYPE eObjType);


private:
	MAPOBJ_TYPE		Check_LayerTag(const string& TagLayer);
	void			Place_PlacedObjectFromLoadData(MapData_Object* pData);



private:
	CGameInstance*		m_pGameInstance = { nullptr };

	vector<string>	m_TagLayers;
	_int			m_iVersion = { 1 };
	_bool			m_isAllDebugRender = { true };

public:
	static CMapToolCore* Create();
	virtual void Free() override;
};

NS_END