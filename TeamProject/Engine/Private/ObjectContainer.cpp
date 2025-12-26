#include "Engine_Defines.h"
#include "ObjectContainer.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "IObjectService.h"
#include "Child.h"
#include "UI_Object.h"
#include "Layer.h"

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

void CObjectContainer::Pre_EngineUpdateChild(_float dt)
{
	for (_uint id : m_UpdateOrder)
	{
		if (id == 0) continue;

		auto indexIter = m_IndexByID.find(id);
		if (indexIter == m_IndexByID.end()) continue;

		CGameObject* child = m_ChildrenObjects[indexIter->second];
		if (child && child->Is_Alive())
			child->Pre_EngineUpdate(dt);
	}
}

void CObjectContainer::Post_EngineUpdateChild(_float dt)
{
	for (_uint id : m_UpdateOrder)
	{
		if (id == 0) continue;

		auto indexIter = m_IndexByID.find(id);
		if (indexIter == m_IndexByID.end()) continue;

		CGameObject* child = m_ChildrenObjects[indexIter->second];
		if (child && child->Is_Alive())
			child->Post_EngineUpdate(dt);
	}
}

void CObjectContainer::Priority_UpdateChild(_float dt)
{
	for (_uint id : m_UpdateOrder)
	{
		if (id == 0) continue;

		auto indexIter = m_IndexByID.find(id);
		if (indexIter == m_IndexByID.end()) continue;

		CGameObject* child = m_ChildrenObjects[indexIter->second];
		if (child && child->Is_Alive())
			child->Priority_Update(dt);
	}
}

void CObjectContainer::UpdateChild(_float dt)
{
	for (_uint id : m_UpdateOrder)
	{
		if (id == 0) continue;

		auto indexIter = m_IndexByID.find(id);
		if (indexIter == m_IndexByID.end()) continue;

		CGameObject* child = m_ChildrenObjects[indexIter->second];
		if (child && child->Is_Alive())
			child->Update(dt);
	}
}

void CObjectContainer::Late_UpdateChild(_float dt)
{
	for (_uint id : m_UpdateOrder)
	{
		if (id == 0) continue;

		auto indexIter = m_IndexByID.find(id);
		if (indexIter == m_IndexByID.end()) continue;

		CGameObject* child = m_ChildrenObjects[indexIter->second];
		if (child && child->Is_Alive())
			child->Late_Update(dt);
	}
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
	if (iter == m_IndexByID.end()) {
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

_int CObjectContainer::Add_Child(CGameObject* pObject, _bool SyncTransform)
{
	if (nullptr == pObject) return -1;

	_uint ObjectID = pObject->Get_ObjectID();

	if (m_IndexByID.count(ObjectID)) {
		/*이미 추가되어 있음.*/
		return m_IndexByID[ObjectID];
	}
	_uint ObjectIndex = m_ChildrenObjects.size();
	m_ChildrenObjects.push_back(pObject);

	for (size_t i = 0; i < m_ChildrenObjects.size(); i++)
	{	//중간에 비어잇다면 널포인터
		if (nullptr == m_ChildrenObjects[i]) {
			ObjectIndex = i;
			break;
		}
	}

	if (ObjectIndex == m_ChildrenObjects.size()) //마지막에 추가
		m_ChildrenObjects.push_back(pObject);
	else
		m_ChildrenObjects[ObjectIndex] = pObject;

	CChild* child = pObject->Add_Component<CChild>(m_pOwner);
	CUI_Object* ownerCast = dynamic_cast<CUI_Object*>(m_pOwner);
	_bool isUI = (ownerCast != nullptr);

	if (isUI)
		child->Sync_To_Parent(false);
	else
		child->Sync_To_Parent(SyncTransform);

	string name = pObject->Get_InstanceName();
	m_ChildrensName.emplace(ObjectID, name);
	m_IndexByID[ObjectID] = ObjectIndex;
	m_ChildrensName[ObjectID] = pObject->Get_InstanceName();

	// 각 아이디 별로 들어온 순서 기록
	m_OrderIndexByID[ObjectID] = (_uint)m_UpdateOrder.size();
	m_UpdateOrder.push_back(ObjectID);

	Safe_AddRef(pObject);

	/*부모의 업데이트 동안에, 차일드가 추가됨*/
	if (isUI) {
		if (!ownerCast->Get_Level().empty()) {
			CUI_Object* uiCast = dynamic_cast<CUI_Object*>(pObject);
			CGameInstance::GetInstance()->Get_UIMgr()->Add_UIObject(uiCast, ownerCast->Get_Level());
		}
	}
	else {
		/*레이어가 있으면*/
		if (m_pOwner->Get_Layer()) { 
			CGameInstance::GetInstance()->Get_ObjectMgr()->Add_Object(pObject, m_pOwner->Get_LayerDesc());
		}
	}

	return static_cast<int>(ObjectIndex);
}

void CObjectContainer::Destroy_Child(_uint ChildIndex)
{
	if (m_ChildrenObjects.size() <= ChildIndex)
		return;

	CGameObject* target = m_ChildrenObjects[ChildIndex];

	auto nameIter = m_ChildrensName.find(target->Get_ObjectID());
	m_ChildrensName.erase(nameIter);
	auto IndexIter = m_IndexByID.find(target->Get_ObjectID());
	m_IndexByID.erase(IndexIter);

	m_ChildrenObjects[ChildIndex] = nullptr;

	CUI_Object* uiCast = dynamic_cast<CUI_Object*>(target);
	_bool isUI = (uiCast != nullptr);

	if (isUI) {
		CGameInstance::GetInstance()->Get_UIMgr()->Remove_UIObject(uiCast);
		Safe_Release(uiCast);
	}
	else {
		CGameInstance::GetInstance()->Get_ObjectMgr()->Remove_Object(target);
		Safe_Release(target);
	}
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

void CObjectContainer::Render_GUI()
{
	ImGui::SeparatorText("Object_Container");
	float childWidth = ImGui::GetContentRegionAvail().x;
	const float textLineHeight = ImGui::GetTextLineHeightWithSpacing();
	const float childHeight = (textLineHeight * (m_ChildrenObjects.size() + 3)) + (ImGui::GetStyle().WindowPadding.y * 2);

	ImGui::BeginChild("##AudioSourceChild", ImVec2{ 0, childHeight }, true);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
	for (size_t i = 0; i < m_ChildrenObjects.size(); i++)
	{
		if (m_ChildrenObjects[i] == nullptr)
			continue;

		string btnName = "Delete : " + m_ChildrenObjects[i]->Get_InstanceName() + to_string(i);

		if (ImGui::Button(btnName.c_str())) {
			Destroy_Child(i);
		}

		if (ImGui::IsItemHovered()) {
			ImGui::BeginTooltip();
			ImGui::Text(btnName.c_str());
			ImGui::EndTooltip();
		}
	}
	ImGui::PopStyleVar();
	ImGui::EndChild();
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
		if (child)
			child->Get_Component<CChild>()->Dettach_Parent();
		Safe_Release(child);
	}

	m_ChildrenObjects.clear();
	m_ChildrensName.clear();
	m_IndexByID.clear();
}
