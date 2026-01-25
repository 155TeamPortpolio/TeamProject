#pragma once
#include "IUI_Service.h"
NS_BEGIN(Engine)
using UIobjects = vector<class CUI_Object*>;

class CUI_Manager :
	public IUI_Service
{
private:
	CUI_Manager();
	virtual ~CUI_Manager();
public:
	virtual void Pre_EngineUpdate(_float dt) override;
	virtual void Priority_Update(_float dt) override;
	virtual void Update(_float dt) override;
	virtual void Late_Update(_float dt) override;
	virtual void Post_EngineUpdate(_float dt) override;

public:
	virtual void Clear(const string& LevelTag) override;
	virtual HRESULT Sync_To_Level() override;
public:
	HRESULT Add_UIObject(class CUI_Object* object, const string& level);
	virtual void Remove_UIObject(class CUI_Object* object);
	const vector<class CUI_Object*>& Get_LevelUI(const string& leveTag);
	virtual class CUI_Object* Request_UIObject(const UI_HANDLE& handle) override;
	virtual class CUI_Object* Acquire(const CLONE_DESC& desc, INIT_DESC* pArg) override;

private:
	void Add_Object_Recursive(const string& LevelTag, class CUI_Object* object);
	void Sort_UI();
	void CleanUp();
	void Prune_Queues_ByLevel(const string& levelTag);
private:
	class CGameInstance* m_pGameInstance = { nullptr };
	unordered_map<string, UIobjects> m_UIObjects;
	UIobjects m_SortedUIObjects;
	string m_nowLevelKey = {};
	class CUI_Pool* m_pUIPool = { nullptr };

	vector<CUI_Object*> DeleteUIs;
	vector<CUI_Object*> m_ReleaseUIs;
	unordered_set<_uint> DeleteUI_IDs;
	unordered_set<_uint> m_ReleaseUI_IDs;
public:
	static CUI_Manager* Create();
	virtual void Free() override;
};
NS_END
