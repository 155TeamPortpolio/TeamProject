#include "pch.h"
#include "LoadingLevel.h"
#include "GameInstance.h"

/* UI */
#include "UIDirector.h"

CLoadingLevel::CLoadingLevel(const string& LevelKey)
	:CLevel(LevelKey),
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CLoadingLevel::Initialize()
{
	return S_OK;
}

HRESULT CLoadingLevel::Awake()
{
	//==================== UI ===============
	auto uiDirector = CUIDirector::GetInstance();
	uiDirector->Load_LevelObjects("Loading_Level");

	auto transDesc = CGameInstance::GetInstance()->Get_LevelMgr()->Get_TransitionDesc();
	m_NextLevel=transDesc.nextLevelKey;
	PreLoadLevel();
	return S_OK;
}

void CLoadingLevel::Update()
{
	/*쓰레드에게 미리 넣어둔 요청 큐를 펌핑 하는 작업 - 로드한다는 뜻!*/
	auto pRcsMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
	vector<PreloadCompleted> completed;
	pRcsMgr->PumpPreloads(completed);

	/*쓰레드에게 미리 넣어둔 요청 큐의 완료 상태 반환 받는 법! 퍼센트 가능*/
	_uint done = 0, total = 0;
	pRcsMgr->GetPreloadProgress(done, total);
}

HRESULT CLoadingLevel::Render()
{
	return S_OK;
}

void CLoadingLevel::PreLoadLevel()
{
	auto pRcsMgr = CGameInstance::GetInstance()->Get_ResourceMgr();

	/*쓰레드에게 미리 요청 큐를 넣어두는 작업*/
	//PreloadKey key{};
	//key.type = ResourceType::Texture;
	//key.levelKey = "Demo_Level";
	//key.resourceKey = "Bangboo_Sharkboo_NPC (merge).model";
	//key.options.isSRGB = true;
	//pRcsMgr->RequestPreload(key);

}

CLoadingLevel* CLoadingLevel::Create(const string& LevelKey)
{
	CLoadingLevel* instance = new CLoadingLevel(LevelKey);

	if (FAILED(instance->Initialize())) {
		MSG_BOX("Laoding level Create Failed");
		Safe_Release(instance);
	}

	return instance;
}

void CLoadingLevel::Free()
{
	__super::Free();

	m_pGameInstance->DestroyInstance();
}