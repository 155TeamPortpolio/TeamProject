#include "Material.h"
#include "GameInstance.h"
#include "IResourceService.h"
#include "MaterialInstance.h"

CMaterial::CMaterial()
{
}

CMaterial::CMaterial(const CMaterial& rhs)
	:CComponent(rhs)
{
	for (auto& instance : rhs.m_MaterialInstances) {
		CMaterialInstance* cloned = instance->Clone();
		m_MaterialInstances.emplace_back(move(cloned));
	}
}

HRESULT CMaterial::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMaterial::Initialize(COMPONENT_DESC* pArg)
{
	return S_OK;
}


HRESULT CMaterial::Link_Material(const string& levelKey, const string& materialKey)
{
	for (auto& data : m_MaterialInstances)
		Safe_Release(data);

	m_MaterialInstances = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_MaterialFromFile(levelKey, materialKey);
	return S_OK;
}

HRESULT CMaterial::Insert_MaterialInstance(CMaterialInstance* pInstance, _uint* outIndex)
{
	if(pInstance == nullptr)
		return E_FAIL;

	m_MaterialInstances.push_back(pInstance);
	//Safe_AddRef(pInstance);
	if(outIndex)
		*outIndex = m_MaterialInstances.size() - 1;

	return S_OK;
}

CShader* CMaterial::Get_Shader(_uint subsetIndex)
{
	if (subsetIndex >= m_MaterialInstances.size()) return nullptr;

	return m_MaterialInstances[subsetIndex]->Get_Shader();
}

_uint CMaterial::Get_ShaderID(_uint subsetIndex)
{
	if (subsetIndex >= m_MaterialInstances.size()) return 0;

	return m_MaterialInstances[subsetIndex]->Get_ShaderID();
}

_uint CMaterial::Get_MaterialDataID(_uint subsetIndex)
{
	if (subsetIndex >= m_MaterialInstances.size()) return 0;

	return m_MaterialInstances[subsetIndex]->Get_MaterialDataID();
}

void CMaterial::Apply_Material(ID3D11DeviceContext* pContext, _uint subsetIndex)
{
	if (subsetIndex >= m_MaterialInstances.size()) return;
	m_MaterialInstances[subsetIndex]->ApplyData(pContext);
}

CMaterialInstance* CMaterial::Get_MaterialInstanceByName(const string& MaterialName)
{
	if (m_MaterialInstances.empty()) 
		return nullptr;

	auto iter = find_if(m_MaterialInstances.begin(), m_MaterialInstances.end(),
		[&](CMaterialInstance* pInstance)->bool{
			return pInstance->Get_MaterialName() == MaterialName;
		});

	if (iter == m_MaterialInstances.end()) return nullptr;

	return *iter;
}

CMaterialInstance* CMaterial::Get_MaterialInstance(_uint Index)
{
	if (m_MaterialInstances.empty())
		return nullptr;

	if (Index >= m_MaterialInstances.size())
		return nullptr;

	return m_MaterialInstances[Index];
}

const string& CMaterial::GetPassConstant(_uint subsetIndex)
{
	return m_MaterialInstances[subsetIndex]->Get_PassConstant();
}

CMaterialInstance* CMaterial::Find_MaterialByName(const string& MaterialName)
{
	auto iter = find_if(m_MaterialInstances.begin(), m_MaterialInstances.end(), [&MaterialName](CMaterialInstance* pInstance)->bool {
			return pInstance->Get_MaterialName() == MaterialName;
		});

	if (iter == m_MaterialInstances.end()) return nullptr;

	return *iter;
}

CMaterial* CMaterial::Create()
{
	CMaterial* instance = new CMaterial;
	if (FAILED(instance->Initialize_Prototype())) {
		Safe_Release(instance);
	}
	return instance;
}

CComponent* CMaterial::Clone()
{
	CMaterial* instance = new CMaterial(*this);
	return instance;
}

void CMaterial::Free()
{
	for (auto& data : m_MaterialInstances)
		Safe_Release(data);

	m_MaterialInstances.clear();
}

void CMaterial::Render_GUI()
{
	if (m_MaterialInstances.empty())
		return;

	ImGui::SeparatorText("Material");
	float childWidth = ImGui::GetContentRegionAvail().x;
	const float textLineHeight = ImGui::GetTextLineHeightWithSpacing();
	const float childHeight = (m_MaterialInstances.size() * 2) + (ImGui::GetStyle().WindowPadding.y * 4);
	
	if(ImGui::Button("Material Tabs")) {
		m_bMaterialTabOpen = true;
	}
	
	if(m_bMaterialTabOpen){
		ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Materials", &m_bMaterialTabOpen, ImGuiWindowFlags_NoCollapse))
		{
		if (ImGui::BeginTabBar("##MaterialTabs"))
		{
			for (int i = 0; i < m_MaterialInstances.size(); ++i)
			{
				CMaterialInstance* hMaterial = m_MaterialInstances[i];
				if (ImGui::BeginTabItem(hMaterial->Get_MaterialName().c_str()))
				{
					hMaterial->Render_GUI();
					ImGui::EndTabItem();
				}
			}
		}
		ImGui::EndTabBar();
	}
	ImGui::End();
	}
}