#include "Engine_Defines.h"
#include "UI_Manager.h"
#include "UI_Object.h"
#include "GameInstance.h"
#include "ILevelService.h"
#include "Level.h"
CUI_Manager::CUI_Manager()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

CUI_Manager::~CUI_Manager()
{
	//Builder::Create_Object().
}

void CUI_Manager::Pre_EngineUpdate(_float dt)
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

	m_nowLevelKey = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey();
	
	auto itLevel = m_UIObjects.find(m_nowLevelKey);
	if (itLevel == m_UIObjects.end())
		return; 

	for (auto* uiObj : itLevel->second)
		if (uiObj && uiObj->Is_Root())
			uiObj->Pre_EngineUpdate(dt);
}

void CUI_Manager::Post_EngineUpdate(_float dt)
{
	Sort_UI();

	auto itLevel = m_UIObjects.find(m_nowLevelKey);
	if (itLevel == m_UIObjects.end())
		return;

	for (auto* uiObj : itLevel->second)
		if (uiObj && uiObj->Is_Root())
			uiObj->Post_EngineUpdate(dt);
}

void CUI_Manager::Priority_Update(_float dt)
{
	auto itLevel = m_UIObjects.find(m_nowLevelKey);
	if (itLevel == m_UIObjects.end())
		return;

	for (auto* uiObj : itLevel->second)
		if (uiObj && uiObj->Is_Root())
			uiObj->Priority_Update(dt);
}

void CUI_Manager::Update(_float dt)
{
	auto itLevel = m_UIObjects.find(m_nowLevelKey);
	if (itLevel == m_UIObjects.end())
		return;

	for (auto* uiObj : itLevel->second)
		if (uiObj && uiObj->Is_Root())
			uiObj->Update(dt);
}

void CUI_Manager::Late_Update(_float dt)
{

	auto itLevel = m_UIObjects.find(m_nowLevelKey);
	if (itLevel == m_UIObjects.end())
		return;

	for (auto* uiObj : itLevel->second)
		if (uiObj && uiObj->Is_Root())
			uiObj->Late_Update(dt);
}

void CUI_Manager::Clear(const string& LevelTag)
{
	auto iter = m_UIObjects.find(LevelTag);
	if (iter != m_UIObjects.end())
	{
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
	if (!object)
		return;

	const _int systemIndex = object->Get_SystemIndex();
	if (systemIndex < 0)
		return;

	const auto level = object->Get_SystemLevel();

	auto itLevel = m_UIObjects.find(level);
	if (itLevel == m_UIObjects.end())
		return;

	auto& vec = itLevel->second;
	if (systemIndex >= static_cast<_int>(vec.size()))
		return;

	if (vec[systemIndex] != object)
		return;

	auto it = std::find(DeleteUIs.begin(), DeleteUIs.end(), object);
	if (it != DeleteUIs.end())
		return;

	DeleteUIs.push_back(object);
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
	if (handle.Level == "")
		return nullptr;

	auto itLevel = m_UIObjects.find(handle.Level);
	if (itLevel == m_UIObjects.end()) return nullptr;

	auto itDelete = find_if(DeleteUIs.begin(), DeleteUIs.end(), [&](CUI_Object* pUI) {
		if (pUI->Get_SystemIndex() == handle.hObjID)
			return true;
		return false;
		});
	if (itDelete != DeleteUIs.end()) return nullptr;

	return itLevel->second[handle.hObjID];
}

void CUI_Manager::Sort_UI()
{
	//for (auto& pair : m_UIObjects) {
	//	sort(pair.second.begin(), pair.second.end(),
	//		[&](CUI_Object* a, CUI_Object* b) {
	//			return a->Get_Priority() < b->Get_Priority();
	//		});
	//}
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

	Safe_Release(m_pGameInstance);
}


