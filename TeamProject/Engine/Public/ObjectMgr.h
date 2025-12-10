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

private:
	 void Add_Object_Recursive(CLayer* pLayer, class CGameObject* object);

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	unordered_map<string, LAYERS> m_Layers; //(레벨 태그 / (레이어 태그/레이어))

	/*object Array to Delete*/
	vector<CGameObject*> DeleteObjs;

public:
	static CObjectMgr* Create();
	virtual void Free() override;
};
NS_END
