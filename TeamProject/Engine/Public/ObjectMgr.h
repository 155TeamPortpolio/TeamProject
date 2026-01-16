#pragma once
#include "IObjectService.h"


NS_BEGIN(Engine)

using LAYERS = unordered_map<string, class CLayer*>;

class CObjectMgr final : public IObjectService 
{
private:
	CObjectMgr();
	~CObjectMgr() DEFAULT;
	HRESULT Initialize();

public:
	virtual void Pre_EngineUpdate(_float dt) override;
	virtual void Post_EngineUpdate(_float dt) override;
	virtual void Priority_Update(_float dt) override;
	virtual void Update(_float dt) override;
	virtual void Late_Update(_float dt) override;

public:
	/*{level,proto}*/
	virtual void Add_Object(class CGameObject* object , const LAYER_DESC& layer) override;
	virtual void Remove_Object(class CGameObject* object ) override;
	virtual void Change_Layer(const LAYER_DESC& SrcLayer, class CGameObject* object, const LAYER_DESC& DstLayer) override;

	virtual void Clear(const string& LevelTag) override;
	virtual HRESULT Sync_To_Level() override;
	virtual const unordered_map<string, class CLayer*>& Get_LevelLayer(const string& LevelTag) override;
	virtual  CLayer* Get_Layer(const LAYER_DESC& SrcLayer) override;

public:
	virtual class CGameObject* Request_Object(const OBJECT_HANDLE& handle) override;
	virtual class CGameObject* Acquire(const CLONE_DESC& desc) override;

private:
	 void Add_Object_Recursive(CLayer* pLayer, class CGameObject* object);
	 void Add_Object_Recursive(CLayer* pLayer, class CGameObject* object,string LevelTag);
	 void Prune_Queues_ByLevel(const string& levelTag);

public:
	virtual void Set_LevelTimeScale(string LevelTag, _float scale) override;
	virtual void Reset_LevelTimeScale(string LevelTag) override;
	virtual void Set_LayerTimeScale(const LAYER_DESC& Layer, _float scale) override;
	virtual void Reset_LayerTimeScale(const LAYER_DESC& Layer) override;
	virtual _float Get_LayerTimeScale(const LAYER_DESC& Layer) override;

public:
	virtual _bool Remember_Global(_uint RememberKey, const OBJECT_HANDLE& handle, _bool replaceIfExists);
	virtual class CGameObject* Find_Global(_uint KeyID);
	virtual _bool Unregister_Global(_uint keyID);
	virtual void Clear_Global();

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	class CObjectPool* m_pObjectPool = { nullptr };
	unordered_map<string, LAYERS> m_Layers; //(레벨 태그 / (레이어 태그/레이어))

	/*object Array to Delete*/
	vector<CGameObject*> m_DeleteObjs;
	unordered_set<_uint> m_DeleteIDs;

	vector<CGameObject*> m_ReleaseObjs;
	unordered_set<_uint> m_ReleaseIDs;

	unordered_map<_uint, OBJECT_HANDLE> m_globalHandleByKey;

public:
	static CObjectMgr* Create();
	virtual void Free() override;
};
NS_END
