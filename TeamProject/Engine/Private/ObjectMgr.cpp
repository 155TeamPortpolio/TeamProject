#include "Engine_Defines.h"
#include "ObjectMgr.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Layer.h"
#include "GameObjectBuilder.h"

#include "ObjectPool.h"

CObjectMgr::CObjectMgr()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CObjectMgr::Initialize()
{
	m_pObjectPool = CObjectPool::Create();
	return S_OK;
}

void CObjectMgr::Pre_EngineUpdate(_float dt)
{
	for (auto& pair : m_Layers)
		for (auto& layers : pair.second)
			layers.second->Pre_EngineUpdate(dt);
}

void CObjectMgr::Post_EngineUpdate(_float dt)
{
	for (auto& pair : m_Layers)
		for (auto& layers : pair.second)
			layers.second->Post_EngineUpdate(dt);
}

void CObjectMgr::Priority_Update(_float dt)
{
	for (auto& pair : m_Layers)
		for (auto& layers : pair.second)
			layers.second->Priority_Update(dt);
}

void CObjectMgr::Update(_float dt)
{
	for (auto& pair : m_Layers)
		for (auto& layers : pair.second)
			layers.second->Update(dt);
}

void CObjectMgr::Late_Update(_float dt)
{
	for (auto& pair : m_Layers)
		for (auto& layers : pair.second)
			layers.second->Late_Update(dt);

	for (auto pObject : m_ReleaseObjs)
	{
		_uint ObjectID = pObject->Get_ObjectID();
		pObject->Get_Layer()->Pop_GameObject(ObjectID);
		pObject->Set_Layer(nullptr);
		pObject->OnPooledRelease();
		const CLONE_DESC& poolKey = pObject->Get_PoolKey();

		m_pObjectPool->Return(poolKey, pObject);
	}
	m_ReleaseObjs.clear();
	m_ReleaseIDs.clear();

	for (auto pObject : m_DeleteObjs)
	{
		_uint ObjectID = pObject->Get_ObjectID();
		pObject->Get_Layer()->Remove_GameObject(ObjectID);
		pObject->Set_Layer(nullptr);
	}

	m_DeleteObjs.clear();
	m_DeleteIDs.clear();
}

void CObjectMgr::Add_Object(CGameObject* object, const LAYER_DESC& layer)
{
	if (!m_Layers.count(layer.LevelTag)) {
		MSG_BOX(" wrong Destination Level  : CObjectMgr");
		return;
	}
	if (nullptr == object) return;

	auto& map = m_Layers.at(layer.LevelTag);
	auto iter = map.find(layer.LayerTag);
	CLayer* pDestLayer = { nullptr };
	if (iter == map.end()) {
		pDestLayer = CLayer::Create(layer.LayerTag);
		map.emplace(layer.LayerTag, pDestLayer);
	}
	else {
		pDestLayer = iter->second;
	}
	Add_Object_Recursive(pDestLayer, object, layer.LevelTag);
}


void CObjectMgr::Add_Object_Recursive(CLayer* pLayer, CGameObject* object)
{
	pLayer->Add_GameObject(object);
	auto vector = object->Get_Children();

	if (vector.empty()) return;

	for (auto& pChild : object->Get_Children()) {
		Add_Object_Recursive(pLayer, pChild);
	}
}

void CObjectMgr::Add_Object_Recursive(CLayer* pLayer, CGameObject* object, string LevelTag)
{
	object->Set_Level(LevelTag);
	pLayer->Add_GameObject(object);
	auto vector = object->Get_Children();

	if (vector.empty()) return;

	for (auto& pChild : object->Get_Children()) {
		Add_Object_Recursive(pLayer, pChild);
	}
}

void CObjectMgr::Remove_Object(CGameObject* object)
{
	if (!object)
		return;
	if (object->IsFromPool())
	{
		auto it = find(m_ReleaseObjs.begin(), m_ReleaseObjs.end(), object);
		if (it != m_ReleaseObjs.end())
			return;

		m_ReleaseObjs.push_back(object);
		m_ReleaseIDs.insert(object->Get_ObjectID());
	}
	else
	{
		auto it = find(m_DeleteObjs.begin(), m_DeleteObjs.end(), object);
		if (it != m_DeleteObjs.end())
			return;

		m_DeleteObjs.push_back(object);
		m_DeleteIDs.insert(object->Get_ObjectID());
	}
}

void CObjectMgr::Change_Layer(const LAYER_DESC& SrcLayer, CGameObject* object, const LAYER_DESC& DstLayer)
{
	_uint ObjectID = object->Get_ObjectID();


	if (!m_Layers.count(SrcLayer.LevelTag)) {
		MSG_BOX(" wrong  Source Level Tag : CObjectMgr");
		return;
	}
	if (!m_Layers.count(DstLayer.LevelTag)) {
		MSG_BOX(" wrong  Dest Level Tag : CObjectMgr");
		return;
	}
	auto& SrcMap = m_Layers.at(SrcLayer.LevelTag);
	auto& DstMap = m_Layers.at(DstLayer.LevelTag);

	auto SrcIter = SrcMap.find(SrcLayer.LayerTag);
	auto DstIter = DstMap.find(DstLayer.LayerTag);

	if (SrcIter == SrcMap.end() || DstIter == DstMap.end()) { //그런 레이어 없음
		return;
	}
	else {
		auto& pSrcLayer = SrcIter->second;
		auto& pDstLayer = DstIter->second;
		CGameObject* pObj = pSrcLayer->Pop_GameObject(ObjectID);
		if (pObj)
			pDstLayer->Add_GameObject(pObj);
	}
}

void CObjectMgr::Clear(const string& LevelTag)
{
	if (!m_Layers.count(LevelTag)) {
		MSG_BOX("There is No Same Level Tag  : CObjectMgr");
		return;
	}

	Prune_Queues_ByLevel(LevelTag);

	for (auto& pair : m_Layers[LevelTag]) {
		Safe_Release(pair.second);
	}
	m_pObjectPool->ClearAll();
	m_Layers[LevelTag].clear();
}

HRESULT CObjectMgr::Sync_To_Level()
{
	ILevelService* pLevelMgr = CGameInstance::GetInstance()->Get_LevelMgr();

	if (!pLevelMgr) {
		MSG_BOX("There is No Level in Level Manager : CPrototypeMgr");
		return E_FAIL;
	}

	vector<string> LevelList = pLevelMgr->Get_LevelList();

	for (string& name : LevelList)
		m_Layers.emplace(name, LAYERS{});

	return S_OK;
}

const unordered_map<string, class CLayer*>& CObjectMgr::Get_LevelLayer(const string& LevelTag)
{
	static const unordered_map<string, CLayer*> kEmpty;
	auto it = m_Layers.find(LevelTag);
	return (it == m_Layers.end()) ? kEmpty : it->second;
}

CLayer* CObjectMgr::Get_Layer(const LAYER_DESC& SrcLayer)
{
	auto iter = m_Layers.find(SrcLayer.LevelTag);
	if (iter == m_Layers.end())
		return nullptr;
	else {
		auto LayerIter = iter->second.find(SrcLayer.LayerTag);
		if (LayerIter != iter->second.end()) {
			return LayerIter->second;
		}
	}
	return nullptr;
}

CGameObject* CObjectMgr::Request_Object(const OBJECT_HANDLE& handle)
{
	auto it = m_Layers.find(handle.Level);
	if (it == m_Layers.end()) return nullptr;

	auto& layers = it->second;
	auto lit = layers.find(handle.Layer);
	if (lit == layers.end()) return nullptr;

	if (m_DeleteIDs.count(handle.hObjID)) return nullptr;

	return lit->second->Find_ObjectByID(handle.hObjID);
}

CGameObject* CObjectMgr::Acquire(const CLONE_DESC& desc)
{
	return m_pObjectPool->Acquire(desc);
}

void CObjectMgr::Prune_Queues_ByLevel(const string& levelTag)
{
	auto IsSameLevel = [&](CGameObject* obj) -> bool
		{
			if (!obj) return false;
			return obj->Get_Level() == levelTag; 
		};

	{
		vector<CGameObject*> newList;
		newList.reserve(m_ReleaseObjs.size());

		for (auto* obj : m_ReleaseObjs)
		{
			if (!IsSameLevel(obj))
				newList.push_back(obj);
			else
				m_ReleaseIDs.erase(obj->Get_ObjectID());
		}
		m_ReleaseObjs.swap(newList);
	}

	{
		vector<CGameObject*> newList;
		newList.reserve(m_DeleteObjs.size());

		for (auto* obj : m_DeleteObjs)
		{
			if (!IsSameLevel(obj))
				newList.push_back(obj);
			else
				m_DeleteIDs.erase(obj->Get_ObjectID());
		}
		m_DeleteObjs.swap(newList);
	}
}

CObjectMgr* CObjectMgr::Create()
{
	CObjectMgr* pInstance = new CObjectMgr();

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CObjectMgr::Free()
{
	__super::Free();

	m_ReleaseObjs.clear(); m_ReleaseIDs.clear();
	m_DeleteObjs.clear();  m_DeleteIDs.clear();

	for (auto& pair : m_Layers)
		Clear(pair.first);
	m_Layers.clear();

	if (m_pObjectPool)
		m_pObjectPool->ClearAll();

	Safe_Release(m_pObjectPool);
	Safe_Release(m_pGameInstance);
}
