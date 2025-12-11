#include "Engine_Defines.h"
#include "Layer.h"
#include "GameObject.h"
#include "GUIWidget.h"

CLayer::CLayer()
{
}

HRESULT CLayer::Add_GameObject(CGameObject* pGameObject)
{
	if (!pGameObject)
		return E_FAIL;

	_uint ObjectIndex = m_GameObjects.size();
	for (size_t i = 0; i < m_GameObjects.size(); i++)
	{
		/*벡터를 순회하면서 널포인터가 있는지 검색*/
		if (m_GameObjects[i] == nullptr) {
			ObjectIndex = i;
			break;
		}
	}

	if (m_IndexByID.count(pGameObject->Get_ObjectID()))
		return E_FAIL;

	/*같은 ID의 오브젝트가 없다면*/
	if(ObjectIndex == m_GameObjects.size()) /*마지막에 추가*/
		m_GameObjects.push_back(pGameObject);
	else
		m_GameObjects[ObjectIndex] = pGameObject;

	pGameObject->Set_Layer(this);
	m_IndexByID[pGameObject->Get_ObjectID()] = ObjectIndex;
	return S_OK;
}

void CLayer::Pre_EngineUpdate(_float dt)
{
	for (auto& pGameObject : m_GameObjects)
		if (pGameObject && pGameObject->Is_Alive() && pGameObject->Is_Root())
			pGameObject->Pre_EngineUpdate(dt);
}

void CLayer::Post_EngineUpdate(_float dt)
{
	if (!m_isRender) return;

	for (auto& pGameObject : m_GameObjects)
		if (pGameObject && pGameObject->Is_Alive() && pGameObject->Is_Alive() && pGameObject->Is_Root())
			pGameObject->Post_EngineUpdate(dt);
}

void CLayer::Priority_Update(_float dt)
{
	for (auto& pGameObject : m_GameObjects)
		if (pGameObject && pGameObject ->Is_Alive() && pGameObject->Is_Root())
			pGameObject->Priority_Update(dt);
}

void CLayer::Update(_float dt)
{
	for (auto& pGameObject : m_GameObjects)
		if (pGameObject && pGameObject->Is_Alive() && pGameObject->Is_Root())
			pGameObject->Update(dt);
}

void CLayer::Late_Update(_float dt)
{
	for (auto& pGameObject : m_GameObjects)
		if (pGameObject && pGameObject->Is_Alive() && pGameObject->Is_Root())
			pGameObject->Late_Update(dt);
}

CGameObject* CLayer::Pop_GameObject(_uint ObjectID)
{
	auto iter = m_IndexByID.find(ObjectID);
	if (iter == m_IndexByID.end())
		return nullptr;

	const _uint removeIdx = iter->second;
	const _uint lastIdx = static_cast<_uint>(m_GameObjects.size() - 1);

	CGameObject* popped = m_GameObjects[removeIdx];
	popped->Set_Layer(nullptr);

	if (removeIdx != lastIdx)
	{
		m_GameObjects[removeIdx] = m_GameObjects[lastIdx];
		const _uint movedID = m_GameObjects[removeIdx]->Get_ObjectID();
		m_IndexByID[movedID] = removeIdx;
	}

	m_GameObjects.pop_back();
	m_IndexByID.erase(iter);

	return popped;
}

void CLayer::Remove_GameObject(_uint ObjectID)
{
	auto iter = m_IndexByID.find(ObjectID);

	if (iter == m_IndexByID.end())
		return ;

	const _uint removeIdx = iter->second; //삭제해야할 오브젝트 인덱스
	const _uint lastIdx = static_cast<_uint>(m_GameObjects.size() - 1); //마지막 친구

	if (auto pRemove = m_GameObjects[removeIdx])
	{
		pRemove->Set_Layer(nullptr);
		Safe_Release(pRemove); //일단 지움
	}

	if (removeIdx != lastIdx)
	{//만약 두개가 다르면 뒤에 있는거 끌어옴
		m_GameObjects[removeIdx] = m_GameObjects[lastIdx];

		//그리고 인덱스 매핑 재정의(바꾼 친구의 아이디 칸에 바뀐 인덱스 넣어줌
		m_IndexByID[m_GameObjects[removeIdx]->Get_ObjectID()] = removeIdx;
	}
	m_GameObjects.pop_back();
	m_IndexByID.erase(iter);
	//
	//else {
	//	_uint ObjectIndex = iter->second;
	//	m_GameObjects[ObjectIndex]->Set_Layer(nullptr);
	//	Safe_Release(m_GameObjects[ObjectIndex]);
	//	m_GameObjects[ObjectIndex] = nullptr;
	//	m_IndexByID.erase(iter);
	//}
}

CGameObject* CLayer::Find_ObjectByID(_uint ObjectID)
{
	auto iter = m_IndexByID.find(ObjectID);
	if (iter == m_IndexByID.end())
		return nullptr;

	else {
		_uint ObjectIndex = iter->second;
		return m_GameObjects[ObjectIndex];
	}
}

CLayer* CLayer::Create()
{
	return new CLayer();
}

CLayer* CLayer::Create(string layerTag)
{
	CLayer* instance = new CLayer();
	instance->m_LayerTag = layerTag;
	return instance;
}

void CLayer::Free()
{
	__super::Free();

	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject);

	m_GameObjects.clear();
	m_IndexByID.clear();
}
