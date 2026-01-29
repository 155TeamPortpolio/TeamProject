#include "pch.h"
#include "MaterialAdjust.h"
#include "MaterialData.h"
#include "Helper_Func.h"

CMaterialAdjust::CMaterialAdjust()
{
}

CMaterialAdjust::CMaterialAdjust(const CMaterialAdjust& rhs)
	:CGameObject(rhs)
{
}

HRESULT CMaterialAdjust::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	return S_OK;
}

HRESULT CMaterialAdjust::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);
	return S_OK;
}

void CMaterialAdjust::Priority_Update(_float dt)
{
}

void CMaterialAdjust::Update(_float dt)
{
}

void CMaterialAdjust::Late_Update(_float dt)
{
}

void CMaterialAdjust::Render_GUI()
{
	if (ImGui::Button("OpenTab"))
		isTabOpen = !isTabOpen;
	if (ImGui::Button("Add_File"))
	{
		OriginPath = Helper::OpenFile_Dialogue();
		if (filesystem::path(OriginPath).extension() != ".mat") {
			OriginPath = {};
			return;
		}

		m_Files = Load_Materials(OriginPath);
	}
	if (ImGui::Button("Clear"))
	{
		OriginPath = {};
		m_Files = {};
	}
	if (isTabOpen) {
		Render_AdjustTab();
	}
}

void CMaterialAdjust::Render_AdjustTab()
{
	ImGui::SetNextWindowSize(ImVec2(600, 600), ImGuiCond_FirstUseEver);
	ImGui::Begin("Adjust_Material", &isTabOpen, ImGuiWindowFlags_NoCollapse);
	Render_FileHierarchy();
	if (ImGui::Button("Save_File"))
	{
		Save_Materials();
	}
	ImGui::End();
}

void CMaterialAdjust::Render_FileHierarchy()
{

	MaterialFile& file = m_Files;
	string fileLabel = string(file.header.materialFileKey) + " (Materials: " + std::to_string(file.materials.size()) + ")";
	if (ImGui::TreeNode(fileLabel.c_str()))
	{
		// Material 단위
		for (size_t m = 0; m < file.materials.size(); ++m)
		{
			MaterialInfo& mat = file.materials[m];
			string subSet = "subSet : " + string(mat.header.materialDataKey) + "##" + std::to_string(m);
			if (ImGui::TreeNode(subSet.c_str()))
			{
				ImGui::InputText("Shader Key", mat.header.ShaderKey, IM_ARRAYSIZE(mat.header.ShaderKey));
				ImGui::InputText("Shader Pass", mat.header.passConstant, IM_ARRAYSIZE(mat.header.passConstant));

				// TextureType 단위
				for (size_t t = 0; t < mat.textureTypes.size(); ++t)
				{
					TextureFile& texFile = mat.textureTypes[t];

					string typeLabel = ConvertToConstant(static_cast<TEXTURE_TYPE>(texFile.header.typeID)) + "##" + std::to_string(t);

					if (ImGui::TreeNode(typeLabel.c_str()))
					{
						ImGui::Text("Texture Count: %u", texFile.header.TextureCount);

						// TextureInfo 단위
						for (size_t i = 0; i < texFile.textures.size(); ++i)
						{
							TextureInfo& info = texFile.textures[i];
							string texLabel = std::string(info.header.TextureKey) + "##" + std::to_string(i);

							if (ImGui::Selectable(texLabel.c_str()))
							{
								m_SelectedMaterial = m;
								m_SelectedType = t;
								m_SelectedTexture = i;
							}
						}

						ImGui::TreePop();
					}
				}

				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}

}

MaterialFile CMaterialAdjust::Load_Materials(string path)
{

	ifstream ifs;
	ifs.open(path);

	if (!ifs.is_open()) {
		MSG_BOX("Wrong File path.  :Load_MaterialFromFile ");
		return MaterialFile{};
	}

	MaterialFile file = {  };
	ifs.read(reinterpret_cast<char*>(&file.header), sizeof(MATERIAL_FILE_HEADER));

	string filePath = path;
	string fileDirectory = filesystem::path(filePath).parent_path().string() + "/";

	for (size_t i = 0; i < file.header.MaterialDataCount; i++)
	{
		MaterialInfo info = {};
		ifs.read(reinterpret_cast<char*>(&info.header), sizeof(MATERIAL_INFO_HEADER));

		for (size_t i = 0; i < info.header.TextureTypeCount; i++)
		{
			TextureFile Texfile = {};
			ifs.read(reinterpret_cast<char*>(&Texfile.header), sizeof(TEXTURE_FILE_HEADER));
			for (size_t i = 0; i < Texfile.header.TextureCount; i++)
			{
				TextureInfo Texinfo = {};
				ifs.read(reinterpret_cast<char*>(&Texinfo.header), sizeof(TEXTURE_INFO_HEADER));
				Texfile.textures.push_back(Texinfo);
			}
			info.textureTypes.push_back(Texfile);
		}

		file.materials.push_back(info);
	}

	return file;
}

HRESULT CMaterialAdjust::Save_Materials()
{
	ofstream ofs(OriginPath, ios::binary);
	if (!ofs.is_open()) {
		MSG_BOX("Failed to open material file for writing.");
		return E_FAIL;
	}

	MATERIAL_FILE_HEADER fileHeader = m_Files.header;
	ofs.write(reinterpret_cast<const char*>(&fileHeader), sizeof(fileHeader));

	for (size_t i = 0; i < m_Files.materials.size(); i++)
	{
		MaterialInfo matInfo = m_Files.materials[i];
		MATERIAL_INFO_HEADER matHeader = matInfo.header;
		ofs.write(reinterpret_cast<const char*>(&matHeader), sizeof(MATERIAL_INFO_HEADER));

		for (size_t j = 0; j < matInfo.textureTypes.size(); j++)
		{
			TextureFile texFile = matInfo.textureTypes[j];
			TEXTURE_FILE_HEADER texHeader = texFile.header;
			ofs.write(reinterpret_cast<const char*>(&texHeader), sizeof(TEXTURE_FILE_HEADER));

			for (size_t k = 0;  k < texFile.textures.size();  k++)
			{
				TextureInfo texInfo = texFile.textures[k];
				TEXTURE_INFO_HEADER svInfo = texInfo.header;
				ofs.write(reinterpret_cast<const char*>(&svInfo), sizeof(TEXTURE_INFO_HEADER));
			}
		}
	}
	ofs.close();

	m_Files = {};
	return S_OK;
}


CMaterialAdjust* CMaterialAdjust::Create()
{
	CMaterialAdjust* instance = new CMaterialAdjust();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CMaterialAdjust");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CMaterialAdjust::Clone(INIT_DESC* pArg)
{
	CMaterialAdjust* instance = new CMaterialAdjust(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CMaterialAdjust");
		Safe_Release(instance);
	}

	return instance;
}

void CMaterialAdjust::Free()
{
	__super::Free();

}


string CMaterialAdjust::ConvertToConstant(TEXTURE_TYPE eType)
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

	case Engine::TEXTURE_TYPE::NOISE:
		return "NoiseTexture";

	case Engine::TEXTURE_TYPE::DISSOLVE:
		return "DissolveTexture";

	case Engine::TEXTURE_TYPE::ALPHA_MASK:
		return "AlphaMaskTexture";

	case Engine::TEXTURE_TYPE::DISTORTION:
		return "DistortionTexture";

	default:
		break;
	}
	return string();
}