#include "ObjectMgr.h"
#include "GameInstance.h"
#include "ILevelService.h"
#include "IProtoService.h"
#include "GameObject.h"
#include "Layer.h"
#include "GameObjectBuilder.h"
#include "ILevelService.h"

CObjectMgr::CObjectMgr()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CObjectMgr::Initialize()
{
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

	for (auto pObject : DeleteObjs)
	{
		_uint ObjectID = pObject->Get_ObjectID();
		pObject->Get_Layer()->Remove_GameObject(ObjectID);
		pObject->Set_Layer(nullptr);
	}

	DeleteObjs.clear();
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
		pDestLayer = CLayer::Create();
		map.emplace(layer.LayerTag, pDestLayer);
	}
	else {
		pDestLayer = iter->second;
	}
	Add_Object_Recursive(pDestLayer, object);
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

void CObjectMgr::Remove_Object(CGameObject* object)
{
	if (!object)
		return;

	auto it = find(DeleteObjs.begin(), DeleteObjs.end(), object);
	if (it != DeleteObjs.end())
		return;

	DeleteObjs.push_back(object);
}

void CObjectMgr::Change_Layer(const LAYER_DESC& SrcLayer, CGameObject* object, const LAYER_DESC& DstLayer)
{
	_uint ObjectID = object->Get_ObjectID();

	auto& SrcMap = m_Layers.at(SrcLayer.LevelTag);
	auto& DstMap = m_Layers.at(DstLayer.LevelTag);

	if (!m_Layers.count(SrcLayer.LevelTag)) {
		MSG_BOX(" wrong  Source Level Tag : CObjectMgr");
		return;
	}
	if (!m_Layers.count(DstLayer.LevelTag)) {
		MSG_BOX(" wrong  Dest Level Tag : CObjectMgr");
		return;
	}
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

	for (auto& pair : m_Layers[LevelTag]) {
		Safe_Release(pair.second);
	}
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
}

const unordered_map<string, class CLayer*>& CObjectMgr::Get_LevelLayer(const string& LevelTag)
{
	auto iter = m_Layers.find(LevelTag);
	if (iter == m_Layers.end())
		return unordered_map<string, class CLayer*>();
	else
		return iter->second;
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

	for (auto& pair : m_Layers) {
		Clear(pair.first);
	}
	m_Layers.clear();

	Safe_Release(m_pGameInstance);
}
