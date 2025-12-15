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

	string parentFolder = filesystem::path(fileDirectory).parent_path().string();
	string ParentName = filesystem::path(parentFolder).filename().string();

	//for (size_t i = 0; i < MAX_TEXTURE_TYPE_VALUE; i++)
	//{
	//	size_t texCount = pAIMaterial->GetTextureCount(static_cast<aiTextureType>(i));
	//
	//	for (size_t j = 0; j < texCount; j++)
	//	{
	//		aiString     strTexturePath;
	//		pAIMaterial->GetTexture(static_cast<aiTextureType>(i), j, &strTexturePath);
	//
	//		string extention = filesystem::path(strTexturePath.C_Str()).extension().string(); //".png"
	//		string fileName = Helper::GetFileNameWithOutExtension(strTexturePath.C_Str()); //"avsAlv" or "avsALv.0"
	//		string BaseName = Helper::GetFileNameWithOutExtension(fileName);
	//
	//		string filePath = fileDirectory + "\\" + BaseName + extention;
	//		/*일단 베이스 네임으로(인덱스 제외 후) 검색*/
	//		if (filesystem::exists(filePath)) { //있으면 로드
	//			CGameInstance::GetInstance()->Get_ResourceMgr()->Add_ResourcePath(BaseName + extention, filePath);
	//			Link_Texture(G_GlobalLevelKey, BaseName + extention, static_cast<TEXTURE_TYPE>(i));
	//		}
	//	}
	//}
	Add_AdditionalTexture(fileDirectory,"MAT_","_N.png",TEXTURE_TYPE::NORMALS);
	Add_AdditionalTexture(fileDirectory,"MAT_","_M.png",TEXTURE_TYPE::METALNESS);
	Add_AdditionalTexture(fileDirectory,"MAT_","_A.png",TEXTURE_TYPE::AMBIENT);
	Add_AdditionalTexture(fileDirectory,"MAT_","_D.png",TEXTURE_TYPE::DIFFUSE);

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
			string textureKey = Helper::GetFileNameWithOutExtension(pair.second[i]->Get_Key()) +".dds";
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
	ImGui::Begin("Add_Texture", nullptr, 0);

	// fallback 타입 목록 (프로젝트 enum에 맞춰 채워)
	static const TEXTURE_TYPE kFallbackTypes[] = {
		TEXTURE_TYPE::DIFFUSE,
	};

	// 콤보에 사용할 "타입 소스" 결정
	const bool useFallback = textureTypes.empty();

	int count = useFallback ? (int)(sizeof(kFallbackTypes) / sizeof(kFallbackTypes[0]))
		: (int)textureTypes.size();

	if (count <= 0)
	{
		ImGui::TextUnformatted("No selectable texture types (fallback list is empty).");
		ImGui::End();
		return;
	}

	// 인덱스 가드
	if (m_currentTextureTypeIndex < 0 || m_currentTextureTypeIndex >= count)
		m_currentTextureTypeIndex = 0;

	// 현재 선택 타입 얻기
	TEXTURE_TYPE selectedType = TEXTURE_TYPE::DIFFUSE;
	if (useFallback)
		selectedType = kFallbackTypes[m_currentTextureTypeIndex];
	else
		selectedType = static_cast<TEXTURE_TYPE>(textureTypes[m_currentTextureTypeIndex]);

	std::string strTextureType = ConvertToConstant(selectedType);

	// 콤보
	if (ImGui::BeginCombo("##textureTypes", strTextureType.c_str()))
	{
		for (int i = 0; i < count; ++i)
		{
			TEXTURE_TYPE t = useFallback
				? kFallbackTypes[i]
				: static_cast<TEXTURE_TYPE>(textureTypes[i]);

			bool isSelected = (i == m_currentTextureTypeIndex);

			if (ImGui::Selectable(ConvertToConstant(t).c_str(), isSelected))
				m_currentTextureTypeIndex = i;

			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	// 텍스처 찾기
	if (ImGui::Button("Find_Texture"))
	{
		std::string path = Helper::OpenFile_Dialogue();
		if (!path.empty())
		{
			std::string fileName = std::filesystem::path(path).filename().string();

			auto* rm = CGameInstance::GetInstance()->Get_ResourceMgr();
			rm->Add_ResourcePath(fileName, path);

			CTexture* pTexture = rm->Load_Texture(G_GlobalLevelKey, fileName);
			if (!pTexture)
			{
				ImGui::TextUnformatted("Failed to load texture.");
			}
			else
			{
				Safe_AddRef(pTexture);
				m_Textures[selectedType].push_back(pTexture);

				// 선택: textureTypes가 비어있으면 이번에 선택한 타입을 등록해두고 싶다면
				// (다음부터는 fallback 말고 실제 리스트로 운영)
				if (textureTypes.empty())
					textureTypes.push_back((_uint)selectedType);
			}
		}
	}

	ImGui::End();
}


void CAIMaterial::Add_AdditionalTexture(const string& fileDirectory, const string& preFix, const string& typeAdd, TEXTURE_TYPE type)
{
	// 접두사 제거
	const string prefix = preFix;

	/*Normal*/
	string Texturekey = m_MaterialKey;
	if (Texturekey.rfind(prefix, 0) == 0)
		Texturekey.erase(0, prefix.size());
	Texturekey += typeAdd;

	string filePath = fileDirectory + "\\" + Texturekey;
	/*일단 베이스 네임으로(인덱스 제외 후) 검색*/
	if (filesystem::exists(filePath)) { //있으면 로드
		CGameInstance::GetInstance()->Get_ResourceMgr()->
			Add_ResourcePath(Texturekey, filePath);
		Link_Texture(G_GlobalLevelKey, Texturekey, type);
	}
}

CAIMaterial* CAIMaterial::Create(const aiMaterial* pAIMaterial, const string& fileDirectory)
{
	CAIMaterial* instance = new CAIMaterial();
	if (FAILED(instance->Initialize(pAIMaterial, fileDirectory))) {
		Safe_Release(instance);
	}
	return instance;
}

CAIMaterial* CAIMaterial::Create(const string& name)
{
	CAIMaterial* instance = new CAIMaterial();
	instance->m_MaterialKey = name;
	instance->m_passConstant = "Opaque";
	return instance;
}

void CAIMaterial::Free()
{
	__super::Free();
}
