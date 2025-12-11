#include "Engine_Defines.h"
#include "PrototypeMgr.h"
#include "GameInstance.h"
#include "ILevelService.h"
#include "GameObject.h"
#include "Component.h"

CPrototypeMgr::CPrototypeMgr()
{
}

HRESULT CPrototypeMgr::Initialize()
{
	return S_OK;
}

HRESULT CPrototypeMgr::Sync_To_Level()
{
	ILevelService* pLevelMgr = CGameInstance::GetInstance()->Get_LevelMgr();

	if (!pLevelMgr) {
		MSG_BOX("There is No Level in Level Manager : CPrototypeMgr");
		return E_FAIL;
	}

	vector<string> LevelList = pLevelMgr->Get_LevelList();

	for (string& name : LevelList)
		m_Prototypes.emplace(name, PROTOTYPES{});
}

HRESULT CPrototypeMgr::Add_ProtoType(const string& LevelTag, const string& protoTag, CGameObject* pProto)
{

	if (!m_Prototypes.count(LevelTag)) {
		MSG_BOX("Level Tag is not appropriate : CPrototypeMgr");
		return E_FAIL;
	}

	CGameObject* pData = Find_Prototype(LevelTag, protoTag);

	if (pData) {
#ifdef _DEBUG

		MSG_BOX("There is alreay same Tag : CPrototypeMgr");
#endif // _DEBUG

		return E_FAIL;
	}
	else {
		m_Prototypes[LevelTag].emplace(protoTag, pProto);
		return S_OK;
	}
}

CGameObject* CPrototypeMgr::Clone_Prototype(const string& LevelTag, const string& protoTag, INIT_DESC* pArg)
{
	if (!m_Prototypes.count(LevelTag)) {
		MSG_BOX("Level Tag is not appropriate : CPrototypeMgr");
		return nullptr;
	}

	CGameObject* pData = Find_Prototype(LevelTag, protoTag);

	if (!pData) {
		MSG_BOX("There is no same Tag : CPrototypeMgr");
		return nullptr;
	}

	return pData->Clone(pArg);
}

void CPrototypeMgr::Clear(const string& LevelTag)
{
	if (!m_Prototypes.count(LevelTag)) {
		MSG_BOX("Level Tag is not appropriate : CPrototypeMgr");
		return;
	}
	if (!m_Prototypes[LevelTag].empty()) {
		for (auto& pair : m_Prototypes[LevelTag])
		{
			assert(pair.second != nullptr);          // nullptr 방어
			printf("Releasing %p\n", pair.second);   // 주소 확인

			Safe_Release(pair.second);
		}
	}

	m_Prototypes[LevelTag].clear();
}

CGameObject* CPrototypeMgr::Find_Prototype(const string& LevelTag, const string& protoTag)
{
	auto iter = m_Prototypes[LevelTag].find(protoTag);

	if (iter == m_Prototypes[LevelTag].end())
		return nullptr;
	else
		return iter->second;
}

CPrototypeMgr* CPrototypeMgr::Create()
{
	CPrototypeMgr* pInstance = new CPrototypeMgr();

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPrototypeMgr::Free()
{
	__super::Free();

	for (auto& pair : m_Prototypes) {
		Clear(pair.first);
	}
	m_Prototypes.clear();
}
