#include "AI_STModel.h"
#include "AIModelData.h"
#include "Helper_Func.h"
#include "AIMesh.h"
CAI_STModel::CAI_STModel()
	: CStaticModel{}
{
}

CAI_STModel::CAI_STModel(const CAI_STModel& rhs)	
	: CStaticModel(rhs)
{
}

HRESULT CAI_STModel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAI_STModel::Initialize(COMPONENT_DESC* pArg)
{
	return S_OK;
}

void CAI_STModel::Render_GUI()
{
	auto data = dynamic_cast<CAIModelData*>(m_pData);

	auto ToggleMarkedMeshes = [&](const vector<_uint>& indices)
		{
			for (_uint index : indices)
				m_DrawableMeshes[index] = !m_DrawableMeshes[index];
		};

	auto IsCleared = [&](const vector<_uint>& indices) -> bool
		{
			if (indices.empty())
				return false;

			for (_uint index : indices)
			{
				if (m_DrawableMeshes[index])
					return false;
			}
			return true;
		};

	auto DrawToggleButton = [&](const char* label, const vector<_uint>& indices)
		{
			bool cleared = IsCleared(indices);

			if (cleared)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.12f, 0.12f, 1.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.70f, 0.16f, 0.16f, 1.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.80f, 0.18f, 0.18f, 1.f));
			}

			if (ImGui::Button(label))
				ToggleMarkedMeshes(indices);

			if (cleared)
				ImGui::PopStyleColor(3);
		};

	if (data)
	{
		DrawToggleButton("Clear Proxy Mesh", data->Get_ProxyIndex());
		ImGui::SameLine();
		DrawToggleButton("Clear LOD0 Mesh", data->Get_LOD0_Index());

		DrawToggleButton("Clear LOD1 Mesh", data->Get_LOD1_Index());
		ImGui::SameLine();
		DrawToggleButton("Clear LOD2 Mesh", data->Get_LOD2_Index());

		DrawToggleButton("Clear LOD3 Mesh", data->Get_LOD3_Index());
		ImGui::SameLine();
		DrawToggleButton("Clear Eff  Mesh", data->Get_Eff_Index());
	}

	__super::Render_GUI();
}

HRESULT CAI_STModel::Load_AIModel(const aiScene* pAIScene, string fileName)
{
	if (nullptr == pAIScene)
		return E_FAIL;
	Release_Mesh();
	_uint meshNum = pAIScene->mNumMeshes;
	m_DrawableMeshes.resize(meshNum, true);
	m_fileName = fileName;

	if (FAILED(Ready_AIModelData(pAIScene)))
		return E_FAIL;
	// -----------------------------------------------
	auto data = dynamic_cast<CAIModelData*>(m_pData);

	auto ApplyDefaultClears = [&](const vector<_uint>& indices)
		{
			if (indices.empty())
				return;

			for (_uint index : indices)
				m_DrawableMeshes[index] = false;
		};

	if (data)
	{
		ApplyDefaultClears(data->Get_ProxyIndex());
		//ApplyDefaultClears(data->Get_LOD0_Index());
		//ApplyDefaultClears(data->Get_LOD1_Index());
		//ApplyDefaultClears(data->Get_LOD2_Index());
		//ApplyDefaultClears(data->Get_LOD3_Index());
		ApplyDefaultClears(data->Get_Eff_Index());
	}
	return S_OK;
}

HRESULT CAI_STModel::Save_Model(const string& SavePath, _fmatrix WorldMatrix)
{
	string path = Helper::SaveFileDialogByWinAPI(m_fileName, "model");
	filesystem::path directory(path);
	ofstream ofs(path.c_str(), ios::binary);
	if (!ofs.is_open())
		return E_FAIL;

	MODEL_FILE_HEADER fileHeader = {};
	fileHeader.isAnimate = false;
	fileHeader.MeshCount = m_pData->Get_MeshCount();
	strcpy_s(fileHeader.ModelKey, sizeof(fileHeader.ModelKey), m_fileName.data());
	ofs.write(reinterpret_cast<char*>(&fileHeader), sizeof(MODEL_FILE_HEADER));

	static_cast<CAIModelData*>(m_pData)->Save_File(ofs, WorldMatrix);
	ofs.close();
	return S_OK;
}

HRESULT CAI_STModel::Ready_AIModelData(const aiScene* pAIScene)
{
	m_pData = CAIModelData::Create(MESH_TYPE::NONANIM, pAIScene);
	if (nullptr == m_pData)
		return E_FAIL;

	return S_OK;
}

void CAI_STModel::Clear_Proxy()
{
	auto proxy = dynamic_cast<CAIModelData*>(m_pData)->Get_ProxyIndex();

	for (_uint Index : proxy) {
		m_DrawableMeshes[Index] = !m_DrawableMeshes[Index];
	}
}

CAIModelData* CAI_STModel::Get_AIModelData()
{
	return static_cast<CAIModelData*>(m_pData);
}


HRESULT CAI_STModel::Release_Mesh()
{
	Safe_Release(m_pData);
	vector<bool> v;
	m_DrawableMeshes.swap(v);

	return S_OK;
}
CAI_STModel* CAI_STModel::Create()
{
	CAI_STModel* instance = new CAI_STModel();

	if (FAILED(instance->Initialize_Prototype())) {
		MSG_BOX("CAI_STModel Create Failed : CAI_STModel");
		Safe_Release(instance);
	}

	return instance;
}

CComponent* CAI_STModel::Clone()
{
	CAI_STModel* instance = new CAI_STModel(*this);
	return instance;
}

void CAI_STModel::Free()
{
	__super::Free();
}
