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

private:
	void Add_Object_Recursive(const string& LevelTag, class CUI_Object* object);
	void Sort_UI();
private:
	class CGameInstance* m_pGameInstance = { nullptr };
	unordered_map<string, UIobjects> m_UIObjects;
	vector<CUI_Object*> DeleteUIs;
	string m_nowLevelKey = {};
public:
	static CUI_Manager* Create();
	virtual void Free() override;
};
NS_END
