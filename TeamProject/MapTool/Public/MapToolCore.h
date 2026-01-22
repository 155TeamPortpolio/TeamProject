#pragma once
#include "Base.h"
#include "MapTool_Context.h"

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

	MAPTOOL_CONTEXT*	Get_Context() { return &m_tMapToolContext; }
	void				Set_AllObjectDebugRender(_bool is);

public:
	LOADED_DATA			Load_MapData();
	void				Load_WithEntityData();
	void				Clear_Layer(MAPOBJ_TYPE eObjType);


private:
	MAPOBJ_TYPE		Check_LayerTag(const string& TagLayer);
	void			Place_PlacedObjectFromLoadData(MapData_Object* pData);
	void			Place_TriggerObjectFromLoadData(MapData_Object* pData);
	void			Place_EntityObjectFromLoadData(ENTITY* pData);


private:
	CGameInstance*		m_pGameInstance = { nullptr };

	//vector<string>		m_TagLayers;
	//_int				m_iVersion = { 1 };
	//_bool				m_isAllDebugRender = { true };
	//string				m_TagLoadArea = {};

	MAPTOOL_CONTEXT		m_tMapToolContext = {};

public:
	static CMapToolCore* Create();
	virtual void Free() override;
};

NS_END