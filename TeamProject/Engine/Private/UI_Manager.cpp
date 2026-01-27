#include "Engine_Defines.h"
#include "UI_Manager.h"
#include "UI_Object.h"
#include "GameInstance.h"
#include "ILevelService.h"
#include "Level.h"
#include "UI_Pool.h"
CUI_Manager::CUI_Manager()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	m_pUIPool = CUI_Pool::Create();
}

CUI_Manager::~CUI_Manager()
{
}

void CUI_Manager::Pre_EngineUpdate(_float dt)
{
	CleanUp();
	m_nowLevelKey = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey();

	auto itLevel = m_UIObjects.find(m_nowLevelKey);
	if (itLevel == m_UIObjects.end())
		return;

	for (auto* uiObj : itLevel->second)
		if (uiObj && uiObj->Is_Alive() && uiObj->Is_Root())
			uiObj->Pre_EngineUpdate(dt);
}

void CUI_Manager::Post_EngineUpdate(_float dt)
{
	Sort_UI();

	auto itLevel = m_UIObjects.find(m_nowLevelKey);
	if (itLevel == m_UIObjects.end())
		return;

	for (auto* uiObj : m_SortedUIObjects)
		uiObj->Post_EngineUpdate(dt);
}

void CUI_Manager::Priority_Update(_float dt)
{
	auto itLevel = m_UIObjects.find(m_nowLevelKey);
	if (itLevel == m_UIObjects.end())
		return;

	for (auto* uiObj : itLevel->second)
		if (uiObj && uiObj->Is_Alive() && uiObj->Is_Root())
			uiObj->Priority_Update(dt);
}

void CUI_Manager::Update(_float dt)
{
	auto itLevel = m_UIObjects.find(m_nowLevelKey);
	if (itLevel == m_UIObjects.end())
		return;

	for (auto* uiObj : itLevel->second)
		if (uiObj && uiObj->Is_Alive() && uiObj->Is_Root())
			uiObj->Update(dt);
}

void CUI_Manager::Late_Update(_float dt)
{

	auto itLevel = m_UIObjects.find(m_nowLevelKey);
	if (itLevel == m_UIObjects.end())
		return;

	for (auto* uiObj : itLevel->second)
		if (uiObj && uiObj->Is_Alive() && uiObj->Is_Root())
			uiObj->Late_Update(dt);
}

void CUI_Manager::Clear(const string& LevelTag)
{
	auto iter = m_UIObjects.find(LevelTag);
	if (iter != m_UIObjects.end())
	{
		Prune_Queues_ByLevel(LevelTag);
		for (auto& UI : iter->second)
			Safe_Release(UI);
		iter->second.clear();
	}
}

HRESULT CUI_Manager::Sync_To_Level()
{
	ILevelService* pLevelMgr = CGameInstance::GetInstance()->Get_LevelMgr();

	if (!pLevelMgr) {
		MSG_BOX("There is No Level in Level Manager : CPrototypeMgr");
		return E_FAIL;
	}

	vector<string> LevelList = pLevelMgr->Get_LevelList();

	for (string& name : LevelList)
		m_UIObjects.emplace(name, UIobjects());
	return S_OK;
}

HRESULT CUI_Manager::Add_UIObject(CUI_Object* object, const string& level)
{
	if (!object)
		return E_FAIL;
	if (object->Get_SystemIndex() != -1)
		return E_FAIL;

	if (!m_UIObjects.count(level)) {
		MSG_BOX(" wrong Destination Level  : CUI_Manager");
		return E_FAIL;
	}

	auto& map = m_UIObjects.at(level);
	Add_Object_Recursive(level, object);
	return S_OK;
}

void CUI_Manager::Add_Object_Recursive(const string& LevelTag, CUI_Object* object)
{
	auto& map = m_UIObjects.at(LevelTag);
	_uint ObjectIndex = map.size();

	if(object && object->Get_SystemIndex() == -1){
	for (size_t i = 0; i < map.size(); i++)
	{
		/*벡터를 순회하면서 널포인터가 있는지 검색*/
		if (map[i] == nullptr) {
			ObjectIndex = i;
			break;
		}
	}

	/*같은 ID의 오브젝트가 없다면*/
	if (ObjectIndex == map.size()) /*마지막에 추가*/
		map.push_back(object);
	else
		map[ObjectIndex] = object;

	object->Set_OnSystem(LevelTag, ObjectIndex);
	}
	auto vector = object->Get_Children();

	if (vector.empty()) return;

	for (auto& pChild : object->Get_Children()) {
		CUI_Object* CastChild = dynamic_cast<CUI_Object*>(pChild);
		if (CastChild)
			Add_Object_Recursive(LevelTag, CastChild);
	}
}

void CUI_Manager::Remove_UIObject(CUI_Object* object)
{
	if (!object) return;
	const _int systemIndex = object->Get_SystemIndex();
	if (systemIndex < 0) return;
	/*유아이에 배치된 적 없음*/
	const auto level = object->Get_SystemLevel();
	auto itLevel = m_UIObjects.find(level);
	if (itLevel == m_UIObjects.end())
		return;
	auto& vec = itLevel->second;
	if (systemIndex >= static_cast<_int>(vec.size()))
		return;
	if (vec[systemIndex] != object)
		return;
	/*여기까지가 말 안되는 오브젝트가 들어옴*/

	if (object->IsFromPool())
	{
		if (m_ReleaseUI_IDs.count(object->Get_ObjectID()))
			return;
		m_ReleaseUIs.push_back(object);
		m_ReleaseUI_IDs.insert(object->Get_ObjectID());
	}
	else {
		if (DeleteUI_IDs.count(object->Get_ObjectID()))
			return;
		DeleteUIs.push_back(object);
		DeleteUI_IDs.insert(object->Get_ObjectID());
	}
}

void CUI_Manager::Release_Subtree_ToPool(CUI_Object* root)
{
	if (!root) return;

	vector<CUI_Object*> nodes;
	nodes.reserve(32);

	vector<CUI_Object*> stack;
	stack.reserve(32);
	stack.push_back(root);

	while (!stack.empty())
	{
		CUI_Object* node = stack.back();
		stack.pop_back();
		if (!node) continue;

		nodes.push_back(node);

		auto children = node->Get_Children();
		for (auto* childBase : children)
		{
			CUI_Object* child = dynamic_cast<CUI_Object*>(childBase);
			if (child) stack.push_back(child);
		}
	}

	for (CUI_Object* node : nodes)
	{
		if (!node) continue;
	
		//const _int idx = node->Get_SystemIndex();
		//if (idx >= 0)
		//{
		//	const auto levelKey = node->Get_SystemLevel();
		//	auto itLevel = m_UIObjects.find(levelKey);
		//	if (itLevel != m_UIObjects.end())
		//	{
		//		auto& vec = itLevel->second;
		//		if (idx < static_cast<_int>(vec.size()) && vec[idx] == node)
		//			vec[idx] = nullptr;
		//	}
		//}
		//
		//node->Set_OnSystem("", -1);
	
		if (node != root)
		{
			node->OnPooledRelease();
		}
	}

	root->OnPooledRelease();
	root->Set_OnSystem("", -1);
	const CLONE_DESC& poolKey = root->Get_PoolKey();
	m_pUIPool->Return(poolKey, root);
}


void CUI_Manager::CleanUp()
{
	for (CUI_Object* obj : DeleteUIs)
	{
		if (!obj) continue;

		const _int idx = obj->Get_SystemIndex();
		if (idx < 0) continue;

		const auto levelKey = obj->Get_SystemLevel();

		auto itLevel = m_UIObjects.find(levelKey);
		if (itLevel == m_UIObjects.end()) continue;

		auto& vec = itLevel->second;
		if (idx >= static_cast<_int>(vec.size())) continue;

		// 아직 그 슬롯에 그 객체가 있을 때만 제거
		if (vec[idx] != obj) continue;

		// 시스템 연결 끊기
		obj->Set_OnSystem("", -1);

		Safe_Release(vec[idx]);
		vec[idx] = nullptr;
	}
	DeleteUIs.clear();
	DeleteUI_IDs.clear();

	for (CUI_Object* obj : m_ReleaseUIs)
	{
		if (!obj) continue;
		Release_Subtree_ToPool(obj);
	}
	m_ReleaseUIs.clear();
	m_ReleaseUI_IDs.clear();
}

static vector<CUI_Object*> emptyVec;

const vector<CUI_Object*>& CUI_Manager::Get_LevelUI(const string& leveTag)
{
	auto iter = m_UIObjects.find(leveTag);
	if (iter != m_UIObjects.end()) {
		return iter->second;
	}

	return emptyVec;
}

CUI_Object* CUI_Manager::Request_UIObject(const UI_HANDLE& handle)
{
	// 삭제 예정이면 실패
	auto itDelete = std::find_if(DeleteUIs.begin(), DeleteUIs.end(),
		[&](CUI_Object* uiPtr)
		{
			return uiPtr && uiPtr->Get_ObjectID() == handle.hObjID;
		});

	if (itDelete != DeleteUIs.end())
		return nullptr;

	const bool hasLevel = !handle.Level.empty();

	if (hasLevel)
	{
		auto itLevel = m_UIObjects.find(handle.Level);
		if (itLevel != m_UIObjects.end())
		{
			UIobjects& uiList = itLevel->second;

			if (handle.SystemIndex >= 0 &&
				handle.SystemIndex < static_cast<_int>(uiList.size()))
			{
				CUI_Object* candidate = uiList[handle.SystemIndex];
				if (candidate && candidate->Get_ObjectID() == handle.hObjID)
					return candidate;
			}

			auto itUI = std::find_if(uiList.begin(), uiList.end(),
				[&](CUI_Object* uiPtr)
				{
					return uiPtr && uiPtr->Get_ObjectID() == handle.hObjID;
				});

			if (itUI != uiList.end())
				return *itUI;
		}
	}

	for (auto& levelPair : m_UIObjects)
	{
		UIobjects& uiList = levelPair.second;

		auto itUI = std::find_if(uiList.begin(), uiList.end(),
			[&](CUI_Object* uiPtr)
			{
				return uiPtr && uiPtr->Get_ObjectID() == handle.hObjID;
			});

		if (itUI != uiList.end())
			return *itUI;
	}

	return nullptr;
}

CUI_Object* CUI_Manager::Acquire(const CLONE_DESC& desc, INIT_DESC* pArg)
{
	return m_pUIPool->Acquire(desc, pArg);
}

void CUI_Manager::Prune_Queues_ByLevel(const string& levelTag)
{
	auto IsSameLevel = [&](CUI_Object* obj) -> bool
		{
			if (!obj) return false;
			return obj->Get_Level() == levelTag;
		};

	{
		vector<CUI_Object*> newList;
		newList.reserve(m_ReleaseUIs.size());

		for (auto* obj : m_ReleaseUIs)
		{
			if (!IsSameLevel(obj))
				newList.push_back(obj);
			else
				m_ReleaseUI_IDs.erase(obj->Get_ObjectID());
		}
		m_ReleaseUIs.swap(newList);
	}

	{
		vector<CUI_Object*> newList;
		newList.reserve(DeleteUIs.size());

		for (auto* obj : DeleteUIs)
		{
			if (!IsSameLevel(obj))
				newList.push_back(obj);
			else
				DeleteUI_IDs.erase(obj->Get_ObjectID());
		}
		DeleteUIs.swap(newList);
	}
}

void CUI_Manager::Sort_UI()
{
	m_SortedUIObjects.clear();
	m_SortedUIObjects.reserve(256);

	for (auto& pair : m_UIObjects)
	{
		for (CUI_Object* ui : pair.second)
		{
			if (ui && ui->Is_Alive() && ui->Is_Root())
				m_SortedUIObjects.push_back(ui);
		}
	}

	std::stable_sort(m_SortedUIObjects.begin(), m_SortedUIObjects.end(),
		[](CUI_Object* left, CUI_Object* right)
		{
			return left->Get_ZPriority() > right->Get_ZPriority();
		});
}

CUI_Manager* CUI_Manager::Create()
{
	CUI_Manager* pInstance = new CUI_Manager();
	return pInstance;
}

void CUI_Manager::Free()
{
	__super::Free();

	for (auto& pair : m_UIObjects)
		for (auto& UI : pair.second)
			Safe_Release(UI);

	Safe_Release(m_pUIPool);
	Safe_Release(m_pGameInstance);
}


