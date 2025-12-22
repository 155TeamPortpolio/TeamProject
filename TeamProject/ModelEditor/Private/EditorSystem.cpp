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
	vector<string> dummy;
	m_Directorys.swap(dummy);
	ModelHelper::Add_ModelPathFromDirectory(m_Directorys, mapDirectory);
	Read_MaterialMaps();
}

HRESULT CEditorSystem::Read_MaterialMaps()
{
	for (size_t i = 0; i < m_Directorys.size(); i++)
	{
		string path = m_Directorys[i];
		ifstream ifs(path);
		if (!ifs.is_open()) {
			string err = path + "파일을 찾을 수 없습니다.";
			MessageBoxA(nullptr, err.c_str(), "Error", MB_OK);
			return E_FAIL;
		}

		json jScene;
		try {
			ifs >> jScene;
		}
		catch (const json::parse_error& e) {
			MessageBoxA(nullptr, e.what(), "JSON Parse Error", MB_OK);
			return E_FAIL;
		}
		ifs.close();

		filesystem::path entry(m_Directorys[i]);
		string fileName = entry.stem().string();
		m_fileMaps.emplace(fileName, jScene);

	}
	return S_OK;
}

void CEditorSystem::Load_TextureMaps()
{
	
}

void CEditorSystem::Free()
{
	__super::Free();
}
