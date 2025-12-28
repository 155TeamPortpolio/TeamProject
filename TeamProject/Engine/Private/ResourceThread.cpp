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
    auto& globalResources = m_LevelResources[G_GlobalLevelKey];

    // TEXTURE/VI_BUFFER 등 인덱싱 가능하게 크기 보장
    if (globalResources.size() < RESOURCE_TYPE_COUNT)
        globalResources.resize(RESOURCE_TYPE_COUNT);

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
            continue;
        }

        // 혹시 과거 데이터라면 안전하게 보정
        if (iteratorFound->second.size() < RESOURCE_TYPE_COUNT)
            iteratorFound->second.resize(RESOURCE_TYPE_COUNT);
    }

	return S_OK;
}

CSoundData* CResourceThread::Load_Sound(const string& levelTag, const string& soundKey)
{
	return nullptr;
}

CVIBuffer* CResourceThread::Load_VIBuffer(const string& levelTag, const string& bufferKey, BUFFER_TYPE bufferType)
{
    CResourceEntry* entry = nullptr;

    auto iteratorLevel = m_LevelResources.find(levelTag);
    if (iteratorLevel == m_LevelResources.end())
        return nullptr;

    Level_Resource& buffersByType = iteratorLevel->second;
    if (buffersByType.size() < RESOURCE_TYPE_COUNT)
        buffersByType.resize(RESOURCE_TYPE_COUNT);

    auto& entryByKey = buffersByType[ENUM(VI_BUFFER)];

    auto iteratorEntry = entryByKey.find(bufferKey);
    if (iteratorEntry == entryByKey.end())
    {
        entry = CResourceEntry::Create();
        entry->SetDebugName(bufferKey);
        entry->SetLevelTag(levelTag);

        // TERRAIN이면 path 필요. 나머지는 굳이 없어도 됨.
        auto iteratorPath = m_PathByKey.find(bufferKey);
        if (iteratorPath != m_PathByKey.end())
            entry->SetSourcePath(iteratorPath->second);

        entryByKey.emplace(bufferKey, entry);
    }
    else
    {
        entry = iteratorEntry->second;
    }

    entry->Pump_CompletedOnly();
    if (entry->GetState() == LoadState::Ready)
    {
        CVIBuffer* loadedBuffer = entry->Get_NoRef<CVIBuffer>();
        return loadedBuffer;
    }

    if (entry->GetState() == LoadState::Unloaded)
    {
        ScheduleFunc scheduleFunc = [this](JobFunc jobFunc)
            {
                return Schedule(std::move(jobFunc));
            };

        ID3D11Device* device = m_pDevice;
        const BUFFER_TYPE capturedType = bufferType;
        const string capturedKey = bufferKey;

        LoaderFunc loaderFunc = [device, capturedType, capturedKey](const string& sourcePath, string& errorMsg) -> ResourceVariant
            {
                errorMsg.clear();

                CVIBuffer* createdBuffer = nullptr;

                switch (capturedType)
                {
                case BUFFER_TYPE::BASIC_RECT:
                    createdBuffer = CVI_Rect::Create(device, capturedKey);
                    break;

                case BUFFER_TYPE::BASIC_PLANE:
                    createdBuffer = CVI_Plane::Create(device, capturedKey);
                    break;

                case BUFFER_TYPE::BASIC_POINT:
                    createdBuffer = CVI_Point::Create(device, capturedKey);
                    break;

                case BUFFER_TYPE::BASIC_INSTANCE_POINT:
                    createdBuffer = CVI_InstancePoint::Create(device, capturedKey);
                    break;

                case BUFFER_TYPE::TERRAIN:
                {
                    if (sourcePath.empty())
                    {
                        errorMsg = "Terrain sourcePath is empty. (key: " + capturedKey + ")";
                        return std::monostate{};
                    }
                    createdBuffer = CVI_Terrain::Create(device, capturedKey, sourcePath);
                }
                break;

                case BUFFER_TYPE::BASIC_CUBE:
                    break;
                case BUFFER_TYPE::BASIC_SPHERE:
                    break;

                default:
                    errorMsg = "Unsupported BUFFER_TYPE in Load_VIBuffer. (key: " + capturedKey + ")";
                    return std::monostate{};
                }

                if (createdBuffer == nullptr)
                {
                    errorMsg = "VIBuffer Create failed. (key: " + capturedKey + ")";
                    return std::monostate{};
                }

                return createdBuffer;
            };

        entry->Begin_LoadAsync(loaderFunc, scheduleFunc);
    }

    entry->Pump_CompletedOnly();
    if (entry->GetState() == LoadState::Ready)
    {
        CVIBuffer* loadedBuffer = entry->Get_NoRef<CVIBuffer>();
        return loadedBuffer;
    }

    return nullptr;
}

CVIBuffer* CResourceThread::Load_WaitVIBuffer(const string& levelTag, const string& bufferKey, BUFFER_TYPE eType)
{
    CResourceEntry* entry = nullptr;

    auto iteratorLevel = m_LevelResources.find(levelTag);
    if (iteratorLevel == m_LevelResources.end())
        return nullptr;

    Level_Resource& buffersByType = iteratorLevel->second;
    if (buffersByType.size() < RESOURCE_TYPE_COUNT)
        buffersByType.resize(RESOURCE_TYPE_COUNT);

    auto& entryByKey = buffersByType[ENUM(VI_BUFFER)];

    auto iteratorEntry = entryByKey.find(bufferKey);
    if (iteratorEntry == entryByKey.end())
    {
        entry = CResourceEntry::Create();
        entry->SetDebugName(bufferKey);
        entry->SetLevelTag(levelTag);

        // TERRAIN이면 path 필요. 나머지는 굳이 없어도 됨.
        auto iteratorPath = m_PathByKey.find(bufferKey);
        if (iteratorPath != m_PathByKey.end())
            entry->SetSourcePath(iteratorPath->second);

        entryByKey.emplace(bufferKey, entry);
    }
    else
    {
        entry = iteratorEntry->second;
    }

    entry->Pump_CompletedOnly();
    if (entry->GetState() == LoadState::Ready)
    {
        CVIBuffer* loadedBuffer = entry->Get_NoRef<CVIBuffer>();
        return loadedBuffer;
    }

    if (entry->GetState() == LoadState::Unloaded)
    {
        ScheduleFunc scheduleFunc = [this](JobFunc jobFunc)
            {
                return Schedule(std::move(jobFunc));
            };

        ID3D11Device* device = m_pDevice;
        const BUFFER_TYPE capturedType = eType;
        const string capturedKey = bufferKey;

        LoaderFunc loaderFunc = [device, capturedType, capturedKey](const string& sourcePath, string& errorMsg) -> ResourceVariant
            {
                errorMsg.clear();

                CVIBuffer* createdBuffer = nullptr;

                switch (capturedType)
                {
                case BUFFER_TYPE::BASIC_RECT:
                    createdBuffer = CVI_Rect::Create(device, capturedKey);
                    break;

                case BUFFER_TYPE::BASIC_PLANE:
                    createdBuffer = CVI_Plane::Create(device, capturedKey);
                    break;

                case BUFFER_TYPE::BASIC_POINT:
                    createdBuffer = CVI_Point::Create(device, capturedKey);
                    break;

                case BUFFER_TYPE::BASIC_INSTANCE_POINT:
                    createdBuffer = CVI_InstancePoint::Create(device, capturedKey);
                    break;

                case BUFFER_TYPE::TERRAIN:
                {
                    if (sourcePath.empty())
                    {
                        errorMsg = "Terrain sourcePath is empty. (key: " + capturedKey + ")";
                        return std::monostate{};
                    }
                    createdBuffer = CVI_Terrain::Create(device, capturedKey, sourcePath);
                }
                break;

                case BUFFER_TYPE::BASIC_CUBE:
                    break;
                case BUFFER_TYPE::BASIC_SPHERE:
                    break;

                default:
                    errorMsg = "Unsupported BUFFER_TYPE in Load_VIBuffer. (key: " + capturedKey + ")";
                    return std::monostate{};
                }

                if (createdBuffer == nullptr)
                {
                    errorMsg = "VIBuffer Create failed. (key: " + capturedKey + ")";
                    return std::monostate{};
                }

                return createdBuffer;
            };

        entry->Begin_LoadAsync(loaderFunc, scheduleFunc);
    }
    entry->Wait_AsyncDone();
    entry->Pump_CompletedOnly();
    if (entry->GetState() == LoadState::Ready)
    {
        CVIBuffer* loadedBuffer = entry->Get_NoRef<CVIBuffer>();
        return loadedBuffer;
    }

    return nullptr;
}

vector<CMaterialInstance*> CResourceThread::Load_MaterialFromFile(
    const string& levelTag,
    const string& fileKey)
{
    vector<CMaterialInstance*> handles;

    vector<CMaterialData*>* dataList = GetOrLoad_MaterialData(levelTag, fileKey);

    if (!dataList)
        dataList = GetOrLoad_MaterialData(G_GlobalLevelKey, "Default.mat",true);

    if (!dataList)
    {
        MSG_BOX("Default.mat is missing. Resource init problem.");
        return handles;
    }

    return Make_MaterialHandles(*dataList);
}

_bool CResourceThread::IsMaterialReady(const string& levelTag, const string& fileKey)
{
    auto levelIterator = m_LevelResources.find(levelTag);
    if (levelIterator == m_LevelResources.end())
        return false;

    Level_Resource& levelResource = levelIterator->second;
    if (levelResource.size() < RESOURCE_TYPE_COUNT)
        return false;

    auto& materialEntryMap = levelResource[ENUM(MATERIALS)];
    auto entryIterator = materialEntryMap.find(fileKey);
    if (entryIterator == materialEntryMap.end())
        return false;

    CResourceEntry* entry = entryIterator->second;
    if (!entry)
        return false;

    entry->Pump_CompletedOnly();

    return (entry->GetState() == LoadState::Ready);
}

vector<CMaterialInstance*> CResourceThread::Make_MaterialHandles(
    const vector<CMaterialData*>& dataList)
{
    vector<CMaterialInstance*> handles;
    handles.reserve(dataList.size());

    for (CMaterialData* data : dataList)
    {
        CMaterialInstance* handle = CMaterialInstance::Make_Handle(data, m_pDevice);
        handles.push_back(handle);
    }
    return handles;
}

vector<CMaterialData*>* CResourceThread::GetOrLoad_MaterialData(const string& levelTag, const string& fileKey, _bool wait)
{
    const string entryKey = fileKey;
    CResourceEntry* entry = nullptr;

    auto levelIterator = m_LevelResources.find(levelTag);
    if (levelIterator == m_LevelResources.end())
        return nullptr;

    Level_Resource& levelResource = levelIterator->second;
    if (levelResource.size() < RESOURCE_TYPE_COUNT)
        levelResource.resize(RESOURCE_TYPE_COUNT);

    auto& materialEntryMap = levelResource[ENUM(MATERIALS)];
    auto entryIterator = materialEntryMap.find(entryKey);

    if (entryIterator == materialEntryMap.end())
    {
        entry = CResourceEntry::Create();
        entry->SetDebugName(entryKey);
        entry->SetLevelTag(levelTag);

        auto pathIterator = m_PathByKey.find(fileKey);
        if (pathIterator != m_PathByKey.end())
            entry->SetSourcePath(pathIterator->second);

        materialEntryMap.emplace(entryKey, entry);
    }
    else
    {
        entry = entryIterator->second;
    }

    if (entry->GetSourcePathCopy().empty())
        return nullptr;

    if (entry->GetState() == LoadState::Unloaded)
    {
        ScheduleFunc scheduleFunc = [this](JobFunc jobFunc)
            {
                return Schedule(std::move(jobFunc));
            };

        LoaderFunc loaderFunc = [levelTag](const string& sourcePath, string& errorMsg) -> ResourceVariant
            {
                errorMsg.clear();

                if (sourcePath.empty())
                {
                    errorMsg = "MaterialData sourcePath is empty.";
                    return monostate{};
                }

                std::error_code errorCode;
                if (!filesystem::exists(sourcePath, errorCode))
                {
                    errorMsg = "MaterialData file not found: " + sourcePath;
                    return monostate{};
                }

                ifstream inputStream(sourcePath, ios::binary);
                if (!inputStream.is_open())
                {
                    errorMsg = "Failed to open material file: " + sourcePath;
                    return monostate{};
                }

                MATERIAL_FILE_HEADER header{};
                inputStream.read(reinterpret_cast<char*>(&header), sizeof(header));
                if (!inputStream.good())
                {
                    errorMsg = "Failed to read MATERIAL_FILE_HEADER: " + sourcePath;
                    return monostate{};
                }

                CMaterialDataList* createdList = new CMaterialDataList();
                createdList->list.reserve(header.MaterialDataCount);
                const string baseDirectory = filesystem::path(sourcePath).parent_path().string() + "/";

                for (size_t materialIndex = 0; materialIndex < header.MaterialDataCount; ++materialIndex)
                {
                    CMaterialData* materialData = CMaterialData::Create(levelTag, inputStream, baseDirectory);
                    if (!materialData)
                    {
                        for (CMaterialData* createdData : createdList->list)
                            Safe_Release(createdData);

                        Safe_Release(createdList);

                        errorMsg = "CMaterialData::Create failed. path: " + sourcePath;
                        return monostate{};
                    }

                    createdList->list.push_back(materialData);
                }

                if (createdList->list.empty())
                {
                    Safe_Release(createdList);
                    errorMsg = "No materials loaded: " + sourcePath;
                    return monostate{};
                }

                return createdList; // 엔트리가 소유(Release는 엔트리의 Variant 해제 로직으로)
            };

        entry->Begin_LoadAsync(loaderFunc, scheduleFunc);
    }
    if(wait)
        entry->Wait_AsyncDone();

    entry->Pump_CompletedOnly();

    if (entry->GetState() == LoadState::Ready)
    {
        CMaterialDataList* loadedList = entry->Get_NoRef<CMaterialDataList>();
        return loadedList ? &loadedList->list : nullptr;
    }

    return nullptr;
}

CShader* CResourceThread::Load_Shader(const string& levelTag, const string& shaderKey)
{
    const string entryKey = shaderKey;
    CResourceEntry* entry = nullptr;
    auto iteratorLevel = m_LevelResources.find(levelTag);
    if (iteratorLevel == m_LevelResources.end())
        return nullptr;

    Level_Resource& resources = iteratorLevel->second;
    if (resources.size() < RESOURCE_TYPE_COUNT)
        resources.resize(RESOURCE_TYPE_COUNT);

    auto& shaderMap = resources[ENUM(SHADER)];
    auto iteratorEntry = shaderMap.find(entryKey);

    if (iteratorEntry == shaderMap.end())
    {
        entry = CResourceEntry::Create();
        entry->SetDebugName(entryKey);
        entry->SetLevelTag(levelTag);

        auto pathIterator = m_PathByKey.find(shaderKey);
        if (pathIterator != m_PathByKey.end())
            entry->SetSourcePath(pathIterator->second);

        shaderMap.emplace(entryKey, entry);
    }
    else
    {
        entry = iteratorEntry->second;
    }

    if (entry->GetSourcePathCopy().empty())
        return nullptr;

    if (entry->GetState() == LoadState::Unloaded)
    {
        ScheduleFunc scheduleFunc = [this](JobFunc jobFunc)
            {
                return Schedule(move(jobFunc));
            };

        ID3D11Device* device = m_pDevice;

        LoaderFunc loaderFunc = [device, entryKey](const string& sourcePath, string& errorMsg) -> ResourceVariant
            {
                errorMsg.clear();

                if (sourcePath.empty())
                {
                    errorMsg = "Shader sourcePath is empty.";
                    return monostate{};
                }

                error_code errorCode;
                if (!filesystem::exists(sourcePath, errorCode))
                {
                    errorMsg = "Shader file not found: " + sourcePath;
                    return monostate{};
                }

                CShader* createdShader = CShader::Create(device, sourcePath, entryKey);
                if (!createdShader)
                {
                    errorMsg = "CShader::Create failed: " + sourcePath;
                    return monostate{};
                }

                return createdShader;
            };

        entry->Begin_LoadAsync(loaderFunc, scheduleFunc);
    }

    // 셰이더는 여기서 "완료까지 대기 + 확정"
    entry->Wait_AndPump();

    if (entry->GetState() == LoadState::Ready)
        return entry->Get_NoRef<CShader>();

    return nullptr;
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

CTexture* CResourceThread::Load_Texture(const string& levelTag, const string& textureKey, _bool sRGBType)
{
    const string entryKey = textureKey;
    CResourceEntry* entry = nullptr;

    auto iteratorLevel = m_LevelResources.find(levelTag);
    if (iteratorLevel == m_LevelResources.end())
        return m_DefaultTexture;

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

    if (entry->GetSourcePathCopy().empty())
        return m_DefaultTexture;

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

                error_code errorCode;
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

CModelData* CResourceThread::Load_ModelData(const string& levelTag, const string& ModelKey)
{
    const string entryKey = ModelKey;
    CResourceEntry* entry = nullptr;

    auto iteratorLevel = m_LevelResources.find(levelTag);
    if (iteratorLevel == m_LevelResources.end())
        return m_DefaultModel;

    Level_Resource& ModelDatas = iteratorLevel->second;
    if (ModelDatas.size() < RESOURCE_TYPE_COUNT)
        ModelDatas.resize(RESOURCE_TYPE_COUNT);

    auto iteratorEntry = ModelDatas[ENUM(MODELDATA)].find(entryKey);
    if (iteratorEntry == ModelDatas[ENUM(MODELDATA)].end())
    {
        entry = CResourceEntry::Create();
        entry->SetDebugName(entryKey);
        entry->SetLevelTag(levelTag);

        auto pathIterator = m_PathByKey.find(ModelKey);
        if (pathIterator != m_PathByKey.end())
            entry->SetSourcePath(pathIterator->second);

        ModelDatas[ENUM(MODELDATA)].emplace(entryKey, entry);
    }
    else
    {
        entry = iteratorEntry->second;
    }

    if (entry->GetSourcePathCopy().empty())
        return m_DefaultModel;

    if (entry->GetState() == LoadState::Unloaded)
    {
        ScheduleFunc scheduleFunc = [this](JobFunc jobFunc)
            {
                return Schedule(move(jobFunc));
            };

        ID3D11Device* device = m_pDevice;

        LoaderFunc loaderFunc = [device, entryKey](const string& sourcePath, string& errorMsg) -> ResourceVariant
            {
                errorMsg.clear();
                if (sourcePath.empty())
                {
                    errorMsg = "ModelDAta sourcePath is empty.";
                    return monostate{};
                }

                const string extension = filesystem::path(sourcePath).extension().string();
                if (extension != ".model" )
                {
                    errorMsg = "Unsupported ModelData extension: " + extension + " (path: " + sourcePath + ")";
                    return monostate{};
                }

                std::error_code errorCode;
                if (!filesystem::exists(sourcePath, errorCode))
                {
                    errorMsg = "ModelData file not found: " + sourcePath;
                    return monostate{};
                }

                const wstring widePath = filesystem::path(sourcePath).wstring();

                CModelData* createdModel = CModelData::Create(sourcePath,device);
                if (createdModel == nullptr)
                {
                    errorMsg = "CModelData::Create failed: " + sourcePath;
                    return monostate{};
                }

                return createdModel;
            };

        entry->Begin_LoadAsync(loaderFunc, scheduleFunc);
    }

    entry->Pump_CompletedOnly();

    if (entry->GetState() == LoadState::Ready)
    {
        CModelData* model = entry->Get_NoRef<CModelData>();
        return model ? model : m_DefaultModel;
    }

    return m_DefaultModel;
}

string CResourceThread::Get_ResourcePath(const string& resourceKey)
{
    auto iter = m_PathByKey.find(resourceKey);
    if (iter != m_PathByKey.end()) return iter->second;
    else return string();
}

HRESULT CResourceThread::Add_ResourcePath(const string& resourceKey, const string& resourcePath)
{
    auto iter = m_PathByKey.find(resourceKey);

    if (iter != m_PathByKey.end()) {
        string msg = "directory Exist: " + resourceKey + "\n";
        OutputDebugStringA(msg.c_str());
        return E_FAIL;
    }

    m_PathByKey.emplace(resourceKey, resourcePath);
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

    /* Default Shader */
    Add_ResourcePath("VTX_TexPos.hlsl", "../Bin/ShaderFiles/VTX_TexPos.hlsl");
    Add_ResourcePath("VTX_Mesh.hlsl", "../Bin/ShaderFiles/VTX_Mesh.hlsl");
    Add_ResourcePath("VTX_NorTex.hlsl", "../Bin/ShaderFiles/VTX_NorTex.hlsl");
    Add_ResourcePath("VTX_SkinMesh.hlsl", "../Bin/ShaderFiles/VTX_SkinMesh.hlsl");
    Add_ResourcePath("VTX_UI.hlsl", "../Bin/ShaderFiles/VTX_UI.hlsl");
    Add_ResourcePath("VTX_Debug.hlsl", "../Bin/ShaderFiles/VTX_Debug.hlsl");
    Add_ResourcePath("VTX_Point.hlsl", "../Bin/ShaderFiles/VTX_Point.hlsl");
    Add_ResourcePath("VTX_InstancePoint.hlsl", "../Bin/ShaderFiles/VTX_InstancePoint.hlsl");
    Add_ResourcePath("VTX_EffectMesh.hlsl", "../Bin/ShaderFiles/VTX_EffectMesh.hlsl");
    Add_ResourcePath("Shader_Deferred.hlsl", "../Bin/ShaderFiles/Shader_Deferred.hlsl");
    Add_ResourcePath("Shader_PostProcess.hlsl", "../Bin/ShaderFiles/Shader_PostProcess.hlsl");
    Add_ResourcePath("Shader_WeightOIT.hlsl", "../Bin/ShaderFiles/Shader_WeightOIT.hlsl");
    Add_ResourcePath("Default.mat", "../../DefaultSource/Default.mat"); 

  Load_Shader(G_GlobalLevelKey,"VTX_TexPos.hlsl");
  Load_Shader(G_GlobalLevelKey,"VTX_Mesh.hlsl");
  Load_Shader(G_GlobalLevelKey,"VTX_NorTex.hlsl");
  Load_Shader(G_GlobalLevelKey,"VTX_SkinMesh.hlsl");
  Load_Shader(G_GlobalLevelKey,"VTX_UI.hlsl");
  Load_Shader(G_GlobalLevelKey,"VTX_Debug.hlsl");
  Load_Shader(G_GlobalLevelKey,"VTX_Point.hlsl");
  Load_Shader(G_GlobalLevelKey,"VTX_EffectMesh.hlsl");
  Load_Shader(G_GlobalLevelKey,"Shader_Deferred.hlsl");
  Load_Shader(G_GlobalLevelKey,"Shader_PostProcess.hlsl");
  Load_Shader(G_GlobalLevelKey,"Shader_WeightOIT.hlsl");


    m_DefaultTexture = CTexture::Create(m_pDevice, L"../../DefaultSource/Default.dds", "Default.dds", false);
    m_DefaultModel = CModelData::Create("../../DefaultSource/Default.model", m_pDevice);
    Load_WaitVIBuffer(G_GlobalLevelKey, "Engine_Default_Rect", BUFFER_TYPE::BASIC_RECT);
    Load_WaitVIBuffer(G_GlobalLevelKey, "Engine_Default_Plane", BUFFER_TYPE::BASIC_PLANE);
    Load_WaitVIBuffer(G_GlobalLevelKey, "Engine_Default_Point", BUFFER_TYPE::BASIC_POINT);
    Load_WaitVIBuffer(G_GlobalLevelKey, "Engine_Default_InstancePoint", BUFFER_TYPE::BASIC_INSTANCE_POINT);

    {
        vector<CMaterialData*>* defaultData =
            GetOrLoad_MaterialData(G_GlobalLevelKey, "Default.mat",true);
    }

}

CResourceEntry* CResourceThread::Request_TextureEntry(const string& levelTag, const string& textureKey, _bool sRGBType)
{
    const string entryKey = textureKey;
    auto iteratorLevel = m_LevelResources.find(levelTag);
    if (iteratorLevel == m_LevelResources.end())
        return nullptr;

    Level_Resource& levelResource = iteratorLevel->second;
    if (levelResource.size() < RESOURCE_TYPE_COUNT)
        levelResource.resize(RESOURCE_TYPE_COUNT);

    auto& textureMap = levelResource[ENUM(TEXTURE)];

    CResourceEntry* entry = nullptr;
    auto iteratorEntry = textureMap.find(entryKey);

    if (iteratorEntry == textureMap.end() || iteratorEntry->second == nullptr)
    {
        if (iteratorEntry != textureMap.end() && iteratorEntry->second == nullptr)
            textureMap.erase(iteratorEntry);

        entry = CResourceEntry::Create();
        entry->SetFallback(ResourceVariant{ m_DefaultTexture }); 
        if (!entry) return nullptr;

        entry->SetDebugName(entryKey);
        entry->SetLevelTag(levelTag);

        auto pathIterator = m_PathByKey.find(textureKey);
        if (pathIterator != m_PathByKey.end())
            entry->SetSourcePath(pathIterator->second);

        textureMap.emplace(entryKey, entry);
    }
    else
    {
        entry = iteratorEntry->second;
    }

    if (!entry)
        return nullptr;

    // 소스패스 없으면 디폴트
    if (entry->GetSourcePathCopy().empty())
        return nullptr;

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
                error_code errorCode;
                if (!filesystem::exists(sourcePath, errorCode))
                {
                    errorMsg = "Texture file not found: " + sourcePath;
                    return monostate{};
                }

                const wstring widePath = filesystem::path(sourcePath).wstring();
                CTexture* createdTexture = CTexture::Create(device, widePath.c_str(), entryKey, sRGBType);
                if (!createdTexture)
                {
                    errorMsg = "CTexture::Create failed: " + sourcePath;
                    return monostate{};
                }

                return createdTexture;
            };

        entry->Begin_LoadAsync(loaderFunc, scheduleFunc);
    }

    return entry;
}

CResourceEntry* CResourceThread::Request_ModelEntry(const string& levelTag, const string& modelKey)
{
    const string entryKey = modelKey;
    auto iteratorLevel = m_LevelResources.find(levelTag);
    if (iteratorLevel == m_LevelResources.end())
        return nullptr;

    Level_Resource& levelResource = iteratorLevel->second;
    if (levelResource.size() < RESOURCE_TYPE_COUNT)
        levelResource.resize(RESOURCE_TYPE_COUNT);

    auto& modelMap = levelResource[ENUM(MODELDATA)];

    CResourceEntry* entry = nullptr;
    auto iteratorEntry = modelMap.find(entryKey);

    if (iteratorEntry == modelMap.end() || iteratorEntry->second == nullptr)
    {
        if (iteratorEntry != modelMap.end() && iteratorEntry->second == nullptr)
            modelMap.erase(iteratorEntry);

        entry = CResourceEntry::Create();
        entry->SetFallback(ResourceVariant{ m_DefaultModel });
        if (!entry) return nullptr;

        entry->SetDebugName(entryKey);
        entry->SetLevelTag(levelTag);

        auto pathIterator = m_PathByKey.find(modelKey);
        if (pathIterator != m_PathByKey.end())
            entry->SetSourcePath(pathIterator->second);

        modelMap.emplace(entryKey, entry);
    }
    else
    {
        entry = iteratorEntry->second;
    }

    if (!entry)
        return nullptr;

    // 소스패스 없으면 디폴트
    if (entry->GetSourcePathCopy().empty())
        return nullptr;

    if (entry->GetState() == LoadState::Unloaded)
    {
        ScheduleFunc scheduleFunc = [this](JobFunc jobFunc)
            {
                return Schedule(move(jobFunc));
            };

        ID3D11Device* device = m_pDevice;

        LoaderFunc loaderFunc = [device, entryKey](const string& sourcePath, string& errorMsg) -> ResourceVariant
            {
                errorMsg.clear();
                error_code errorCode;
                if (!filesystem::exists(sourcePath, errorCode))
                {
                    errorMsg = "Model file not found: " + sourcePath;
                    return monostate{};
                }

                CModelData* createdModel = CModelData::Create(sourcePath,device);
                if (!createdModel)
                {
                    errorMsg = "CModelData::Create failed: " + sourcePath;
                    return monostate{};
                }

                return createdModel;
            };

        entry->Begin_LoadAsync(loaderFunc, scheduleFunc);
    }

    return entry;
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
    vector<string> levelKeys;
    levelKeys.reserve(m_LevelResources.size());
    for (const auto& levelPair : m_LevelResources)
        levelKeys.push_back(levelPair.first);

    // 2) 레벨 리소스 정리
    for (const auto& levelKey : levelKeys)
        Clear_Resource(levelKey);

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pInstance);

    /*디폴트 삭제*/
    Safe_Release(m_DefaultTexture);
    Safe_Release(m_DefaultModel);

    Safe_Release(m_pThreadPool);
    __super::Free();
}
