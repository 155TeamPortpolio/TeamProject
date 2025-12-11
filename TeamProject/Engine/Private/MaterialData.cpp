#include "Engine_Defines.h"
#include "MaterialData.h"
#include "GameInstance.h"
#include "IResourceService.h"
#include "Texture.h"
#include "MaterialInstance.h"
#include "Sprite2D.h"
_uint CMaterialData::s_NextID = 1;


CMaterialData::CMaterialData()
	:m_MaterialDataID(s_NextID++)
{
	if (s_NextID > 4000) {
		int i = 0;
	}
	m_DefaultMaterialConstant.vMtrlAmbient = { 0.5f,0.5f,0.5f,1.f };
}

CMaterialData::~CMaterialData()
{
}

HRESULT CMaterialData::Initialize(const string& levelKey, ifstream& ifs, const string& directory)
{
	MATERIAL_INFO_HEADER infoHeader = {};
	string parentFolder = filesystem::path(directory).parent_path().string();
	string ParentName = filesystem::path(parentFolder).filename().string();
	//string parentFolder = filesystem::path(directory).di.string();
	
	ifs.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));

	m_DefaultMaterialConstant = infoHeader.materialConstant;
	m_passConstant = infoHeader.passConstant;
	m_MaterialKey = infoHeader.materialDataKey;

	Link_Shader(levelKey, infoHeader.ShaderKey);

	for (size_t i = 0; i < infoHeader.TextureTypeCount; i++)
	{
		TEXTURE_FILE_HEADER textureHeader = {};
		ifs.read(reinterpret_cast<char*>(&textureHeader), sizeof(TEXTURE_FILE_HEADER));
		for (size_t i = 0; i < textureHeader.TextureCount; i++)
		{
			TEXTURE_INFO_HEADER textureInfoHeader = {};
			ifs.read(reinterpret_cast<char*>(&textureInfoHeader), sizeof(TEXTURE_INFO_HEADER));
			CGameInstance::GetInstance()->Get_ResourceMgr()->Add_ResourcePath(
				ParentName+ "_" +textureInfoHeader.TextureKey,
				directory + textureInfoHeader.TextureKey);

			Link_Texture(levelKey, ParentName + "_" + textureInfoHeader.TextureKey, static_cast<TEXTURE_TYPE>(textureHeader.typeID));
		}
	}

	return S_OK;
}



void CMaterialData::ApplyData(ID3D11DeviceContext* pContext, const vector<_uint>& TextureIndexs)
{
	if(TextureIndexs.size() < MAX_TEXTURE_TYPE_VALUE) return;

	SHADER_PARAM param = {};
	param.typeName = "Texture2D";
	param.iSize = 0;

	for (_uint i = 0; i < MAX_TEXTURE_TYPE_VALUE; ++i)
	{
		auto it = m_Textures.find(static_cast<TEXTURE_TYPE>(i));

		if (it != m_Textures.end())
		{
			if (it->second.empty())
				continue;
			_uint texIndex = TextureIndexs[i];
			if (texIndex < it->second.size())
				param.pData = it->second[texIndex]->Get_SRV();
			else
				param.pData = it->second[0]->Get_SRV();
		}
		else
		{
			param.pData = nullptr;
		}

		m_pShader->Bind_Value(ConvertToConstant(static_cast<TEXTURE_TYPE>(i)), param);
	}
}

HRESULT CMaterialData::Set_MaterialConstantBuffer(ID3D11Buffer* pCBuffer)
{
	return m_pShader->SetConstantBuffer("MaterialBuffer",pCBuffer);
}

_bool CMaterialData::Has_Texture(TEXTURE_TYPE eType)
{
	return !m_Textures[eType].empty();
}

void CMaterialData::Render_GUI()
{
	if (m_Textures.empty())
		return;

	for (auto& pair : m_Textures) {
		ImGui::Text(ConvertToConstant(pair.first).c_str());
		const auto& vector = pair.second;

		for (size_t i = 0; i < vector.size(); i++)
		{
			vector[i]->Render_GUI(64);
		}
	}

}

void CMaterialData::Render_GUI( vector<_uint>& TextureIndexs)
{
	if (m_Textures.empty())
		return;

	for (auto& pair : m_Textures) {
		ImGui::SeparatorText(ConvertToConstant(pair.first).c_str()); //텍스처 타입 콘스탄트로
		_uint& CurrentIndex = TextureIndexs[static_cast<_uint>(pair.first)];
		const auto& vector = pair.second; //텍스처 타입이 있는 벡터
		for (size_t i = 0; i < vector.size(); i++)
		{
			string ButtonID = "##" + vector[i]->Get_Key() + "_" + to_string(i);
			if (ImGui::ImageButton(
				ButtonID.c_str(),
				(ImTextureID)vector[i]->Get_SRV(),  
				ImVec2(64, 64)                  
			)) 
			{
				CurrentIndex = i;
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
HRESULT CMaterialData::Link_Texture(const string& levelKey, const string& textureKey, TEXTURE_TYPE eType)
{
	auto& textureList = m_Textures[eType];
	for (auto* pTex : textureList)
	{
		if (pTex && pTex->Get_Key() == textureKey)
		{
			return S_OK;
		}
	}

	// 새로운 텍스처 로드
	CTexture* pTexture = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_Texture(levelKey, textureKey);
	if (!pTexture)
	{
		MSG_BOX("There is no Texture Key : Link_Texture");
		return E_FAIL;
	}

	textureList.push_back(pTexture);
	Safe_AddRef(pTexture);

	return S_OK;
}

HRESULT CMaterialData::Link_Shader(const string& levelKey, const string& shaderKey)
{
	Safe_Release(m_pShader);
	m_pShader = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_Shader(levelKey, shaderKey);

	if (!m_pShader)
		return E_FAIL;

	Safe_AddRef(m_pShader);
	return S_OK;
}


string CMaterialData::ConvertToConstant(TEXTURE_TYPE eType)
{
	switch (eType)
	{
	case Engine::TEXTURE_TYPE::NONE:
		break;
	case Engine::TEXTURE_TYPE::DIFFUSE:
		return "DiffuseTexture";

	case Engine::TEXTURE_TYPE::SPECULAR:
		return "SpecularTexture";

	case Engine::TEXTURE_TYPE::AMBIENT:
		return "AmbientTexture";

	case Engine::TEXTURE_TYPE::EMISSIVE:
		return "EmissiveTexture";

	case Engine::TEXTURE_TYPE::HEIGHT:
		return "HeightTexture";

	case Engine::TEXTURE_TYPE::NORMALS:
		return "NormalTexture";

	case Engine::TEXTURE_TYPE::SHININESS:
		return "ShinessTexture";

	case Engine::TEXTURE_TYPE::OPACITY:
		return "OpacityTexture";

	case Engine::TEXTURE_TYPE::DISPLACEMENT:
		return "DisplacementTexture";

	case Engine::TEXTURE_TYPE::LIGHTMAP:
		return "LightTexture";

	case Engine::TEXTURE_TYPE::REFLECTION:
		return "ReflectionTexture";

	case Engine::TEXTURE_TYPE::BASE_COLOR:
		return "BaseColorTexture";

	case Engine::TEXTURE_TYPE::NORMAL_CAMERA:
		return "NormalCameraTexture";

	case Engine::TEXTURE_TYPE::EMISSION_COLOR:
		return "EmmisionTexture";

	case Engine::TEXTURE_TYPE::METALNESS:
		return "MetalnessTexture";

	case Engine::TEXTURE_TYPE::DIFFUSE_ROUGHNESS:
		return "DiffuseRoughness";

	case Engine::TEXTURE_TYPE::AMBIENT_OCCLUSION:
		return "AmbientOcclusion";

	case Engine::TEXTURE_TYPE::SHEEN:
		return "SheenTexture";

	case Engine::TEXTURE_TYPE::CLEARCOAT:
		return "ClearCoatTexture";

	case Engine::TEXTURE_TYPE::TRANSMISSION:
		return "TransmissionTexture";

	case Engine::TEXTURE_TYPE::MAYA_BASE:
		return "MayaBaseTexture";

	case Engine::TEXTURE_TYPE::MAYA_SPECULAR:
		return "MayaSpecularTexture";

	case Engine::TEXTURE_TYPE::MAYA_SPECULAR_COLOR:
		return "MayaSpecularColorTexture";

	case Engine::TEXTURE_TYPE::MAYA_SPECULAR_ROUGHNESS:
		return "MayaSpecularRoughnessTexture";

	case Engine::TEXTURE_TYPE::ANISOTROPY:
		return "AnisotropyTexture";

	case Engine::TEXTURE_TYPE::GLTF_METALLIC_ROUGHNESS:
		return "GltfMetalicRoughnessTexture";

	default:
		break;
	}
	return string();
}


CMaterialData* CMaterialData::Create(const string& levelKey, ifstream& ifs, const string& directory)
{
	CMaterialData* instance = new CMaterialData();
	if (FAILED(instance->Initialize( levelKey, ifs, directory))) {
		Safe_Release(instance);
	}
	return instance;
}

CMaterialData* CMaterialData::Create(const string& materialKey, const string& DefualtpassConstant)
{
	CMaterialData* instance = new CMaterialData();
	instance->m_MaterialKey = materialKey;
	instance->m_passConstant = DefualtpassConstant;
	return instance;
}

void CMaterialData::Free()
{
	Safe_Release(m_pShader);

	for (auto& pair : m_Textures)
		for(auto& tex : pair.second)
			Safe_Release(tex);

	m_Textures.clear();
}
