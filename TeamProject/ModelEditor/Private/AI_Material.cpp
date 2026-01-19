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
	if (m_MaterialInstances.empty())
		return;

	ImGui::SeparatorText("Material");
	float childWidth = ImGui::GetContentRegionAvail().x;
	const float textLineHeight = ImGui::GetTextLineHeightWithSpacing();
	const float childHeight = (m_MaterialInstances.size() * 2) + (ImGui::GetStyle().WindowPadding.y * 4);

	if (ImGui::Button("Material Tabs")) {
		m_bMaterialTabOpen = true;
	}

	if (m_bMaterialTabOpen) {
		ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Materials", &m_bMaterialTabOpen, ImGuiWindowFlags_NoCollapse))
		{
			if (ImGui::BeginTabBar("##MaterialTabs"))
			{
				for (int i = 0; i < m_MaterialInstances.size(); ++i)
				{
					CMaterialInstance* hMaterial = m_MaterialInstances[i];
					const bool specific = m_AIMaterialDatas[i]->Get_Specific();

					if (specific)
					{
						ImVec4 col = ImGui::GetStyleColorVec4(ImGuiCol_Tab);
						ImVec4 colH = ImGui::GetStyleColorVec4(ImGuiCol_TabHovered);
						ImVec4 colA = ImGui::GetStyleColorVec4(ImGuiCol_TabActive);

						col = ImVec4(col.x + 0.80f, col.y + 0.05f, col.z + 0.00f, col.w);
						colH = ImVec4(colH.x + 0.80f, colH.y + 0.07f, colH.z + 0.00f, colH.w);
						colA = ImVec4(colA.x + 0.80f, colA.y + 0.10f, colA.z + 0.00f, colA.w);

						ImGui::PushStyleColor(ImGuiCol_Tab, col);
						ImGui::PushStyleColor(ImGuiCol_TabHovered, colH);
						ImGui::PushStyleColor(ImGuiCol_TabActive, colA);
					}

					if (ImGui::BeginTabItem(hMaterial->Get_MaterialName().c_str()))
					{
						hMaterial->Render_GUI();
						ImGui::EndTabItem();
					}

					if (specific)
					{
						ImGui::PopStyleColor(3);
					}
				}
			}
			ImGui::EndTabBar();
		}
		ImGui::End();
	}
	if (ImGui::Button("Add_Material")) 
		m_bCustomTabOpened = !m_bCustomTabOpened;
	
	if (m_bCustomTabOpened)
		Render_CustomMaterial();
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

HRESULT CAI_Material::Save_Material(const string& SavePath)
{

	string path = SavePath + m_MaterialFileKey + ".mat";
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

static bool IsUnderDirectory(const filesystem::path& file, const filesystem::path& dir)
{
	error_code ec;

	auto f = std::filesystem::weakly_canonical(file, ec);
	if (ec) return false;

	auto d = std::filesystem::weakly_canonical(dir, ec);
	if (ec) return false;

	auto fit = f.begin();
	for (auto dit = d.begin(); dit != d.end(); ++dit, ++fit)
	{
		if (fit == f.end() || *fit != *dit)
			return false;
	}
	return true;
}

void CAI_Material::Render_CustomMaterial()
{
	const ImVec2 winSize(400.f, 400.f);
	ImGui::SetNextWindowSize(winSize, ImGuiCond_Appearing);

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	ImGui::Begin("Material_Add");

	ImGui::Text("MaterialKey");
	if (ImGui::InputText("##CustomMaterialKey", m_MaterialKeyBuf, sizeof(m_MaterialKeyBuf)))
		Added_MaterialName = m_MaterialKeyBuf;

	if (Added_MaterialName.empty())
	{
		ImGui::End();
		return;
	}

	if (ImGui::Button("Link_Shader"))
	{
		string filePath = Helper::OpenFile_Dialogue();
		if (!filePath.empty())
		{
			filesystem::path baseDir =
			filesystem::current_path() / ".." / "Bin" / "ShaderFiles";

			if (!IsUnderDirectory(filesystem::path(filePath), baseDir))
			{
				Added_ShaderFile = "Invalid path: must be under ../Bin/ShaderFiles/";
			}
			else
			{
				Added_ShaderFile = filePath;

				string file = filesystem::path(Added_ShaderFile).filename().string();
				CGameInstance::GetInstance()->Get_ResourceMgr()->Add_ResourcePath(file, Added_ShaderFile);

				CAIMaterial* data = CAIMaterial::Create(Added_MaterialName);
				if (data)
				{
					data->LinkShader(file);
					m_AIMaterialDatas.push_back(data);

					CMaterialInstance* pHandle = CMaterialInstance::Make_Handle(data, m_pDevice);
					m_MaterialInstances.push_back(pHandle);

					Added_ShaderFile.clear();
					Added_MaterialName.clear();
					m_MaterialKeyBuf[0] = '\0';
				}
			}
		}
	}

	ImGui::End();
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
