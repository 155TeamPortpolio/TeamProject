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

#include "Shader.h"
#include "Material.h"
#include "Texture.h"
#include "SoundData.h"
#include "MaterialData.h"
#include "ModelData.h"
#include "MaterialInstance.h"
#include "AnimationClip.h"

CResourceMgr::CResourceMgr(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }, m_pContext{ pContext }, m_pInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pInstance);
}

HRESULT CResourceMgr::Initiallize()
{
	Load_InitialResource();
	return S_OK;
}


void CResourceMgr::Clear_Resource(const string& levelTag)
{
	int index = ValidLevel(levelTag);
	if (index == -1) {
		MSG_BOX("Wrong Level Tag. :Clear_Resource ");
		return;
	}

	for (auto& pair : m_Resources[index].m_Sounds)
		Safe_Release(pair.second);

	for (auto& pair : m_Resources[index].m_Buffers)
		Safe_Release(pair.second);

	for (auto& pair : m_Resources[index].m_Textures)
		Safe_Release(pair.second);

	for (auto& pair : m_Resources[index].m_Shaders)
		Safe_Release(pair.second);

	for (auto& pair : m_Resources[index].m_ModelDatas)
		Safe_Release(pair.second);

	for (auto& pair : m_Resources[index].m_MaterialInstances)
		for (auto& materialData : pair.second)
			Safe_Release(materialData);

	for (auto& pair : m_Resources[index].m_Animations)
		Safe_Release(pair.second);

	m_Resources[index] = {};
}

HRESULT CResourceMgr::Sync_To_Level()
{
	ILevelService* pLevelMgr = m_pInstance->Get_LevelMgr();

	if (!pLevelMgr) {
		MSG_BOX("There is No Level in Level Manager : CResourceMgr");
		return E_FAIL;
	}

	vector<string> LevelList = pLevelMgr->Get_LevelList();
	int iCount = 0;
	for (string& name : LevelList) {
		//m_ResourcePool.emplace(name, ResourcePool());
		auto iter = m_LevelIndex.emplace(name, iCount);
		if (iter.second == true)
			m_Resources.push_back(RS_Pool());

		iCount++;
	}
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
	default:
		break;
	}

	if (buffer)
		map.emplace(bufferKey, buffer);

	return buffer;
}

vector<CMaterialInstance*> CResourceMgr::Load_MaterialFromFile(const string& levelTag, const string& fileKey)
{
	vector<CMaterialInstance*> MaterialHandles;

	int index = ValidLevel(levelTag);
	if (index == -1) {
		MSG_BOX("Wrong Level Tag. :Load_MaterialFromFile ");
		return MaterialHandles;
	}
	/*일단 레벨에서 꺼내봐*/

	/*머티리얼 데이터 벡터를 가진 맵*/
	auto& map = m_Resources[index].m_MaterialInstances;

	auto iter = map.find(fileKey);

	//그런 키가 없다면
	if (iter == map.end()) {
		ifstream ifs;
		ifs.open(MakePath(fileKey));

		if (!ifs.is_open()) {
			MSG_BOX("Wrong File path.  :Load_MaterialFromFile ");
			return MaterialHandles;
		}

		MATERIAL_FILE_HEADER fileHeader = {};
		ifs.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
		vector<CMaterialData*> materialDataContainer;

		string filePath = MakePath(fileHeader.materialFileKey);
		string fileDirectory = filesystem::path(filePath).parent_path().string() + "/";

		for (size_t i = 0; i < fileHeader.MaterialDataCount; i++)
		{
			CMaterialData* pData = CMaterialData::Create(levelTag, ifs, fileDirectory);
			materialDataContainer.push_back(pData);
			CMaterialInstance* pMaterialHandle = CMaterialInstance::Make_Handle(pData, m_pDevice);
			MaterialHandles.push_back(pMaterialHandle);
		}
		map.emplace(fileHeader.materialFileKey, move(materialDataContainer));
	}

	//그런 키가 있다면
	else {
		vector<CMaterialData*>& vector = iter->second;
		for (auto& pData : vector)
		{ //순회하면서 핸들에 담아.
			CMaterialInstance* pMaterialHandle = CMaterialInstance::Make_Handle(pData, m_pDevice);
			MaterialHandles.push_back(pMaterialHandle);
		}
	}

	return MaterialHandles;
}

CShader* CResourceMgr::Load_Shader(const string& levelTag, const string& shaderKey)
{
	int index = ValidLevel(levelTag);
	if (index == -1) {
		MSG_BOX("Wrong Level Tag. :Load_Shader ");
		return nullptr;
	}

	auto& map = m_Resources[index].m_Shaders;
	auto iter = map.find(shaderKey);

	if (iter != map.end()) return iter->second;

	if (MakePath(shaderKey).empty()) {
		MSG_BOX("There is No Key : Load_Shader");
	}
	CShader* pData = CShader::Create(m_pDevice, MakePath(shaderKey), shaderKey);
	map.emplace(shaderKey, pData);

	return pData;
}

CTexture* CResourceMgr::Load_Texture(const string& levelTag, const string& textureKey)
{
	int index = ValidLevel(levelTag);
	if (index == -1) {
		MSG_BOX("Wrong Level Tag. :Load_Texture ");
		return nullptr;
	}

	auto& map = m_Resources[index].m_Textures;
	auto iter = map.find(textureKey);

	if (iter != map.end()) return iter->second;

	wstring path = Helper::ConvertToWideString(MakePath(textureKey));
	CTexture* pData = CTexture::Create(m_pDevice, path, textureKey);
	map.emplace(textureKey, pData);

	return pData;
}

CAnimationClip* CResourceMgr::Load_AnimClip(const string& levelTag, 
	const string& AnimClipKey, 
	const string& Subject)
{
	int index = ValidLevel(levelTag);
	if (index == -1) {
		MSG_BOX("Wrong Level Tag. :Load_AnimClip ");
		return nullptr;
	}

	auto& map = m_Resources[index].m_Animations;
	auto iter = map.find(AnimClipKey + "_" + Subject);

	if (iter != map.end()) {
		if (iter->second->Get_Subject() == Subject)
			return iter->second;
	}

	CAnimationClip* pData = CAnimationClip::Create(MakePath(AnimClipKey + "_" + Subject), AnimClipKey, Subject);
	if (pData)
		map.emplace(AnimClipKey + "_" + Subject, pData);

	return pData;
}

CModelData* CResourceMgr::Load_ModelData(const string& levelTag, const string& ModelKey)
{
	int index = ValidLevel(levelTag);
	if (index == -1) {
		MSG_BOX("Wrong Level Tag. :Load_ModelData ");
		return nullptr;
	}

	auto& map = m_Resources[index].m_ModelDatas;
	auto iter = map.find(ModelKey);

	if (iter != map.end()) return iter->second;

	CModelData* pData = CModelData::Create(MakePath(ModelKey), m_pDevice);
	map.emplace(ModelKey, pData);

	return pData;
}


string CResourceMgr::Get_ResourcePath(const string& resourceKey)
{
	auto iter = m_KeyPath.find(resourceKey);
	if (iter != m_KeyPath.end()) return iter->second;
	else return string();
}

HRESULT CResourceMgr::Add_ResourcePath(const string& resourceKey, const string& resourcePath)
{
	auto iter = m_KeyPath.find(resourceKey);

	if (iter != m_KeyPath.end()) {
		string msg = "directory Exist: " + resourceKey + "\n";
		//string msg = "directory Exist: " + resourcePath + "\n";
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
	/*리소스 로더 필요. 초기 로딩 너무 많이 걸림*/

	m_LevelIndex.emplace(G_GlobalLevelKey, 0);
	m_Resources.resize(1);
	Add_ResourcePath("VTX_TexPos.hlsl", "../Bin/ShaderFiles/VTX_TexPos.hlsl");
	Add_ResourcePath("VTX_Mesh.hlsl", "../Bin/ShaderFiles/VTX_Mesh.hlsl");
	Add_ResourcePath("VTX_NorTex.hlsl", "../Bin/ShaderFiles/VTX_NorTex.hlsl");
	Add_ResourcePath("VTX_SkinMesh.hlsl", "../Bin/ShaderFiles/VTX_SkinMesh.hlsl");
	Add_ResourcePath("VTX_UI.hlsl", "../Bin/ShaderFiles/VTX_UI.hlsl");
	Add_ResourcePath("VTX_Debug.hlsl", "../Bin/ShaderFiles/VTX_Debug.hlsl");
	Add_ResourcePath("VTX_Point.hlsl", "../Bin/ShaderFiles/VTX_Point.hlsl");

	m_Resources[0].m_Buffers.emplace("Engine_Default_Rect", CVI_Rect::Create(m_pDevice, "Engine_Default_Rect"));
	m_Resources[0].m_Buffers.emplace("Engine_Default_Plane", CVI_Plane::Create(m_pDevice, "Engine_Default_Plane"));
	m_Resources[0].m_Buffers.emplace("Engine_Default_Point", CVI_Point::Create(m_pDevice, "Engine_Default_Point"));

	m_Resources[0].m_Shaders.emplace("VTX_TexPos.hlsl", CShader::Create(m_pDevice, "../Bin/ShaderFiles/VTX_TexPos.hlsl", "VTX_TexPos.hlsl"));
	m_Resources[0].m_Shaders.emplace("VTX_Mesh.hlsl", CShader::Create(m_pDevice, "../Bin/ShaderFiles/VTX_Mesh.hlsl", "VTX_Mesh.hlsl"));
	m_Resources[0].m_Shaders.emplace("VTX_NorTex.hlsl", CShader::Create(m_pDevice, "../Bin/ShaderFiles/VTX_NorTex.hlsl", "VTX_NorTex.hlsl"));
	m_Resources[0].m_Shaders.emplace("VTX_SkinMesh.hlsl", CShader::Create(m_pDevice, "../Bin/ShaderFiles/VTX_SkinMesh.hlsl", "VTX_SkinMesh.hlsl"));
	m_Resources[0].m_Shaders.emplace("VTX_UI.hlsl", CShader::Create(m_pDevice, "../Bin/ShaderFiles/VTX_UI.hlsl", "VTX_UI.hlsl"));
	m_Resources[0].m_Shaders.emplace("VTX_Debug.hlsl", CShader::Create(m_pDevice, "../Bin/ShaderFiles/VTX_Debug.hlsl", "VTX_Debug.hlsl"));
	m_Resources[0].m_Shaders.emplace("VTX_Point.hlsl", CShader::Create(m_pDevice, "../Bin/ShaderFiles/VTX_Point.hlsl", "VTX_Point.hlsl"));
	m_Resources[0].m_Shaders.emplace("Shader_Deferred.hlsl", CShader::Create(m_pDevice, "../Bin/ShaderFiles/Shader_Deferred.hlsl", "Shader_Deferred.hlsl"));
}


string CResourceMgr::MakePath(const string& pathKey)
{
	auto iter = m_KeyPath.find(pathKey);
	if (iter != m_KeyPath.end()) {
		return iter->second;
	}
	string msg = "Cant Find Path: " + pathKey + "\n";
	OutputDebugStringA(msg.c_str());
	return string();
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
}
