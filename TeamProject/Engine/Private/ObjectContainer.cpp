#include "Engine_Defines.h"
#include "ObjectContainer.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "IObjectService.h"
#include "Child.h"
#include "UI_Object.h"

CObjectContainer::CObjectContainer()
{
}

CObjectContainer::CObjectContainer(const CObjectContainer& rhs)
	:CComponent(rhs)
{
	//*Initialize Prototype시에는 다른 오브젝트들과 부모자식 관계 설정하지 않는 것으로*//
}

CObjectContainer::~CObjectContainer()
{
}

HRESULT CObjectContainer::Initialize_Prototype()
{
	//*Initialize Prototype시에는 다른 오브젝트들과 부모자식 관계 설정하지 않는 것으로*//
    return S_OK;
}

HRESULT CObjectContainer::Initialize(COMPONENT_DESC* pArg)
{
	//각자가 자기것만 담당하는 걸로. 단, 이때에는 추가된 자식에 대한 레이어 추가는 진행이 알아서 됨.

    return S_OK;
}

void CObjectContainer::Priority_UpdateChild(_float dt)
{
	for (auto& child : m_ChildrenObjects)
		if (child && child->Is_Alive()) child->Priority_Update(dt);
}

void CObjectContainer::UpdateChild(_float dt)
{
	for (auto& child : m_ChildrenObjects) 
		if (child && child->Is_Alive()) child->Update(dt);
}

void CObjectContainer::Late_UpdateChild(_float dt)
{
	for (auto& child : m_ChildrenObjects)
		if (child && child->Is_Alive()) child->Late_Update(dt);
}

	/*자식 전체 순회 -> 효율 떨어짐*/
CGameObject* CObjectContainer::Find_ObjectByName(const string& ObjectName)
{

	for (const auto& name : m_ChildrensName) {
		if (ObjectName == name.second) {
			auto IndexIter = m_IndexByID.find(name.first);
			if (IndexIter != m_IndexByID.end()) {
				return m_ChildrenObjects[IndexIter->second];
			}
		}
	}

	return nullptr;
}

CGameObject* CObjectContainer::Find_ObjectByID(_uint ObjectID)
{
	auto iter = m_IndexByID.find(ObjectID);
	if (iter == m_IndexByID.end()){
		return nullptr;
	}
	else {
		_uint objIndex = iter->second;
		return m_ChildrenObjects[objIndex];
	}
}

/*자식 전체 순회 -> 효율 떨어짐*/
_int CObjectContainer::Find_IndexByName(const string& ObjectName)
{
	for (const auto& name : m_ChildrensName) {
		if (ObjectName == name.second) {
			auto IndexIter = m_IndexByID.find(name.first);
			if (IndexIter != m_IndexByID.end()) {
				return static_cast<_int>(IndexIter->second);
			}
		}
	}

	return -1;
}

_int CObjectContainer::Find_IndexByID(_uint ObjectID)
{
	auto iter = m_IndexByID.find(ObjectID);
	if (iter == m_IndexByID.end()) {
		return -1;
	}
	else {
		return static_cast<_int>(iter->second);
	}
}

/*IParent-> vector<CGamObject*> m_CHilds*/
/*UPdate_Child(dt)*/

_int CObjectContainer::Add_Child(CGameObject* pObject, _bool SyncTransform )
{
	if (nullptr == pObject) return -1;

	_uint ObjectID = pObject->Get_ObjectID();

	if (m_IndexByID.count(ObjectID)) {
		/*이미 추가되어 있음.*/
		return m_IndexByID[ObjectID];
	}

	_uint ObjectIndex = m_ChildrenObjects.size();

	for (size_t i = 0; i < m_ChildrenObjects.size(); i++)
	{	//중간에 비어잇다면 널포인터
		if (nullptr == m_ChildrenObjects[i]) {
			ObjectIndex = i;
			break;
		}
	}

	if (ObjectIndex == m_ChildrenObjects.size()) //마지막에 추가
	{
		m_ChildrenObjects.push_back(pObject);
	}
	else {
		m_ChildrenObjects[ObjectIndex] = pObject;
	}
	

	CChild* child = pObject->Add_Component<CChild>(m_pOwner);

	if(dynamic_cast<CUI_Object*>(pObject))
		child->Sync_To_Parent(false);
	else
		child->Sync_To_Parent(SyncTransform);

	string name = pObject->Get_InstanceName();
	m_ChildrensName.emplace(ObjectID, name);
	m_IndexByID.emplace(ObjectID, ObjectIndex);
	Safe_AddRef(pObject);

	return static_cast<int>(ObjectIndex);
}

void CObjectContainer::Destroy_Child(_uint ChildIndex)
{
	if (m_ChildrenObjects.size() <= ChildIndex)
		return;

	CGameObject* target= m_ChildrenObjects[ChildIndex];
	  
	auto nameIter = m_ChildrensName.find(target->Get_ObjectID());
	m_ChildrensName.erase(nameIter);
	auto IndexIter = m_IndexByID.find(target->Get_ObjectID());
	m_IndexByID.erase(IndexIter);

	m_ChildrenObjects[ChildIndex] = nullptr;

	//여기서 자식 오브젝트를 지우는 작업 필요.
	//해당 컴포넌트 안에서도 지우고, 레이어에서도 지우는 작업이 필요함. 
	CGameInstance::GetInstance()->Get_ObjectMgr()->Remove_Object(target);
	Safe_Release(target);
}

void CObjectContainer::Dettach_Child(_uint ChildIndex)
{
	if (m_ChildrenObjects.size() <= ChildIndex)
		return;

	CGameObject* target = m_ChildrenObjects[ChildIndex];

	auto nameIter = m_ChildrensName.find(target->Get_ObjectID());
	m_ChildrensName.erase(nameIter);
	auto IndexIter = m_IndexByID.find(target->Get_ObjectID());
	m_IndexByID.erase(IndexIter);

	m_ChildrenObjects[ChildIndex] = nullptr;

	//여기서 자식 오브젝트들 떼어내는 작업 필요.
	//떼어냈지만 레이어에는 살아있고, 바로 아래 단계 자식들은 루트 오브젝트가 되어야 함.
	target->Get_Component<CChild>()->Dettach_Parent();
	target->Remove_Component<CChild>();
	Safe_Release(target);
}

CObjectContainer* CObjectContainer::Create()
{
	CObjectContainer* instance = new CObjectContainer();

	if (FAILED(instance->Initialize_Prototype())) {
		MSG_BOX("CObjectContainer Create Failed : CObjectContainer");
		Safe_Release(instance);
	}

	return instance;
}

CComponent* CObjectContainer::Clone()
{
	CObjectContainer* instance = new CObjectContainer(*this);
	return instance;
}

void CObjectContainer::Free()
{
	__super::Free();
	if (m_ChildrenObjects.empty()) return;

	for (auto& child : m_ChildrenObjects) {
		if(child)
		child->Get_Component<CChild>()->Dettach_Parent();
		Safe_Release(child);
	}

	m_ChildrenObjects.clear();
	m_ChildrensName.clear();
	m_IndexByID.clear();
}
