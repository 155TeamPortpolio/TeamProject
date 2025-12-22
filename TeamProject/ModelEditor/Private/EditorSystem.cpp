#include "pch.h"
#include "EditorSystem.h"
#include "GameInstance.h"
#include "EditModel.h"
#include "RayCaster.h"
#include "Layer.h"
#include "AI_SKModel.h"
#include "AI_STModel.h"
#include "AIModelData.h"
#include "AIMesh.h"
#include "GameObject.h"
#include "ModelHelper.h"

IMPLEMENT_SINGLETON(CEditorSystem)

namespace
{
	static const nlohmann::json* FindPtr(const nlohmann::json& root, initializer_list<const char*> keys)
	{
		const nlohmann::json* cur = &root;
		for (auto* k : keys)
		{
			if (!cur->is_object()) return nullptr;
			auto it = cur->find(k);
			if (it == cur->end()) return nullptr;
			cur = &(*it);
		}
		return cur;
	}
	static string GetTexNameOrEmpty(const nlohmann::json& root, const char* texEnvKey)
	{
		auto* t = FindPtr(root, { "m_SavedProperties","m_TexEnvs", texEnvKey, "m_Texture" });
		if (!t || !t->is_object()) return {};

		if (t->value("IsNull", true)) return {};

		string name = t->value("Name", string{});
		return name; // Name 비면 그대로 "" 반환
	}
	static MaterialTexNames Extract4Maps_Strict(const nlohmann::json& root)
	{
		MaterialTexNames out;
		out.Albedo = GetTexNameOrEmpty(root, "_BaseMap");
		out.Normal = GetTexNameOrEmpty(root, "_BumpMap");
		out.Metallic = GetTexNameOrEmpty(root, "_MetallicGlossMap");
		out.AO = GetTexNameOrEmpty(root, "_OcclusionMap");
		return out;
	}
	static void WriteU32(std::ofstream& os, uint32_t v)
	{
		os.write(reinterpret_cast<const char*>(&v), sizeof(v));
	}

	static void WriteString(std::ofstream& os, const std::string& s)
	{
		uint32_t len = static_cast<uint32_t>(s.size());
		WriteU32(os, len);
		if (len) os.write(s.data(), len);
	}

	static bool ReadU32(std::ifstream& is, uint32_t& v)
	{
		return static_cast<bool>(is.read(reinterpret_cast<char*>(&v), sizeof(v)));
	}

	static bool ReadString(std::ifstream& is, std::string& out)
	{
		uint32_t len = 0;
		if (!ReadU32(is, len)) return false;
		out.clear();
		if (len == 0) return true;
		out.resize(len);
		return static_cast<bool>(is.read(out.data(), len));
	}
}

CEditorSystem::CEditorSystem()
{
}

HRESULT CEditorSystem::Initialize()
{

	return S_OK;
}

void CEditorSystem::Update()
{
	
}

void CEditorSystem::Load_MaterialMaps(const string& mapDirectory)
{
	m_matTexNames.clear();

	vector<string> dummy;
	m_Directorys.swap(dummy);
	ModelHelper::Add_ModelPathFromDirectory(m_Directorys, mapDirectory);

	Read_MaterialMaps();

	// 저장
	namespace fs =filesystem;
	fs::path cachePath = fs::path(mapDirectory) / "MaterialTexCache.bin";
	ofstream os(cachePath, ios::binary);
	if (!os.is_open()) return;

	os.write("MATC", 4);
	WriteU32(os, 1);
	WriteU32(os, static_cast<uint32_t>(m_matTexNames.size()));
	for (const auto& [key, v] : m_matTexNames)
	{
		WriteString(os, key);
		WriteString(os, v.Albedo);
		WriteString(os, v.Normal);
		WriteString(os, v.Metallic);
		WriteString(os, v.AO);
	}
}


HRESULT CEditorSystem::Read_MaterialMaps()
{
	m_matTexNames.clear();

	for (size_t i = 0; i < m_Directorys.size(); i++)
	{
		const string& path = m_Directorys[i];
		ifstream ifs(path);
		if (!ifs.is_open()) {
			string err = path + "파일을 찾을 수 없습니다.";
			MessageBoxA(nullptr, err.c_str(), "Error", MB_OK);
			return E_FAIL;
		}

		json jScene;
		try { ifs >> jScene; }
		catch (const json::parse_error& e) {
			MessageBoxA(nullptr, e.what(), "JSON Parse Error", MB_OK);
			return E_FAIL;
		}

		filesystem::path entry(path);
		string fileName = entry.stem().string();

    	m_matTexNames.emplace(fileName, Extract4Maps_Strict(jScene));
	}

	return S_OK;
}

HRESULT CEditorSystem::Read_MaterialMapsCache(const string& mapDirectory)
{
	namespace fs =filesystem;
	fs::path cachePath = fs::path(mapDirectory);

	ifstream is(cachePath, ios::binary);
	if (!is.is_open()) return E_FAIL;

	char sig[4];
	if (!is.read(sig, 4)) return E_FAIL;
	if (memcmp(sig, "MATC", 4) != 0) return E_FAIL;

	uint32_t ver = 0;
	if (!ReadU32(is, ver)) return E_FAIL;

	uint32_t count = 0;
	if (!ReadU32(is, count)) return E_FAIL;

	m_matTexNames.clear();
	m_matTexNames.reserve(count);

	for (uint32_t i = 0; i < count; ++i)
	{
		string key;
		MaterialTexNames v;

		if (!ReadString(is, key)) return E_FAIL;
		if (!ReadString(is, v.Albedo)) return E_FAIL;
		if (!ReadString(is, v.Normal)) return E_FAIL;
		if (!ReadString(is, v.Metallic)) return E_FAIL;
		if (!ReadString(is, v.AO)) return E_FAIL;

		m_matTexNames.emplace(move(key), std::move(v));
	}

	return S_OK;
}


void CEditorSystem::Load_TextureMaps(const string& mapDirectory)
{
	
}

void CEditorSystem::Free()
{
	__super::Free();
}



