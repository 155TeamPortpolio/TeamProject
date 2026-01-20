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
	m_LogMsgs.clear();

	m_MaterialKey = pAIMaterial ? pAIMaterial->GetName().C_Str() : "";

	string parentFolder = filesystem::path(fileDirectory).parent_path().string();
	string ParentName = filesystem::path(parentFolder).filename().string();

	Add_AdditionalTexture(fileDirectory, "", "Map_N.png", TEXTURE_TYPE::NORMALS);
	Add_AdditionalTexture(fileDirectory, "", "Map_M.png", TEXTURE_TYPE::METALNESS);
	Add_AdditionalTexture(fileDirectory, "", "Map_A.png", TEXTURE_TYPE::AMBIENT);
	Add_AdditionalTexture(fileDirectory, "", "Map_D.png", TEXTURE_TYPE::DIFFUSE);

	LoadByAssimp(fileDirectory, pAIMaterial);

	m_passConstant = "Opaque";
	for (size_t i = 0; i < MAX_TEXTURE_TYPE_VALUE; i++)
	{
		if (ConvertToConstant(static_cast<TEXTURE_TYPE>(i)).empty()) continue;
		textureTypes.push_back(i);
	}

	ReCheck_Material(fileDirectory);
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
			string textureKey = Helper::GetFileNameWithOutExtension(pair.second[i]->Get_Key()) + ".dds";
			strcpy_s(texInfo.TextureKey, sizeof(texInfo.TextureKey), textureKey.c_str());
			ofs.write(reinterpret_cast<const char*>(&texInfo), sizeof(texInfo));
			if (FAILED(Helper::SaveTextureToDDs(pContext, directory + "\\" + textureKey, pair.second[i]->Get_SRV()))) {
				MSG_BOX("DDS SAVE FAILED : Save_MaterialData");
			}
		}
	}

}
void CAIMaterial::Render_GUI(vector<_uint>& TextureIndexes)
{
	vector<string> passes = m_pShader->Get_PassList();
	if (passes.empty())
		return;

	float childWidth = ImGui::GetContentRegionAvail().x;//->이건 넓이 설정
	ImGui::SetNextItemWidth(childWidth);
	if (ImGui::Button("Link_Shader"))
	{
		string filePath = Helper::OpenFile_Dialogue();
		if (!filePath.empty())
		{
			filesystem::path baseDir =
				filesystem::current_path() / ".." / "Bin" / "ShaderFiles";
			_bool inBin = !Helper::IsUnderDirectory(filesystem::path(filePath), baseDir);
			if (inBin)
			{
				MSG_BOX("Invalid path: must be under ../Bin/ShaderFiles/");
			}
			else
			{
				string file = filesystem::path(filePath).filename().string();
				CGameInstance::GetInstance()->Get_ResourceMgr()->Add_ResourcePath(file, filePath);
				Link_Shader(G_GlobalLevelKey, file);
			}
		}
	}
	ImGui::SameLine();
	ImGui::Text(m_MaterialKey.c_str());
	if (ImGui::Button("Add Materia(One)")) {
		MaterialTabOpened = !MaterialTabOpened;
	}
	ImGui::SameLine(0.f,5.f);
	if (ImGui::Button("Add Material(Set)")) {
		string path = Helper::OpenFile_Dialogue();
		if (!path.empty())
		{
			string fileName = std::filesystem::path(path).filename().string();

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
				m_Textures[TEXTURE_TYPE::DIFFUSE].push_back(pTexture);
				if (textureTypes.empty())
					textureTypes.push_back((_uint)TEXTURE_TYPE::DIFFUSE);

				AddByDiffuse(fileName, path);
			}
		}
	}
	if (MaterialTabOpened)
		Render_MaterialAdd();

	if (!m_LogMsgs.empty()) {
		ImGui::Begin("Err Msg");
		for (auto msg : m_LogMsgs)
			ImGui::Text(msg.c_str());
		ImGui::End();
	}
	if (m_Textures.empty())
		return;

	for (auto& pair : m_Textures) {
		ImGui::SeparatorText(ConvertToConstant(pair.first).c_str()); //텍스처 타입 콘스탄트로
		_uint& CurrentIndex = TextureIndexes[static_cast<_uint>(pair.first)];
		 auto& vector = pair.second;
		for (size_t i = 0; i < vector.size(); i++)
		{
			string ButtonID = "##" + vector[i]->Get_Key() + "_" + to_string(i);
			bool clicked = ImGui::ImageButton(
				ButtonID.c_str(),
				(ImTextureID)vector[i]->Get_SRV(),
				ImVec2(64, 64)
			);
			if (clicked)
			{
				CurrentIndex = static_cast<_uint>(i);
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
			{
				Safe_Release(vector[i]);
				pair.second.erase(pair.second.begin() + i);

				if (CurrentIndex >= pair.second.size())
					CurrentIndex = pair.second.empty() ? 0 : static_cast<_uint>(pair.second.size() - 1);
				break;
			}

			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Image((ImTextureID)vector[i]->Get_SRV(), ImVec2(256, 256));
				ImGui::Text(vector[i]->Get_Key().c_str());
				ImGui::EndTooltip();
			}
			ImGui::Text(vector[i]->Get_Key().c_str());
			ImGui::Separator();
		}
	}

}

void CAIMaterial::LinkShader(const string& shader)
{
	Link_Shader(G_GlobalLevelKey, shader);
}

HRESULT CAIMaterial::LoadByAssimp(const std::string& fileDirectory, const aiMaterial* mat)
{
	if (!mat) return E_FAIL;

	namespace fs = std::filesystem;
	fs::path baseDir = fs::path(fileDirectory);

	for (int ti = 0; ti < AI_TEXTURE_TYPE_MAX; ++ti)
	{
		aiTextureType aiType = static_cast<aiTextureType>(ti);
		const unsigned int count = mat->GetTextureCount(aiType);

		for (unsigned int j = 0; j < count; ++j)
		{
			aiString aiPath;
			if (AI_SUCCESS != mat->GetTexture(aiType, j, &aiPath))
				continue;

			const char* raw = aiPath.C_Str();
			if (!raw || !raw[0]) continue;

			// 임베디드(*0 같은)면 여기선 파일로 못 찾음 -> 스킵 or 별도 처리
			if (raw[0] == '*')
				continue;

			fs::path p(raw);
			fs::path full = p.is_absolute() ? p : (baseDir / p);
			full = full.lexically_normal();

			if (!fs::exists(full)) {
				continue;
			}

			string filePath = full.string();
			string textureKey = full.filename().string();
			auto rm = CGameInstance::GetInstance()->Get_ResourceMgr();
			rm->Add_ResourcePath(textureKey, filePath);
			_uint type = static_cast<_uint>(aiType);
			Link_Texture(G_GlobalLevelKey, textureKey, static_cast<TEXTURE_TYPE>(type));
		}
	}

	return S_OK;
}

void CAIMaterial::ReCheck_Material(const string& fileDirectory)
{

	Add_AdditionalTexture(fileDirectory, "", "_N.png", TEXTURE_TYPE::NORMALS);
	Add_AdditionalTexture(fileDirectory, "", "_M.png", TEXTURE_TYPE::METALNESS);
	Add_AdditionalTexture(fileDirectory, "", "_A.png", TEXTURE_TYPE::AMBIENT);
	Add_AdditionalTexture(fileDirectory, "", "_D.png", TEXTURE_TYPE::DIFFUSE);

	_bool hasDiffuse = !m_Textures[TEXTURE_TYPE::DIFFUSE].empty();
	_bool hasNormal = !m_Textures[TEXTURE_TYPE::NORMALS].empty();
	_bool hasMetal = !m_Textures[TEXTURE_TYPE::METALNESS].empty();
	_bool hasAmbient = !m_Textures[TEXTURE_TYPE::AMBIENT].empty();
	_bool hasNothing = !(hasDiffuse || hasNormal || hasMetal || hasAmbient);

	if (hasNothing) {
		m_bSpecific = true;
	}

	if (hasDiffuse) {
		string Key = m_Textures[TEXTURE_TYPE::DIFFUSE].front()->Get_Key();
		filesystem::path fileName(Key);
		string stem = fileName.stem().string();
		const string oldSuffix = "_D";
		if (stem.size() >= oldSuffix.size() &&stem.compare(stem.size() - oldSuffix.size(), oldSuffix.size(), oldSuffix) == 0)
		{
			stem.erase(stem.size() - oldSuffix.size()); 
		}

		Add_ReTexture(fileDirectory, stem, "_N",TEXTURE_TYPE::NORMALS);
		Add_ReTexture(fileDirectory, stem, "_M",TEXTURE_TYPE::METALNESS);
		Add_ReTexture(fileDirectory, stem, "_A",TEXTURE_TYPE::AMBIENT);
	}
}

_bool CAIMaterial::EndsWith(const string& text, const string& suffix)
{
	if (text.size() < suffix.size())
		return false;
	return equal(suffix.rbegin(), suffix.rend(), text.rbegin());
}

 _bool CAIMaterial::MakeSiblingNameFromDiffuse(
	const string& diffuseFileName,
	const char replaceSuffixChar,
	string& outSiblingFileName)
{
	filesystem::path diffusePath(diffuseFileName);
	string stem = diffusePath.stem().string();      
	string ext = diffusePath.extension().string(); 
	
	if (!EndsWith(stem, "_D"))
		return false;

	stem.pop_back(); 
	stem.push_back(replaceSuffixChar); 

	outSiblingFileName = stem + ext; 
	return true;
}
void CAIMaterial::AddByDiffuse(const string& DiffuseName, const string& fullpath)
{
	filesystem::path diffusePath(fullpath);
	filesystem::path folderPath = diffusePath.parent_path();

	const char siblingSuffixChars[] = { 'N', 'A', 'M' };
	auto* resourceMgr = CGameInstance::GetInstance()->Get_ResourceMgr();

	for (char suffixChar : siblingSuffixChars)
	{
		string siblingFileName;
		if (!MakeSiblingNameFromDiffuse(DiffuseName, suffixChar, siblingFileName))
			continue;

		filesystem::path siblingFullPath = folderPath / siblingFileName;
		if (!filesystem::exists(siblingFullPath))
			continue;

		resourceMgr->Add_ResourcePath(siblingFileName, siblingFullPath.string());

		CTexture* siblingTexture = resourceMgr->Load_Texture(G_GlobalLevelKey, siblingFileName);
		if (!siblingTexture)
			continue;

		Safe_AddRef(siblingTexture);

		TEXTURE_TYPE textureType = TextureTypeFromSuffixChar(suffixChar);

		m_Textures[textureType].push_back(siblingTexture);

		_bool ContainsType = find(textureTypes.begin(), textureTypes.end(), (_uint)textureType) != textureTypes.end();
		if (!ContainsType)
			textureTypes.push_back((_uint)textureType);
	}
}


void CAIMaterial::Render_MaterialAdd()
{
	ImGui::Begin("Add_Texture", nullptr, 0);
	static const TEXTURE_TYPE kFallbackTypes[] = {
		TEXTURE_TYPE::DIFFUSE,
	};

	const bool useFallback = textureTypes.empty();
	int count = useFallback ? (int)(sizeof(kFallbackTypes) / sizeof(kFallbackTypes[0]))
		: (int)textureTypes.size();

	if (count <= 0)
	{
		ImGui::TextUnformatted("No selectable texture types (fallback list is empty).");
		ImGui::End();
		return;
	}

	if (m_currentTextureTypeIndex < 0 || m_currentTextureTypeIndex >= count)
		m_currentTextureTypeIndex = 0;

	TEXTURE_TYPE selectedType = TEXTURE_TYPE::DIFFUSE;
	if (useFallback)
		selectedType = kFallbackTypes[m_currentTextureTypeIndex];
	else
		selectedType = static_cast<TEXTURE_TYPE>(textureTypes[m_currentTextureTypeIndex]);

	string strTextureType = ConvertToConstant(selectedType);

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
				if (textureTypes.empty())
					textureTypes.push_back((_uint)selectedType);
			}
		}
	}

	ImGui::End();
}

TEXTURE_TYPE CAIMaterial::TextureTypeFromSuffixChar(char suffixChar)
{
	switch (suffixChar)
	{
	case 'N': return TEXTURE_TYPE::NORMALS;
	case 'A': return TEXTURE_TYPE::AMBIENT;
	case 'M': return TEXTURE_TYPE::METALNESS;
	default:  return TEXTURE_TYPE::DIFFUSE;
	}
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

void CAIMaterial::Add_ReTexture(const string& fileDirectory, const string& base, const string& typeAdd, TEXTURE_TYPE type)
{
	/*Normal*/
	string Texturekey = base;
	Texturekey += typeAdd;

	string filePath = fileDirectory + "\\" + Texturekey+".png";
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
