#include "pch.h"
#include "AI_Material.h"

#include "Helper_Func.h"
#include "GameInstance.h"
#include "IResourceService.h"
#include "GameObject.h"
#include "Texture.h"
#include "AIMaterial.h"
#include "MaterialInstance.h"

CAI_Material::CAI_Material()
	:m_pDevice(CGameInstance::GetInstance()->Get_Device()),
	m_pContext(CGameInstance::GetInstance()->Get_Context())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CAI_Material::CAI_Material(const CAI_Material& rhs)
	:CMaterial(rhs), m_pDevice(rhs.m_pDevice), m_pContext(rhs.m_pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CAI_Material::Initialize()
{

	return S_OK;
}

void CAI_Material::Render_GUI()
{
	__super::Render_GUI();

}

HRESULT CAI_Material::Load_Material(_uint materialNum, aiMaterial* material[], const string& filePath)
{
	for (auto& MatData : m_MaterialInstances)
		Safe_Release(MatData);

	m_AIMaterialDatas.clear();
	m_MaterialInstances.clear();

	m_MaterialFileKey = Helper::GetFileNameWithOutExtension(filePath);
	string Directory = filesystem::path(filePath).parent_path().string();

	for (size_t i = 0; i < materialNum; i++)
	{
		CAIMaterial* data = CAIMaterial::Create(material[i], Directory);
		if (data) {
			m_AIMaterialDatas.push_back(data);
			CMaterialInstance* pHandle = CMaterialInstance::Make_Handle(data, m_pDevice);
			m_MaterialInstances.push_back(pHandle);
		}
	}
	return S_OK;
}

HRESULT CAI_Material::Save_Material()
{

	string path = Helper::SaveFileDialogByWinAPI(m_MaterialFileKey, "mat");
	filesystem::path directory(path);
	ofstream ofs(path.c_str(), ios::binary);
	if (!ofs.is_open())
		return E_FAIL;
	string FileKey = m_MaterialFileKey + ".mat";
	MATERIAL_FILE_HEADER fileHead = {};
	fileHead.MaterialDataCount = m_MaterialInstances.size();
	strcpy_s(fileHead.materialFileKey, sizeof(fileHead.materialFileKey), FileKey.c_str());
	ofs.write(reinterpret_cast<const char*>(&fileHead), sizeof(fileHead));

	for (size_t i = 0; i < m_MaterialInstances.size(); i++) {
		CAIMaterial* data = dynamic_cast<CAIMaterial*>(m_MaterialInstances[i]->Get_MaterialData());
		data->Save_MaterialData(m_pContext, ofs, directory.parent_path().string());
	}

	ofs.close();
}

HRESULT CAI_Material::Save_Material(const string& SavePath, const string& fileName)
{

	filesystem::path folderPath = filesystem::path(SavePath) / fileName;

	filesystem::create_directories(folderPath);
	filesystem::path fullSavePath = folderPath / (fileName + ".mat");

	ofstream ofs(fullSavePath.c_str(), ios::binary);
	if (!ofs.is_open())
		return E_FAIL;
	string FileKey = m_MaterialFileKey + ".mat";
	MATERIAL_FILE_HEADER fileHead = {};
	fileHead.MaterialDataCount = m_MaterialInstances.size();
	strcpy_s(fileHead.materialFileKey, sizeof(fileHead.materialFileKey), FileKey.c_str());
	ofs.write(reinterpret_cast<const char*>(&fileHead), sizeof(fileHead));

	for (size_t i = 0; i < m_MaterialInstances.size(); i++) {
		CAIMaterial* data = dynamic_cast<CAIMaterial*>(m_MaterialInstances[i]->Get_MaterialData());
		data->Save_MaterialData(m_pContext, ofs, folderPath.string(), m_OverrideShaderKey);
	}

	ofs.close();

	return S_OK;
}

void CAI_Material::LinkShader(const string& shader)
{
	for (auto& Aidata : m_AIMaterialDatas)
		Aidata->LinkShader(shader);
}

CAI_Material* CAI_Material::Create()
{
	CAI_Material* instance = new CAI_Material;
	if (FAILED(instance->Initialize_Prototype())) {
		Safe_Release(instance);
	}
	return instance;
}

CComponent* CAI_Material::Clone()
{
	CAI_Material* instance = new CAI_Material(*this);
	return instance;
}

void CAI_Material::Free()
{
	__super::Free();
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	for (auto& Aidata : m_AIMaterialDatas)
		Safe_Release(Aidata);
}
