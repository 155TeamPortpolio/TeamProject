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
}

void CUI_Manager::Pre_EngineUpdate(_float dt)
{

	for (auto pObject : DeleteUIs)
	{
		_uint ObjectID = pObject->Get_ObjectID();
		//pObject->Get_Layer()->Remove_GameObject(ObjectID);
		//pObject->Set_Layer(nullptr);
	}

	DeleteUIs.clear();
	
	m_nowLevelKey = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey();
	for (auto& UI : m_UIObjects[m_nowLevelKey])
		if (UI && UI->Is_Root())
			UI->Pre_EngineUpdate(dt);
}

void CUI_Manager::Post_EngineUpdate(_float dt)
{
	Sort_UI();

	for (auto& UI : m_UIObjects[m_nowLevelKey]) {
		if (UI && UI->Is_Root())
			UI->Post_EngineUpdate(dt);
	}
}

void CUI_Manager::Priority_Update(_float dt)
{
	for (auto& UI : m_UIObjects[m_nowLevelKey]) {
		if (UI && UI->Is_Root())
			UI->Priority_Update(dt);
	}
}

void CUI_Manager::Update(_float dt)
{
	for (auto& UI : m_UIObjects[m_nowLevelKey])
		if (UI && UI->Is_Root())
			UI->Update(dt);
}

void CUI_Manager::Late_Update(_float dt)
{
	for (auto& UI : m_UIObjects[m_nowLevelKey])
		if (UI && UI->Is_Root())
			UI->Late_Update(dt);
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

	if (object == nullptr)
		return;
	_int systemIndex = object->Get_SystemIndex();
	if (systemIndex == -1)
		return;
	auto& map = m_UIObjects.at(object->Get_SystemLevel());

	if (map.empty())
		return;
	if (systemIndex >= map.size())
		return;

	object->Set_OnSystem("", -1);
	Safe_Release(map[systemIndex]);
	map[systemIndex] = nullptr;
}


void CUI_Manager::Sort_UI()
{
	for (auto& pair : m_UIObjects) {
		sort(pair.second.begin(), pair.second.end(),
			[&](CUI_Object* a, CUI_Object* b) {
				return a->Get_Priority() < b->Get_Priority();
			});
	}
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


