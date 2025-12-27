#include "Engine_Defines.h"
#include "ResourceThread.h"
#include "GameInstance.h"
#include "Helper_Func.h"

#include "VIBuffer.h"
#include "VI_Rect.h"
#include "VI_Cube.h"
#include "VI_Terrain.h"
#include "VI_Plane.h"
#include "VI_Point.h"
#include "VI_InstancePoint.h"

#include "Shader.h"
#include "Material.h"
#include "Texture.h"
#include "SoundData.h"
#include "MaterialData.h"
#include "ModelData.h"
#include "MaterialInstance.h"
#include "AnimationClip.h"
#include "AnimationLayout.h"
#include "ComputeShader.h"

CResourceThread::CResourceThread(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }, m_pContext{ pContext }, m_pInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pInstance);
}

HRESULT CResourceThread::Initiallize()
{
    m_pThreadPool = CThreadPool::Create(4);

	return S_OK;
}


HRESULT CResourceThread::Sync_To_Level()
{
    ILevelService* pLevelMgr = m_pInstance->Get_LevelMgr();

    if (!pLevelMgr) {
        MSG_BOX("There is No Level in Level Manager : CResourceMgr");
        return E_FAIL;
    }
    vector<string> LevelList = pLevelMgr->Get_LevelList();
    for (string& name : LevelList) {

        auto iteratorFound = m_LevelResources.find(name);
        if (iteratorFound == m_LevelResources.end())
        {
            Level_Resource newLevelResource;
            newLevelResource.resize(RESOURCE_TYPE_COUNT); // TEXTURE 인덱스 접근 가능하게
            auto inserted = m_LevelResources.emplace(name, move(newLevelResource));
            return S_OK;
        }

        // 혹시 과거 데이터라면 안전하게 보정
        if (iteratorFound->second.size() < RESOURCE_TYPE_COUNT)
            iteratorFound->second.resize(RESOURCE_TYPE_COUNT);

        return S_OK;
    }
	return S_OK;
}

CSoundData* CResourceThread::Load_Sound(const string& levelTag, const string& soundKey)
{
	return nullptr;
}

CVIBuffer* CResourceThread::Load_VIBuffer(const string& levelTag, const string& bufferKey, BUFFER_TYPE eType)
{
	return nullptr;
}

vector<CMaterialInstance*> CResourceThread::Load_MaterialFromFile(
	const string& levelTag,
	const string& fileKey)
{
	return {};

}

CShader* CResourceThread::Load_Shader(const string& levelTag, const string& shaderKey)
{
	return nullptr;
}

CTexture* CResourceThread::Load_Texture(const string& levelTag, const string& textureKey, _bool sRGBType)
{
    const string entryKey = textureKey;
    CResourceEntry* entry = nullptr;

    // 레벨 맵 찾기
    auto iteratorLevel = m_LevelResources.find(levelTag);
    if (iteratorLevel == m_LevelResources.end())
        return m_DefaultTexture;

    Level_Resource& textures = iteratorLevel->second;
    Level_Resource& textures = iteratorLevel->second;
    if (textures.size() < RESOURCE_TYPE_COUNT)
        textures.resize(RESOURCE_TYPE_COUNT);

    auto iteratorEntry = textures[ENUM(TEXTURE)].find(entryKey);
    if (iteratorEntry == textures[ENUM(TEXTURE)].end())
    {
        entry = CResourceEntry::Create();
        entry->SetDebugName(entryKey);
        entry->SetLevelTag(levelTag);

        auto pathIterator = m_PathByKey.find(textureKey);
        if (pathIterator != m_PathByKey.end())
            entry->SetSourcePath(pathIterator->second);

        textures[ENUM(TEXTURE)].emplace(entryKey, entry);
    }
    else
    {
        entry = iteratorEntry->second;
    }

    // 경로 없으면 default
    if (entry->GetSourcePathCopy().empty())
        return m_DefaultTexture;

    // Unloaded일 때만 비동기 요청
    if (entry->GetState() == LoadState::Unloaded)
    {
        ScheduleFunc scheduleFunc = [this](JobFunc jobFunc)
            {
                return Schedule(move(jobFunc));
            };

        ID3D11Device* device = m_pDevice;

        LoaderFunc loaderFunc = [sRGBType, device, entryKey](const string& sourcePath, string& errorMsg) -> ResourceVariant
            {
                errorMsg.clear();

                if (sourcePath.empty())
                {
                    errorMsg = "Texture sourcePath is empty.";
                    return monostate{};
                }

                const string extension = filesystem::path(sourcePath).extension().string();
                if (extension != ".dds" &&
                    extension != ".png" &&
                    extension != ".jpg" &&
                    extension != ".jpeg" &&
                    extension != ".tga" &&
                    extension != ".bmp")
                {
                    errorMsg = "Unsupported texture extension: " + extension + " (path: " + sourcePath + ")";
                    return monostate{};
                }

                std::error_code errorCode;
                if (!filesystem::exists(sourcePath, errorCode))
                {
                    errorMsg = "Texture file not found: " + sourcePath;
                    return monostate{};
                }

                const wstring widePath = filesystem::path(sourcePath).wstring();

                CTexture* createdTexture = CTexture::Create(device, widePath.c_str(), entryKey, sRGBType);
                if (createdTexture == nullptr)
                {
                    errorMsg = "CTexture::Create failed: " + sourcePath;
                    return monostate{};
                }

                return createdTexture;
            };

        entry->Begin_LoadAsync(loaderFunc, scheduleFunc);
    }

    // 메인 스레드에서 완료된 것만 확정
    entry->Pump_CompletedOnly();

    if (entry->GetState() == LoadState::Ready)
    {
        CTexture* texture = entry->Get_NoRef<CTexture>();
        return texture ? texture : m_DefaultTexture;
    }

    return m_DefaultTexture;
}


shared_future<ResourceVariant> CResourceThread::Schedule(JobFunc jobFunc)
{
    future<ResourceVariant> futureValue =
        m_pThreadPool->enqueue([jobFunc = move(jobFunc)]() mutable -> ResourceVariant
            {
                return jobFunc();
            });

    return futureValue.share();
}

vector<CAnimationClip*> CResourceThread::Load_MetaClip(const string& levelTag, const string& MetaClipKey)
{
	return {};
}

EFFECT_ASSET CResourceThread::Load_EffectAsset(const string& levelTag, const string& effectTag)
{
	return {};
}

CComputeShader* CResourceThread::Load_ComputeShader(const string& levelTag, const string& shaderKey)
{
	return nullptr;
}

CModelData* CResourceThread::Load_ModelData(const string& levelTag, const string& ModelKey)
{
	return nullptr;
}

string CResourceThread::Get_ResourcePath(const string& resourceKey)
{
	return string();
}

HRESULT CResourceThread::Add_ResourcePath(const string& resourceKey, const string& resourcePath)
{
	return S_OK;
}

void CResourceThread::Pump_AllEntries_MainThread()
{
    for (auto& levelPair : m_LevelResources)
    {
        Level_Resource& levelResource = levelPair.second;

        if (levelResource.size() < RESOURCE_TYPE_COUNT)
            levelResource.resize(RESOURCE_TYPE_COUNT);

        for (size_t resourceTypeIndex = 0; resourceTypeIndex < RESOURCE_TYPE_COUNT; ++resourceTypeIndex)
        {
            auto& entryMap = levelResource[resourceTypeIndex];

            for (auto& entryPair : entryMap)
            {
                CResourceEntry* entry = entryPair.second;
                if (!entry)
                    continue;

                entry->Pump_CompletedOnly();
            }
        }
    }

    auto iteratorPending = m_PendingDestroyEntries.begin();
    while (iteratorPending != m_PendingDestroyEntries.end())
    {
        CResourceEntry* entry = *iteratorPending;
        if (!entry)
        {
            iteratorPending = m_PendingDestroyEntries.erase(iteratorPending);
            continue;
        }

        entry->Pump_CompletedOnly();

        if (entry->GetState() == LoadState::Loading)
        {
            ++iteratorPending;
            continue;
        }

        Safe_Release(entry);
        iteratorPending = m_PendingDestroyEntries.erase(iteratorPending);
    }
}


void CResourceThread::Load_InitialResource()
{
    m_DefaultTexture = CTexture::Create(m_pDevice, L"../../DefaultSource/Default.dds", "Default.dds", false);
}

string CResourceThread::MakePath(const string& pathKey)
{
	return "";
}


void CResourceThread::Clear_Resource(const string& levelTag)
{
    auto iteratorLevel = m_LevelResources.find(levelTag);
    if (iteratorLevel == m_LevelResources.end())
        return;

    Level_Resource& levelResource = iteratorLevel->second;

    if (levelResource.size() < RESOURCE_TYPE_COUNT)
        levelResource.resize(RESOURCE_TYPE_COUNT);

    for (size_t resourceTypeIndex = 0; resourceTypeIndex < RESOURCE_TYPE_COUNT; ++resourceTypeIndex)
    {
        auto& entryMap = levelResource[resourceTypeIndex];

        for (auto& entryPair : entryMap)
        {
            CResourceEntry* entry = entryPair.second;
            if (!entry) continue;

            entry->Pump_CompletedOnly();

            if (entry->GetState() == LoadState::Loading)
            {
                m_PendingDestroyEntries.push_back(entry);
            }
            else
            {
                Safe_Release(entry);
            }
        }

        entryMap.clear();
    }

    m_LevelResources.erase(iteratorLevel);
}

CResourceThread* CResourceThread::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CResourceThread* instance = new CResourceThread(pDevice, pContext);
	if (FAILED(instance->Initiallize())) {
		Safe_Release(instance);
	}
	return instance;
}

void CResourceThread::Free()
{
    Safe_Release(m_pThreadPool);
	__super::Free();
}
