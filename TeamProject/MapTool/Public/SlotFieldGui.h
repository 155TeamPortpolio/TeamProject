#pragma once
#include "BasePanel.h"
#include "MapTool_Context.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(MapTool)


class CSlotFieldGui final : public CBasePanel
{
public:
	struct RECORD {
		string TagName = {};
		unordered_map<string, SlotValue> values;
	};

private:
	CSlotFieldGui(GUI_CONTEXT* pContext);
	virtual ~CSlotFieldGui() = default;

public:
	HRESULT			Initialize();
	virtual void	Update_Panel(_float dt) override;
	virtual void	Render_GUI() override;

public:
	void			Set_isOpen(_bool is) { m_isOpen = is; }
	_bool			IsOpen() { return m_isOpen; }

private:
	void			CheckCoolTime(_float dt);
	SlotValue		MakeDefaultValue(SLOT_DATA_TYPE eType);
	string			MakeNewID();
	void			InjectDefaultToAllRecords(vector<RECORD>& records, const FIELD_DATA_DEFINE& def);
	_bool			ToBool(_bool is) { return is ? true : false; }
	_bool			FromBool(_bool is) { return (_bool)is; }
	const char*		FieldDataTypeName(SLOT_DATA_TYPE eType);
	FIELD_DATA_DEFINE MakeFromPrefab(const tagFieldDataDef& prefab);
	void			ApplyDraftToB();
	SlotValue&		EnsureValue(_int iObjIndex, const FIELD_DATA_DEFINE& Tab);

	/* Data */
	void					LoadBaseMapData();
	void					to_json(MTjson& j, const SlotValue& value);
	void					from_json(const MTjson& j, SlotValue& v);
	void					SaveSlotData();


private: 
	/* DrawGui */
	void			DrawDefaultEditor(FIELD_DATA_DEFINE& def);
	void			DrawValueEditor(const FIELD_DATA_DEFINE& tab, SlotValue& v);
	void			DrawLeftPanel();
	void			DrawRightPanel();

private:
	/* Data */
	vector<LOADED_OBJECT>		m_LoadedObjects;		// 로드 된 오브젝트 목록
	vector<FIELD_DATA_DEFINE>	m_FieldPrefabDefs;		// 미리 저장된 Prefab
	vector<FIELD_DATA_DEFINE>	m_DraftTabs;			// A칸 편집용 Draft(행)
	vector<FIELD_DATA_DEFINE>	m_AppliedTabs;			// Apply 후 확정된 탭

	vector<unordered_map<string, SlotValue>>	m_ObjUserValue;

	/* GUI Status */
	_int		m_iSelectedPrefabIndex = 0;
	_uint		m_iIdCounter = {};
	_bool		m_isRequestApply = { false };
	_bool		m_isRequestSave = { false };
	string		m_SavePath = {};
	
	string		m_TagSlotFormat = {};


private:
	CGameInstance*			m_pGameInstance = { nullptr };
	class CMapToolCore*		m_pMapToolCore = { nullptr };
	MAPTOOL_CONTEXT*		m_pMapToolContext = { nullptr };

	_bool					m_isOpen = { false };

	_float2					m_vShowSaveFinish = {};
	_bool					m_isShowSaveFinish = { false };

public:
	static CSlotFieldGui* Create(GUI_CONTEXT* pContext);
	virtual void Free() override;
};

NS_END