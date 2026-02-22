#include "pch.h"
#include "LogoLevel.h"
#include "GameInstance.h"
#include "LevelMgr.h"

/*Video*/
#include "UI_LogoVideo.h"

/* UI */
#include "UIDirector.h"

CLogoLevel::CLogoLevel(const string& LevelKey)
	:CLevel(LevelKey),
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CLogoLevel::Initialize()
{
	return S_OK;
}

HRESULT CLogoLevel::Awake()
{
	//==================== UI ===============
	auto uiDirector = CUIDirector::GetInstance();
	uiDirector->Load_LevelObjects("Logo_Level");
	m_NextLevel = "Global_Level";
	PreLoadLeveleff("Global_Level");
	PreLoadLevel("Global_Level");
	Create_LogoVideo();
	return S_OK;
}

void CLogoLevel::Update()
{
	
	auto pRcsMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
	vector<PreloadCompleted> completed;
	pRcsMgr->PumpPreloads(completed);
	_uint done = 0, total = 0;
	ResourceManager()->GetPreloadProgress(done, total);
	if (InputDevice()->Key_Tap('Y')&& ResourceManager()->isLoadComplete()) {
		LevelManager()->Request_ChangeLevel("Scott_Level", true);
	}
}

HRESULT CLogoLevel::Render()
{
	return S_OK;
}

HRESULT CLogoLevel::Create_LogoVideo()
{
	PrototypeManager()->Add_ProtoType("Logo_Level", "Proto_GameObject_LogoVideo", CUI_LogoVideo::Create());
	auto pObj = Builder::Create_UIObject({ "Logo_Level", "Proto_GameObject_LogoVideo" })
		.Build("video");
	UIManager()->Add_UIObject(pObj, "Logo_Level");
	return S_OK;
}

void CLogoLevel::PreLoadLeveleff(const string& levelKey)
{
	string LevelKey = levelKey;

	const std::string suffix = "_Level";
	if (LevelKey.size() >= suffix.size() &&
		LevelKey.compare(LevelKey.size() - suffix.size(), suffix.size(), suffix) == 0)
	{
		LevelKey.erase(LevelKey.size() - suffix.size());
	}

	const string clientPath = "../Bin/Resources/Global/Effect";
	filesystem::path directory = clientPath;

	error_code ec;
	if (!filesystem::exists(directory, ec) || ec) return;
	if (!filesystem::is_directory(directory, ec) || ec) return;

	auto* resourceManager = ResourceManager();
	if (!resourceManager) return;

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

		const ResourceType type = CheckResourceType(filePath, fileName);
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


void CLogoLevel::PreLoadLevel(const string& levelKey)
{
	string LevelKey = levelKey;

	const std::string suffix = "_Level";
	if (LevelKey.size() >= suffix.size() &&
		LevelKey.compare(LevelKey.size() - suffix.size(), suffix.size(), suffix) == 0)
	{
		LevelKey.erase(LevelKey.size() - suffix.size());
	}

	const string clientPath = "../Bin/Resources/Global";
	filesystem::path directory = clientPath;

	error_code ec;
	if (!filesystem::exists(directory, ec) || ec) return;
	if (!filesystem::is_directory(directory, ec) || ec) return;

	auto* resourceManager = ResourceManager();
	if (!resourceManager) return;

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

		const ResourceType type = CheckResourceType(filePath, fileName);
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

ResourceType CLogoLevel::CheckResourceType(const string& filePath, const string& fileName)
{
	const string extRaw = filesystem::path(fileName).extension().string();

	if (extRaw.empty())
		return ResourceType::None;

	string ext = extRaw;
	ext = Helper::ToLower(ext);

	if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".dds" || ext == ".bmp")
		return ResourceType::Texture;

	if (ext == ".model")
		return ResourceType::Model;

	if (ext == ".mat")
		return ResourceType::Material;

	if (ext == ".wav")
		return ResourceType::Sound;

	if (isEffect(filePath))
		return ResourceType::Effect;

	if (isAnim(filePath))
		return ResourceType::Animation;

	return	ResourceType::None;
}

_bool CLogoLevel::isSRGB(const string& filePath)
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
	if (isDiffuse)
		return true;

	return false;
}

_bool CLogoLevel::isEffect(const string& filePath)
{
	const _bool effFolder = (filePath.find("Effect") != string::npos);
	const string extRaw = filesystem::path(filePath).extension().string();
	_bool effJson = extRaw == ".json";

	return effFolder && effJson;
}

_bool CLogoLevel::isAnim(const string& filePath)
{
	string fileName = filesystem::path(filePath).filename().string();
	const _bool animFolder = (filePath.find("Meta.json") != string::npos);

	return animFolder;
}

CLogoLevel* CLogoLevel::Create(const string& LevelKey)
{
	CLogoLevel* instance = new CLogoLevel(LevelKey);
	if (FAILED(instance->Initialize())) {
		MSG_BOX("Logo level Create Failed");
		Safe_Release(instance);
	}

	return instance;
}

void CLogoLevel::Free()
{
	__super::Free();

	m_pGameInstance->DestroyInstance();
}