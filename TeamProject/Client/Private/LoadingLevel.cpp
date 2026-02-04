#include "pch.h"
#include "LoadingLevel.h"
#include "GameInstance.h"
#include "Helper_Func.h"
#include "Zero_Level.h"

/* UI */
#include "UIDirector.h"

_bool CLoadingLevel::m_bGlobal = { false };

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
	PreLoadLevel(m_NextLevel);
	if (!m_bGlobal)
	{
		PreLoadLevel("Global_Level");
		PreLoadForTestLevel();
		m_bGlobal = true;
	}
	return S_OK;
}
void CLoadingLevel::Update()
{
	m_leastTime += GameInstance()->Get_EngineDeltaTime();

	/*쓰레드에게 미리 넣어둔 요청 큐를 펌핑 하는 작업 - 로드한다는 뜻!*/
	auto pRcsMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
	vector<PreloadCompleted> completed;
	pRcsMgr->PumpPreloads(completed);

	/*쓰레드에게 미리 넣어둔 요청 큐의 완료 상태 반환 받는 법! 퍼센트 가능*/
	_uint done = 0, total = 0;
	pRcsMgr->GetPreloadProgress(done, total);
	if (ResourceManager()->isLoadComplete() &&(m_leastTime>1.f))
	{
		LevelManager()->Notify_LoadComplete();
	}

}

HRESULT CLoadingLevel::Render()
{
	return S_OK;
}

void CLoadingLevel::PreLoadLevel(const string& levelKey)
{
	string LevelKey = levelKey;

	const std::string suffix = "_Level";
	if (LevelKey.size() >= suffix.size() &&
		LevelKey.compare(LevelKey.size() - suffix.size(), suffix.size(), suffix) == 0)
	{
		LevelKey.erase(LevelKey.size() - suffix.size());
	}

	const string clientPath = "../Bin/Resources/"+ LevelKey;
	filesystem::path directory = clientPath;

	error_code ec;
	if (!filesystem::exists(directory, ec) || ec) return;
	if (!filesystem::is_directory(directory, ec) || ec) return;

	auto* resourceManager = ResourceManager();
	if (!resourceManager) return;

	// (선택) 이전에 남아있던 큐가 있으면 비우기
	// for (auto& q : m_LoadQue) while (!q.empty()) q.pop();

	filesystem::recursive_directory_iterator iter(
		directory,
		filesystem::directory_options::skip_permission_denied,
		ec
	);
	filesystem::recursive_directory_iterator endIter;

	for (; iter != endIter; iter.increment(ec))
	{
		if (ec) { ec.clear(); continue; }

		const auto& entry = *iter;

		if (!entry.is_regular_file(ec) || ec) { ec.clear(); continue; }

		const filesystem::path filePathObj = entry.path();
		const string filePath = filePathObj.string();
		const string fileName = filePathObj.filename().string();

		resourceManager->Add_ResourcePath(fileName, filePath);

		const ResourceType type = CheckResourceType(filePath,fileName);
		if (type == ResourceType::None)
			continue;

		PreloadKey key{};
		key.type = type;
		key.levelKey = m_NextLevel;
		key.resourceKey = fileName;
		key.options.isSRGB = isSRGB(filePath);

		m_LoadQue[type].push(key);
	}

	// 큐에 쌓인 것들 전부 프리로드 요청
	for (auto& queuePerType : m_LoadQue)
	{
		while (!queuePerType.second.empty())
		{
			resourceManager->RequestPreload(queuePerType.second.front());
			queuePerType.second.pop();
		}
	}

}

void CLoadingLevel::PreLoadForTestLevel()
{
	const string clientPath = "../Bin/Resources";
	filesystem::path directory = clientPath;

	error_code ec;
	if (!filesystem::exists(directory, ec) || ec) return;
	if (!filesystem::is_directory(directory, ec) || ec) return;

	auto* resourceManager = ResourceManager();
	if (!resourceManager) return;

	// (선택) 이전에 남아있던 큐가 있으면 비우기
	// for (auto& q : m_LoadQue) while (!q.empty()) q.pop();

	filesystem::recursive_directory_iterator iter(
		directory,
		filesystem::directory_options::skip_permission_denied,
		ec
	);
	filesystem::recursive_directory_iterator endIter;

	for (; iter != endIter; iter.increment(ec))
	{
		if (ec) { ec.clear(); continue; }

		const auto& entry = *iter;

		if (!entry.is_regular_file(ec) || ec) { ec.clear(); continue; }

		const filesystem::path filePathObj = entry.path();
		const string filePath = filePathObj.string();
		const string fileName = filePathObj.filename().string();

		resourceManager->Add_ResourcePath(fileName, filePath);
	}

}


ResourceType CLoadingLevel::CheckResourceType(const string& filePath,const string& fileName)
{
	const string extRaw = filesystem::path(fileName).extension().string();

	if (extRaw.empty())
		return ResourceType::None; 

	string ext = extRaw;
	ext = Helper::ToLower(ext);

	if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".dds" || ext == ".bmp" )
		return ResourceType::Texture;

	if (ext == ".model")
		return ResourceType::Model;

	if (ext == ".mat")
		return ResourceType::Material;

	if(isEffect(filePath))
		return ResourceType::Effect;

	if(isAnim(filePath))
		return ResourceType::Animation;

	return	ResourceType::None;
}

_bool CLoadingLevel::isSRGB(const string& filePath)
{
	const _bool uiFolder = (filePath.find("UI") != string::npos);
	if (uiFolder)
		return true;
	//if (filePath.find("Effect"))
	//	return true;
	const _bool effectFolder = (filePath.find("Effect\\Texture\\Diffuse") != string::npos);
	if (effectFolder)
		return true;

	string fileName = filesystem::path(filePath).filename().string();

	const _bool isDiffuse = (fileName.find("_D") != string::npos);
	if(isDiffuse)
		return true;

	return false;
}

_bool CLoadingLevel::isEffect(const string& filePath)
{
	const _bool effFolder = (filePath.find("Effect") != string::npos);
	const string extRaw = filesystem::path(filePath).extension().string();
	_bool effJson = extRaw == ".json";

	return effFolder&&effJson;
}

_bool CLoadingLevel::isAnim(const string& filePath)
{
	string fileName = filesystem::path(filePath).filename().string();
	const _bool animFolder = (filePath.find("Meta.json") != string::npos);

	return animFolder ;
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