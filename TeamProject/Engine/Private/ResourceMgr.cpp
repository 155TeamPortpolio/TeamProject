#include "Engine_Defines.h"
#include "ResourceMgr.h"
#include "GameInstance.h"
#include "ILevelService.h"
#include "IAudioService.h"
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
#include "ThreadPool.h"
#include "PreloadScheduler.h"

CResourceMgr::CResourceMgr(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }, m_pContext{ pContext }, m_pInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pInstance);
}

HRESULT CResourceMgr::Initiallize()
{
	m_pPreloader = CPreloadScheduler::Create(CThreadPool::Create());
	Init_PreLoader();
	return S_OK;
}

void CResourceMgr::Clear_Resource(const string& levelTag)
{
	int index = ValidLevel(levelTag);
	if (index == -1) {
		MSG_BOX("Wrong Level Tag. :Clear_Resource ");
		return;
	}

	RS_Pool& pool = m_Resources[index];

	unordered_map<string, CSoundData*> sounds;
	unordered_map<string, CVIBuffer*> buffers;
	unordered_map<string, CTexture*> textures;
	unordered_map<string, CShader*> shaders;
	unordered_map<string, CModelData*> models;
	unordered_map<string, vector<CMaterialData*>> materials;
	unordered_map<string, ANIMATION_META> metas;
	unordered_map<string, EFFECT_ASSET> effects;
	unordered_map<string, CComputeShader*> computeShaders;

	{
		lock_guard<mutex> lockGuard(pool.soundMutex);
		sounds.swap(pool.m_Sounds);
	}
	{
		lock_guard<mutex> lockGuard(pool.bufferMutex);
		buffers.swap(pool.m_Buffers);
	}
	{
		lock_guard<mutex> lockGuard(pool.textureMutex);
		textures.swap(pool.m_Textures);
	}
	{
		lock_guard<mutex> lockGuard(pool.shaderMutex);
		shaders.swap(pool.m_Shaders);
	}
	{
		lock_guard<mutex> lockGuard(pool.modelMutex);
		models.swap(pool.m_ModelDatas);
	}
	{
		lock_guard<mutex> lockGuard(pool.materialMutex);
		materials.swap(pool.m_MaterialInstances);
	}
	{
		lock_guard<mutex> lockGuard(pool.animMutex);
		metas.swap(pool.m_AnimationMetas);
	}
	{
		lock_guard<mutex> lockGuard(pool.effectMutex);
		effects.swap(pool.m_EffectAssets);
	}
	{
		lock_guard<mutex> lockGuard(pool.computeMutex);
		computeShaders.swap(pool.m_ComputeShaders);
	}

	for (auto& pair : sounds) Safe_Release(pair.second);
	for (auto& pair : buffers) Safe_Release(pair.second);
	for (auto& pair : textures) Safe_Release(pair.second);
	for (auto& pair : shaders) Safe_Release(pair.second);
	for (auto& pair : models) Safe_Release(pair.second);

	for (auto& pair : materials)
		for (auto* materialData : pair.second)
			Safe_Release(materialData);

	for (auto& pair : metas)
		for (auto* clipData : pair.second.pClips)
			Safe_Release(clipData);

	for (auto& pair : effects)
		for (auto* node : pair.second.Nodes)
			Safe_Delete(node);

	for (auto& pair : computeShaders) 
		Safe_Release(pair.second);
}

HRESULT CResourceMgr::Sync_To_Level()
{
	ILevelService* levelMgr = m_pInstance->Get_LevelMgr();
	if (!levelMgr) return E_FAIL;

	vector<string> levelList = levelMgr->Get_LevelList();

	if (m_Resources.empty())
		m_Resources.resize(1);

	m_LevelIndex.clear();
	m_LevelIndex.emplace(G_GlobalLevelKey, 0);

	m_Resources.resize(1 + levelList.size());

	for (_uint i = 0; i < levelList.size(); ++i)
		m_LevelIndex.emplace(levelList[i], 1 + i);

	return S_OK;
}



CSoundData* CResourceMgr::Load_Sound(const string& levelTag, const string& soundKey)
{
	int index = ValidLevel(levelTag);
	if (index == -1) {
		MSG_BOX("Wrong Level Tag. :Load_Sound ");
		return nullptr;
	}

	auto& map = m_Resources[index].m_Sounds;
	auto iter = map.find(soundKey);

	if (iter != map.end()) return iter->second;

	CSoundData* pData = CSoundData::Create(MakePath(soundKey), soundKey);
	map.emplace(soundKey, pData);

	return pData;
}

CVIBuffer* CResourceMgr::Load_VIBuffer(const string& levelTag, const string& bufferKey, BUFFER_TYPE eType)
{
	int index = ValidLevel(levelTag);
	if (index == -1) {
		MSG_BOX("Wrong Level Tag. :Load_VIBuffer ");
		return nullptr;
	}
	auto& map = m_Resources[index].m_Buffers;
	auto iter = map.find(bufferKey);
	if (iter != map.end()) return iter->second;

	CVIBuffer* buffer = nullptr;
	switch (eType)
	{
	case Engine::BUFFER_TYPE::BASIC_RECT:
		buffer = CVI_Rect::Create(m_pDevice, bufferKey);
		break;
	case Engine::BUFFER_TYPE::BASIC_CUBE:
		break;
	case Engine::BUFFER_TYPE::BASIC_SPHERE:
		break;
	case Engine::BUFFER_TYPE::BASIC_PLANE:
		buffer = CVI_Plane::Create(m_pDevice, bufferKey);
		break;
	case Engine::BUFFER_TYPE::TERRAIN:
		buffer = CVI_Terrain::Create(m_pDevice, bufferKey, MakePath(bufferKey));
		break;
	case Engine::BUFFER_TYPE::BASIC_POINT:
		buffer = CVI_Point::Create(m_pDevice, bufferKey);
		break;
	case Engine::BUFFER_TYPE::BASIC_INSTANCE_POINT:
		buffer = CVI_InstancePoint::Create(m_pDevice, bufferKey);
		break;
	default:
		break;
	}

	if (buffer)
		map.emplace(bufferKey, buffer);

	return buffer;
}

vector<CMaterialInstance*> CResourceMgr::Load_MaterialFromFile(
	const string& levelTag,
	const string& fileKey)
{
	vector<CMaterialInstance*> handles;

	vector<CMaterialData*>* dataList = GetOrLoad_MaterialData(levelTag, fileKey);

	if (!dataList)
		dataList = GetOrLoad_MaterialData(levelTag, "Default.mat");

	if (!dataList)
	{
		MSG_BOX("Default.mat is missing. Resource init problem.");
		return handles;
	}

	return Make_MaterialHandles(*dataList);
}

vector<CMaterialInstance*> CResourceMgr::Make_MaterialHandles(
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

void CResourceMgr::Init_PreLoader()
{
	m_pPreloader->BindLoader(ResourceType::Texture,
		[this](const PreloadKey& key, string& errorMessage) -> bool
		{
			CTexture* texture = this->Load_Texture(key.levelKey, key.resourceKey, key.options.isSRGB);
			if (!texture)
			{
				errorMessage = "Load_Texture failed: " + key.resourceKey;
				return false;
			}
			return true;
		});

	m_pPreloader->BindLoader(ResourceType::Sound,
		[this](const PreloadKey& key, string& errorMessage) -> bool
		{
			CSoundData* sound = this->Load_Sound(key.levelKey, key.resourceKey);
			if (!sound)
			{
				errorMessage = "Load_Sound failed: " + key.resourceKey;
				return false;
			}
			return true;
		});

	m_pPreloader->BindLoader(ResourceType::Shader,
		[this](const PreloadKey& key, string& errorMessage) -> bool
		{
			CShader* shader = this->Load_Shader(key.levelKey, key.resourceKey);
			if (!shader)
			{
				errorMessage = "Load_Shader failed: " + key.resourceKey;
				return false;
			}
			return true;
		});

	m_pPreloader->BindLoader(ResourceType::Model,
		[this](const PreloadKey& key, string& errorMessage) -> bool
		{
			CModelData* model = this->Load_ModelData(key.levelKey, key.resourceKey);
			if (!model)
			{
				errorMessage = "Load_Model failed: " + key.resourceKey;
				return false;
			}
			return true;
		});

	m_pPreloader->BindLoader(ResourceType::Material,
		[this](const PreloadKey& key, string& errorMessage) -> bool
		{
			vector<CMaterialData*>* material = this->GetOrLoad_MaterialData(key.levelKey, key.resourceKey);
			if (!material)
			{
				errorMessage = "Load_Material failed: " + key.resourceKey;
				return false;
			}
			return true;
		});


	m_pPreloader->BindLoader(ResourceType::ComputeShader,
		[this](const PreloadKey& key, string& errorMessage) -> bool
		{
			CComputeShader* pComShader = this->Load_ComputeShader(key.levelKey, key.resourceKey);
			if (!pComShader)
			{
				errorMessage = "Load_ComputeShader failed: " + key.resourceKey;
				return false;
			}
			return true;
		});

	m_pPreloader->BindLoader(ResourceType::Animation,
		[this](const PreloadKey& key, string& errorMessage) -> bool
		{
			ANIMATION_META Meta = this->Load_MetaClip(key.levelKey, key.resourceKey);
			if (Meta.pClips.empty())
			{
				errorMessage = "Load_Animation failed: " + key.resourceKey;
				return false;
			}
			return true;
		});

	m_pPreloader->BindLoader(ResourceType::Effect,
		[this](const PreloadKey& key, string& errorMessage) -> bool
		{
			EFFECT_ASSET AssetFile = this->Load_EffectAsset(key.levelKey, key.resourceKey);
			if (AssetFile.iNodeCount == 0)
			{
				errorMessage = "Load_EffectAsse failed: " + key.resourceKey;
				return false;
			}
			return true;
		});
}

vector<CMaterialData*>* CResourceMgr::GetOrLoad_MaterialData(
	const string& levelTag,
	const string& fileKey)
{
	const int levelIndex = ValidLevel(levelTag);
	if (levelIndex == -1)
		return nullptr;

	RS_Pool& pool = m_Resources[levelIndex];
	auto& materialMap = pool.m_MaterialInstances;

	{
		lock_guard<mutex> lockGuard(pool.materialMutex);
		auto it = materialMap.find(fileKey);
		if (it != materialMap.end())
			return &it->second;
	}

	const string materialPath = MakePath(fileKey);
	if (materialPath.empty())
		return nullptr;

	ifstream ifs(materialPath, ios::binary);
	if (!ifs.is_open())
		return nullptr;

	MATERIAL_FILE_HEADER header{};
	ifs.read(reinterpret_cast<char*>(&header), sizeof(header));
	if (!ifs.good())
		return nullptr;

	vector<CMaterialData*> container;
	container.reserve(header.MaterialDataCount);

	const string baseFilePath = MakePath(header.materialFileKey);
	const string directory = filesystem::path(baseFilePath).parent_path().string() + "/";

	for (size_t i = 0; i < header.MaterialDataCount; ++i)
	{
		CMaterialData* data = CMaterialData::Create(levelTag, ifs, directory);
		if (!data)
		{
			for (auto* created : container)
				Safe_Release(created);
			return nullptr;
		}
		container.push_back(data);
	}

	{
		lock_guard<mutex> lockGuard(pool.materialMutex);

		auto it = materialMap.find(fileKey);
		if (it != materialMap.end())
		{
			for (auto* created : container)
				Safe_Release(created);
			return &it->second;
		}

		auto [insertIt, inserted] = materialMap.emplace(fileKey, std::move(container));
		return &insertIt->second;
	}
}


CShader* CResourceMgr::Load_Shader(const string& levelTag, const string& shaderKey)
{
	int index = ValidLevel(levelTag);
	if (index == -1) {
		MSG_BOX("Wrong Level Tag. :Load_Shader ");
		return nullptr;
	}

	RS_Pool& pool = m_Resources[index];

	{
		lock_guard<mutex> lockGuard(m_Resources[index].shaderMutex);
		auto it = pool.m_Shaders.find(shaderKey);
		if (it != pool.m_Shaders.end())
			return it->second;
	}

	if (MakePath(shaderKey).empty()) {
		MSG_BOX("There is No Key : Load_Shader");
	}

	CShader* pData = CShader::Create(m_pDevice, MakePath(shaderKey), shaderKey);
	if (!pData)
		return nullptr;

	{
		lock_guard<mutex> lockGuard(pool.shaderMutex);

		auto it = pool.m_Shaders.find(shaderKey);
		if (it != pool.m_Shaders.end())
		{
			Safe_Release(pData);
			return it->second;
		}

		pool.m_Shaders.emplace(shaderKey, pData);
		return pData;
	}
}

CTexture* CResourceMgr::Load_Texture(const string& levelTag, const string& textureKey, _bool sRGBType)
{
	int index = ValidLevel(levelTag);
	if (index == -1) {
		MSG_BOX("Wrong Level Tag. :Load_Texture ");
		return nullptr;
	}
	RS_Pool& pool = m_Resources[index];

	{
		lock_guard<mutex> lockGuard(m_Resources[index].textureMutex);
		auto it = pool.m_Textures.find(textureKey);
		if (it != pool.m_Textures.end())
			return it->second;
	}

	wstring path = Helper::ConvertToWideString(MakePath(textureKey));
	CTexture* pData = CTexture::Create(m_pDevice, path, textureKey, sRGBType);

	if (!pData)
		return nullptr;

	{
		lock_guard<mutex> lockGuard(pool.textureMutex);

		auto it = pool.m_Textures.find(textureKey);
		if (it != pool.m_Textures.end())
		{
			Safe_Release(pData);
			return it->second;
		}

		pool.m_Textures.emplace(textureKey, pData);
		return pData;
	}
}

ANIMATION_META CResourceMgr::Load_MetaClip(const string& levelTag, const string& MetaKey)
{
	int index = ValidLevel(levelTag);
	if (index == -1) {
		MSG_BOX("Wrong Level Tag. : Load_AnimClip");
		return ANIMATION_META();
	}

	RS_Pool& pool = m_Resources[index];
	{
		lock_guard<mutex> lockGuard(m_Resources[index].animMutex);
		auto it = pool.m_AnimationMetas.find(MetaKey);
		if (it != pool.m_AnimationMetas.end())
			return it->second;
	}

	const string metaPath = Get_ResourcePath(MetaKey);
	const string animDir = filesystem::path(metaPath).parent_path().string();

	ANIM_META MetaData = Helper::LoadJson<ANIM_META>(metaPath);

	ANIMATION_META Meta;
	Meta.PreTransform = MetaData.PreTransform;
	Meta.pClips.reserve(MetaData.Clips.size());

	for (auto& DataClip : MetaData.Clips) {

		string animPath = animDir + "\\Anim\\" + DataClip.ClipTag + ".anim";
		CAnimationClip* pClip = CAnimationClip::Create(animPath);

		if (!DataClip.Events.empty())
			pClip->Set_Events(DataClip.Events);

		if (pClip)
			Meta.pClips.push_back(pClip);
	}
	if (Meta.pClips.empty()) return {};
	{
		lock_guard<mutex> lockGuard(pool.animMutex);

		auto it = pool.m_AnimationMetas.find(MetaKey);
		if (it != pool.m_AnimationMetas.end())
		{
			for (auto pData : Meta.pClips)
			{
				Safe_Release(pData);
			}
			return it->second;
		}

		pool.m_AnimationMetas.emplace(MetaKey, Meta);
		return Meta;
	}
}

EFFECT_ASSET CResourceMgr::Load_EffectAsset(const string& levelTag, const string& effectTag)
{
	using namespace nlohmann;
	namespace fs = std::filesystem;

	int index = ValidLevel(levelTag);
	if (index == -1) {
		MSG_BOX("Wrong Level Tag. : Load_EffectAsset");
		return EFFECT_ASSET();
	}

	RS_Pool& pool = m_Resources[index];

	{
		lock_guard<mutex> lockGuard(m_Resources[index].effectMutex);
		auto it = pool.m_EffectAssets.find(effectTag);
		if (it != pool.m_EffectAssets.end())
			return it->second;
	}

	string filePath = MakePath(effectTag);
	ifstream file(filePath);

	ordered_json EffectData = json::parse(file);
	EFFECT_ASSET Effect = EFFECT_ASSET::FromJson(EffectData);

	for (_uint i = 0; i < Effect.iNodeCount; ++i)
	{
		EFFECT_TYPE type = static_cast<EFFECT_TYPE>(EffectData["nodes"][i].at("effect_type"));

		switch (type)
		{
		case Engine::EFFECT_TYPE::SPRITE:
		{

		}break;
		case Engine::EFFECT_TYPE::PARTICLE:
		{
			PARTICLE_NODE* pParticleNode = new PARTICLE_NODE();
			*pParticleNode = PARTICLE_NODE::FromJson(EffectData["nodes"][i]);

			Effect.Nodes.push_back(pParticleNode);
		}break;
		case Engine::EFFECT_TYPE::MESH:
		{
			MESH_NODE* pMeshNode = new MESH_NODE();
			*pMeshNode = MESH_NODE::FromJson(EffectData["nodes"][i]);

			Effect.Nodes.push_back(pMeshNode);
		}break;
		case Engine::EFFECT_TYPE::TRAIL:
		{
			TRAIL_NODE* pTrailNode = new TRAIL_NODE();
			*pTrailNode = TRAIL_NODE::FromJson(EffectData["nodes"][i]);

			Effect.Nodes.push_back(pTrailNode);
		}break;
		default:
			break;
		}
	}

	{
		lock_guard<mutex> lockGuard(pool.effectMutex);

		auto it = pool.m_EffectAssets.find(effectTag);
		if (it != pool.m_EffectAssets.end())
		{
			return it->second;
		}

		pool.m_EffectAssets.emplace(effectTag, Effect);
		return Effect;
	}
}

CComputeShader* CResourceMgr::Load_ComputeShader(const string& levelTag, const string& shaderKey)
{
	int index = ValidLevel(levelTag);
	if (index == -1) {
		MSG_BOX("Wrong Level Tag. : Load_ComputeShader ");
		return nullptr;
	}

	RS_Pool& pool = m_Resources[index];

	{
		lock_guard<mutex> lockGuard(m_Resources[index].computeMutex);
		auto it = pool.m_ComputeShaders.find(shaderKey);
		if (it != pool.m_ComputeShaders.end())
			return it->second;
	}

	if (MakePath(shaderKey).empty()) {
		MSG_BOX("There is No Key : Load_ComputeShader");
	}

	CComputeShader* pData = CComputeShader::Create(m_pDevice, MakePath(shaderKey), shaderKey);
	if (!pData)
		return nullptr;

	{
		lock_guard<mutex> lockGuard(pool.computeMutex);
		auto it = pool.m_ComputeShaders.find(shaderKey);
		if (it != pool.m_ComputeShaders.end())
		{
			Safe_Release(pData);
			return it->second;
		}
		pool.m_ComputeShaders.emplace(shaderKey, pData);

		return pData;
	}
}

CModelData* CResourceMgr::Load_ModelData(const string& levelTag, const string& ModelKey)
{
	int index = ValidLevel(levelTag);
	if (index == -1) {
		MSG_BOX("Wrong Level Tag. : Load_ModelData ");
		return nullptr;
	}

	RS_Pool& pool = m_Resources[index];

	{
		lock_guard<mutex> lockGuard(m_Resources[index].modelMutex);
		auto it = pool.m_ModelDatas.find(ModelKey);
		if (it != pool.m_ModelDatas.end())
			return it->second;
	}

	CModelData* pData = CModelData::Create(MakePath(ModelKey), m_pDevice);

	{
		lock_guard<mutex> lockGuard(pool.modelMutex);
		auto it = pool.m_ModelDatas.find(ModelKey);
		if (it != pool.m_ModelDatas.end())
		{
			Safe_Release(pData);
			return it->second;
		}

		if (!pData)
			pData = m_Resources[0].m_ModelDatas["Default.model"];
		else
			pool.m_ModelDatas.emplace(ModelKey, pData);
		return pData;
	}
}


string CResourceMgr::Get_ResourcePath(const string& resourceKey)
{
	lock_guard<std::mutex> lockGuard(m_keyPathMutex);

	auto it = m_KeyPath.find(resourceKey);
	if (it == m_KeyPath.end())
		return string{};
	return it->second;
}

HRESULT CResourceMgr::Add_ResourcePath(const string& resourceKey, const string& resourcePath)
{
	lock_guard<mutex> lockGuard(m_keyPathMutex);
	auto iter = m_KeyPath.find(resourceKey);

	if (iter != m_KeyPath.end()) {
		string msg = "directory Exist: " + resourceKey + "\n";
		OutputDebugStringA(msg.c_str());
		return E_FAIL;
	}

	m_KeyPath.emplace(resourceKey, resourcePath);
	return S_OK;
}

_int CResourceMgr::ValidLevel(const string& levelKey)
{
	auto iter = m_LevelIndex.find(levelKey);

	if (iter == m_LevelIndex.end())
		return -1;
	return iter->second;
}

void CResourceMgr::Load_InitialResource()
{
	m_LevelIndex.emplace(G_GlobalLevelKey, 0);
	m_Resources.resize(1);

	/* Default Shader */
	Add_ResourcePath("VTX_TexPos.hlsl", "../Bin/ShaderFiles/VTX_TexPos.hlsl");
	Add_ResourcePath("VTX_Mesh.hlsl", "../Bin/ShaderFiles/VTX_Mesh.hlsl");
	Add_ResourcePath("VTX_NorTex.hlsl", "../Bin/ShaderFiles/VTX_NorTex.hlsl");
	Add_ResourcePath("VTX_SkinMesh.hlsl", "../Bin/ShaderFiles/VTX_SkinMesh.hlsl");
	Add_ResourcePath("VTX_UI.hlsl", "../Bin/ShaderFiles/VTX_UI.hlsl");
	Add_ResourcePath("VTX_Debug.hlsl", "../Bin/ShaderFiles/VTX_Debug.hlsl");
	Add_ResourcePath("VTX_Cloud.hlsl", "../Bin/ShaderFiles/VTX_Cloud.hlsl");
	Add_ResourcePath("VTX_Point.hlsl", "../Bin/ShaderFiles/VTX_Point.hlsl");
	Add_ResourcePath("VTX_InstancePoint.hlsl", "../Bin/ShaderFiles/VTX_InstancePoint.hlsl");
	Add_ResourcePath("VTX_EffectMesh.hlsl", "../Bin/ShaderFiles/VTX_EffectMesh.hlsl");
	Add_ResourcePath("VTX_Trail.hlsl", "../Bin/ShaderFiles/VTX_Trail.hlsl");
	Add_ResourcePath("Shader_Deferred.hlsl", "../Bin/ShaderFiles/Shader_Deferred.hlsl");
	Add_ResourcePath("Shader_PostProcess.hlsl", "../Bin/ShaderFiles/Shader_PostProcess.hlsl");
	Add_ResourcePath("Shader_Deferred_SkinnedMesh.hlsl", "../Bin/ShaderFiles/Shader_Deferred_SkinnedMesh.hlsl");
	Add_ResourcePath("Shader_Deferred_StaticMesh.hlsl", "../Bin/ShaderFiles/Shader_Deferred_StaticMesh.hlsl");
	Add_ResourcePath("Shader_Deferred_Effect.hlsl", "../Bin/ShaderFiles/Shader_Deferred_Effect.hlsl");

	/* Compute Shader */
	Add_ResourcePath("CS_Particle_Spawn.hlsl", "../Bin/ShaderFiles/CS_Particle_Spawn.hlsl");
	Add_ResourcePath("CS_Particle_Basic.hlsl", "../Bin/ShaderFiles/CS_Particle_Basic.hlsl");
	Add_ResourcePath("CS_Particle_DeadListInit.hlsl", "../Bin/ShaderFiles/CS_Particle_DeadListInit.hlsl");
	Add_ResourcePath("CS_Particle_BuildInstance.hlsl", "../Bin/ShaderFiles/CS_Particle_BuildInstance.hlsl");
	Add_ResourcePath("CS_HiZ_Copy.hlsl","../Bin/ShaderFiles/CS_HiZ_Copy.hlsl");
	Add_ResourcePath("CS_HiZ_Reduce.hlsl","../Bin/ShaderFiles/CS_HiZ_Reduce.hlsl");
	Add_ResourcePath("CS_OcclusionCull.hlsl","../Bin/ShaderFiles/CS_OcclusionCull.hlsl");

	//Default
	Add_ResourcePath("Default.dds", "../../DefaultSource/Default.dds");
	Add_ResourcePath("Default.mat", "../../DefaultSource/Default.mat");
	Add_ResourcePath("Default.model", "../../DefaultSource/Default.model");

	m_Resources[0].m_Buffers.emplace("Engine_Default_Rect", CVI_Rect::Create(m_pDevice, "Engine_Default_Rect"));
	m_Resources[0].m_Buffers.emplace("Engine_Default_Plane", CVI_Plane::Create(m_pDevice, "Engine_Default_Plane"));
	m_Resources[0].m_Buffers.emplace("Engine_Default_Point", CVI_Point::Create(m_pDevice, "Engine_Default_Point"));
	m_Resources[0].m_Buffers.emplace("Engine_Default_InstancePoint", CVI_InstancePoint::Create(m_pDevice, "Engine_Default_InstancePoint"));

	m_Resources[0].m_Shaders.emplace("VTX_TexPos.hlsl", CShader::Create(m_pDevice, "../Bin/ShaderFiles/VTX_TexPos.hlsl", "VTX_TexPos.hlsl"));
	m_Resources[0].m_Shaders.emplace("VTX_Mesh.hlsl", CShader::Create(m_pDevice, "../Bin/ShaderFiles/VTX_Mesh.hlsl", "VTX_Mesh.hlsl"));
	m_Resources[0].m_Shaders.emplace("VTX_NorTex.hlsl", CShader::Create(m_pDevice, "../Bin/ShaderFiles/VTX_NorTex.hlsl", "VTX_NorTex.hlsl"));
	m_Resources[0].m_Shaders.emplace("VTX_SkinMesh.hlsl", CShader::Create(m_pDevice, "../Bin/ShaderFiles/VTX_SkinMesh.hlsl", "VTX_SkinMesh.hlsl"));
	m_Resources[0].m_Shaders.emplace("VTX_UI.hlsl", CShader::Create(m_pDevice, "../Bin/ShaderFiles/VTX_UI.hlsl", "VTX_UI.hlsl"));
	m_Resources[0].m_Shaders.emplace("VTX_Cloud.hlsl", CShader::Create(m_pDevice, "../Bin/ShaderFiles/VTX_Cloud.hlsl", "VTX_Cloud.hlsl"));
	m_Resources[0].m_Shaders.emplace("VTX_Debug.hlsl", CShader::Create(m_pDevice, "../Bin/ShaderFiles/VTX_Debug.hlsl", "VTX_Debug.hlsl"));
	m_Resources[0].m_Shaders.emplace("VTX_Point.hlsl", CShader::Create(m_pDevice, "../Bin/ShaderFiles/VTX_Point.hlsl", "VTX_Point.hlsl"));
	m_Resources[0].m_Shaders.emplace("VTX_EffectMesh.hlsl", CShader::Create(m_pDevice, "../Bin/ShaderFiles/VTX_EffectMesh.hlsl", "VTX_EffectMesh.hlsl"));
	m_Resources[0].m_Shaders.emplace("VTX_Trail.hlsl", CShader::Create(m_pDevice, "../Bin/ShaderFiles/VTX_Trail.hlsl", "VTX_Trail.hlsl"));
	m_Resources[0].m_Shaders.emplace("VTX_UIMesh.hlsl", CShader::Create(m_pDevice, "../Bin/ShaderFiles/VTX_UIMesh.hlsl", "VTX_UIMesh.hlsl"));
	m_Resources[0].m_Shaders.emplace("Shader_Deferred.hlsl", CShader::Create(m_pDevice, "../Bin/ShaderFiles/Shader_Deferred.hlsl", "Shader_Deferred.hlsl"));
	m_Resources[0].m_Shaders.emplace("Shader_PostProcess.hlsl", CShader::Create(m_pDevice, "../Bin/ShaderFiles/Shader_PostProcess.hlsl", "Shader_PostProcess.hlsl"));
	m_Resources[0].m_Shaders.emplace("Shader_Deferred_Effect.hlsl", CShader::Create(m_pDevice, "../Bin/ShaderFiles/Shader_Deferred_Effect.hlsl", "Shader_Deferred_Effect.hlsl"));

	m_Resources[0].m_ComputeShaders.emplace("CS_Particle_Spawn.hlsl", CComputeShader::Create(m_pDevice, "../Bin/ShaderFiles/CS_Particle_Spawn.hlsl", "CS_Particle_Spawn.hlsl"));
	m_Resources[0].m_ComputeShaders.emplace("CS_Particle_Basic.hlsl", CComputeShader::Create(m_pDevice, "../Bin/ShaderFiles/CS_Particle_Basic.hlsl", "CS_Particle_Basic.hlsl"));
	m_Resources[0].m_ComputeShaders.emplace("CS_Particle_DeadListInit.hlsl", CComputeShader::Create(m_pDevice, "../Bin/ShaderFiles/CS_Particle_DeadListInit.hlsl", "CS_Particle_DeadListInit.hlsl"));
	m_Resources[0].m_ComputeShaders.emplace("CS_Particle_BuildInstance.hlsl", CComputeShader::Create(m_pDevice, "../Bin/ShaderFiles/CS_Particle_BuildInstance.hlsl", "CS_Particle_BuildInstance.hlsl"));
	m_Resources[0].m_ComputeShaders.emplace("CS_HiZ_Copy.hlsl", CComputeShader::Create(m_pDevice, "../Bin/ShaderFiles/CS_HiZ_Copy.hlsl", "CS_HiZ_Copy.hlsl"));
	m_Resources[0].m_ComputeShaders.emplace("CS_HiZ_Reduce.hlsl", CComputeShader::Create(m_pDevice, "../Bin/ShaderFiles/CS_HiZ_Reduce.hlsl", "CS_HiZ_Reduce.hlsl"));
	m_Resources[0].m_ComputeShaders.emplace("CS_OcclusionCull.hlsl", CComputeShader::Create(m_pDevice, "../Bin/ShaderFiles/CS_OcclusionCull.hlsl", "CS_OcclusionCull.hlsl"));

	/*Default*/
	m_Resources[0].m_ModelDatas.emplace("Default.model", CModelData::Create("../../DefaultSource/Default.model", m_pDevice));
	m_Resources[0].m_Textures.emplace("Default.dds", CTexture::Create(m_pDevice, L"../../DefaultSource/Default.dds", "Default.dds", false));
	{
		vector<CMaterialData*>* defaultData =
			GetOrLoad_MaterialData(G_GlobalLevelKey, "Default.mat");

		if (!defaultData)
			MSG_BOX("Failed to preload Default.mat");
	}
}

_bool CResourceMgr::RequestPreload(const PreloadKey& key)
{
	return m_pPreloader->Request(key);
}

void CResourceMgr::PumpPreloads(vector<PreloadCompleted>& outCompleted)
{
	m_pPreloader->Pump(outCompleted);
}

void CResourceMgr::GetPreloadProgress(_uint& outDone, _uint& outTotal) const
{
	m_pPreloader->GetProgress(outDone, outTotal);
}

_bool CResourceMgr::isLoadComplete() const
{
	return m_pPreloader->IsAllDone();
}

string CResourceMgr::MakePath(const string& pathKey)
{
	lock_guard<mutex> lockGuard(m_keyPathMutex);

	auto it = m_KeyPath.find(pathKey);
	if (it != m_KeyPath.end())
		return it->second;

	string msg = "Cant Find Path: " + pathKey + "\n";
	OutputDebugStringA(msg.c_str());
	return {};
}



CResourceMgr* CResourceMgr::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CResourceMgr* instance = new CResourceMgr(pDevice, pContext);
	if (FAILED(instance->Initiallize())) {
		Safe_Release(instance);
	}
	return instance;
}

void CResourceMgr::Free()
{
	__super::Free();

	for (auto& pair : m_LevelIndex)
		Clear_Resource(pair.first);

	m_LevelIndex.clear();
	m_Resources.clear();

	Safe_Release(m_pInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pPreloader);
}
