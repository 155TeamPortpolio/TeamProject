#include "pch.h"
#include "AIMaterial.h"
#include "Texture.h"
#include "GameInstance.h"
#include "IResourceService.h"
#include "Helper_Func.h"

CAIMaterial::CAIMaterial()
{
}


HRESULT CAIMaterial::Initialize(const aiMaterial* pAIMaterial, const string& fileDirectory)
{
	m_MaterialKey = pAIMaterial->GetName().C_Str();
	for (size_t i = 0; i < MAX_TEXTURE_TYPE_VALUE; i++)
	{
		size_t texCount = pAIMaterial->GetTextureCount(static_cast<aiTextureType>(i));

		for (size_t j = 0; j < texCount; j++)
		{
			aiString     strTexturePath;
			pAIMaterial->GetTexture(static_cast<aiTextureType>(i), j, &strTexturePath);

			string extention = filesystem::path(strTexturePath.C_Str()).extension().string(); //".png"
			string fileName = Helper::GetFileNameWithOutExtension(strTexturePath.C_Str()); //"avsAlv" or "avsALv.0"
			string BaseName = Helper::GetFileNameWithOutExtension(fileName);

			string filePath = fileDirectory + "\\" + BaseName + extention;
			/*일단 베이스 네임으로(인덱스 제외 후) 검색*/
			if (filesystem::exists(filePath)) { //있으면 로드
				CGameInstance::GetInstance()->Get_ResourceMgr()->Add_ResourcePath(BaseName + extention, filePath);
				Link_Texture(G_GlobalLevelKey, BaseName + extention, static_cast<TEXTURE_TYPE>(i));
			}

			/*순회 검색 시작*/
			_uint Index = {};

			while (true) {
				string targetName = BaseName + "." + to_string(Index);
				string IndexedPath = fileDirectory + "\\" + targetName + extention;

				if (filesystem::exists(IndexedPath)) { //있으면 로드
					CGameInstance::GetInstance()->Get_ResourceMgr()->Add_ResourcePath(targetName + extention, IndexedPath);
					Link_Texture(G_GlobalLevelKey, targetName + extention, static_cast<TEXTURE_TYPE>(i));
					Index++;
				}
				else {
					break;
				}
			}
		}
	}

	m_passConstant = "Opaque";

	for (size_t i = 0; i < MAX_TEXTURE_TYPE_VALUE; i++)
	{
		if (ConvertToConstant(static_cast<TEXTURE_TYPE>(i)).empty()) continue;
		textureTypes.push_back(i);
	}

	return S_OK;
}


void CAIMaterial::Save_MaterialData(ID3D11DeviceContext* pContext, ofstream& ofs, const string& directory, const string& overrideKey)
{
	MATERIAL_INFO_HEADER infoHead = {};

	strcpy_s(infoHead.materialDataKey, sizeof(infoHead.materialDataKey), m_MaterialKey.c_str());
	strcpy_s(infoHead.passConstant, sizeof(infoHead.passConstant), m_passConstant.c_str());
	if (overrideKey.empty() && m_pShader)
		strcpy_s(infoHead.ShaderKey, sizeof(infoHead.ShaderKey), m_pShader->Get_Key().c_str());
	else {
		strcpy_s(infoHead.ShaderKey, sizeof(infoHead.ShaderKey), overrideKey.c_str());
	}
	memcpy(&infoHead.materialConstant, &m_DefaultMaterialConstant, sizeof(MaterialConstants));

	infoHead.TextureTypeCount = m_Textures.size();
	ofs.write(reinterpret_cast<const char*>(&infoHead), sizeof(infoHead));

	for (auto pair : m_Textures) {
		TEXTURE_FILE_HEADER texHeader = {};
		texHeader.TextureCount = pair.second.size();
		texHeader.typeID = static_cast<_uint>(pair.first);
		ofs.write(reinterpret_cast<const char*>(&texHeader), sizeof(TEXTURE_FILE_HEADER));

		for (size_t i = 0; i < pair.second.size(); i++)
		{
			TEXTURE_INFO_HEADER texInfo = {};
			string textureKey = Helper::GetFileNameWithOutExtension(pair.second[i]->Get_Key()) + "." + to_string(i) + ".dds";
			strcpy_s(texInfo.TextureKey, sizeof(texInfo.TextureKey), textureKey.c_str());
			ofs.write(reinterpret_cast<const char*>(&texInfo), sizeof(texInfo));
			if (FAILED(Helper::SaveTextureToDDs(pContext, directory + "\\" + textureKey, pair.second[i]->Get_SRV()))) {
				MSG_BOX("DDS SAVE FAILED : Save_MaterialData");
			}
		}
	}
}
void CAIMaterial::Render_GUI()
{
	vector<string> passes = m_pShader->Get_PassList();
	if (passes.empty())
		return;

	float childWidth = ImGui::GetContentRegionAvail().x;//->이건 넓이 설정
	ImGui::SetNextItemWidth(childWidth);
	if (ImGui::BeginCombo(string("##shaderPass").c_str(), passes[m_currentPassIndex].c_str())) {
		for (int i = 0; i < passes.size(); ++i) {
			bool isSelected = (i == m_currentPassIndex);

			if (ImGui::Selectable(passes[i].c_str(), isSelected)) {
				m_currentPassIndex = i;
				m_passConstant = passes[i];
			}

			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	if (ImGui::Button("Add MaterialData")) {
		MaterialTabOpened = !MaterialTabOpened;
	}

	if (MaterialTabOpened)
		Render_MaterialAdd();

	__super::Render_GUI();
}

void CAIMaterial::Render_GUI(vector<_uint>& TextureIndexes)
{
	vector<string> passes = m_pShader->Get_PassList();
	if (passes.empty())
		return;

	float childWidth = ImGui::GetContentRegionAvail().x;//->이건 넓이 설정
	ImGui::SetNextItemWidth(childWidth);

	if (ImGui::BeginCombo(string("##shaderPass").c_str(), passes[m_currentPassIndex].c_str())) {
		for (int i = 0; i < passes.size(); ++i) {
			bool isSelected = (i == m_currentPassIndex);

			if (ImGui::Selectable(passes[i].c_str(), isSelected)) {
				m_currentPassIndex = i;
				m_passConstant = passes[i];
			}

			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	if (ImGui::Button("Add MaterialData")) {
		MaterialTabOpened = !MaterialTabOpened;
	}

	if (MaterialTabOpened)
		Render_MaterialAdd();

	__super::Render_GUI(TextureIndexes);
}

void CAIMaterial::LinkShader(const string& shader)
{
	Link_Shader(G_GlobalLevelKey, shader);
}

void CAIMaterial::Render_MaterialAdd()
{
	_uint iTextureType = textureTypes[m_currentTextureTypeIndex];
	string strTextureType = ConvertToConstant(static_cast<TEXTURE_TYPE>(iTextureType));

	ImGui::Begin("Add_Texture", 0, 0);
	if (ImGui::BeginCombo(string("##textureTypes").c_str(), strTextureType.c_str())) {
		for (int i = 0; i < textureTypes.size(); ++i) {
			bool isSelected = (i == m_currentTextureTypeIndex);

			if (ImGui::Selectable(ConvertToConstant(static_cast<TEXTURE_TYPE>(textureTypes[i])).c_str(), isSelected)) {
				m_currentTextureTypeIndex = i;
			}

			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	if (ImGui::Button("Find_Texture")) {
		string path = Helper::OpenFile_Dialogue();
		if (path.empty()) return;
		string fileName = filesystem::path(path).filename().string();
		CGameInstance::GetInstance()->Get_ResourceMgr()->Add_ResourcePath(fileName, path);
		CTexture* pTexture = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_Texture(G_GlobalLevelKey, fileName);
		Safe_AddRef(pTexture);
		m_Textures[static_cast<TEXTURE_TYPE>(textureTypes[m_currentTextureTypeIndex])].push_back(pTexture);
	}


	ImGui::End();
}

CAIMaterial* CAIMaterial::Create(const aiMaterial* pAIMaterial, const string& fileDirectory)
{
	CAIMaterial* instance = new CAIMaterial();
	if (FAILED(instance->Initialize(pAIMaterial, fileDirectory))) {
		Safe_Release(instance);
	}
	return instance;
}

void CAIMaterial::Free()
{
	__super::Free();
}
