#include "pch.h"
#include "EffectContainer_Edit.h"
#include "EffectNode.h"
#include "Texture.h"
#include "GameInstance.h"
#include "SpriteNode_Edit.h"
#include "ObjectContainer.h"

CEffectContainer_Edit::CEffectContainer_Edit()
    :CEffectContainer()
{
}

CEffectContainer_Edit::CEffectContainer_Edit(const CEffectContainer_Edit& rhs)
    :CEffectContainer(rhs)
{
}

HRESULT CEffectContainer_Edit::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    return S_OK;
}

HRESULT CEffectContainer_Edit::Initialize(INIT_DESC* pArg)
{
   // __super::Initialize(pArg);

	LoadTextureFromDirectory("../Bin/TestTexture");
    return S_OK;
}

void CEffectContainer_Edit::Awake()
{
}

void CEffectContainer_Edit::Priority_Update(_float dt)
{
}

void CEffectContainer_Edit::Update(_float dt)
{
    __super::Update(dt);
}

void CEffectContainer_Edit::Late_Update(_float dt)
{
}

void CEffectContainer_Edit::Render_GUI()
{
    ImGui::PushID(this);
	DisplayAllTextures();
	Play();
    AddNode();
	RemoveLastNode();

    CGameObject::Render_GUI();
    for (const auto& node : m_Nodes)
        node->Render_GUI();
    ImGui::PopID();
}

CEffectContainer_Edit* CEffectContainer_Edit::Create()
{
    CEffectContainer_Edit* instance = new CEffectContainer_Edit();

    if (FAILED(instance->Initialize_Prototype()))
    {
        MSG_BOX("Object Clone Failed : CEffectContainer_Edit");
        Safe_Release(instance);
    }

    return instance;
}

CGameObject* CEffectContainer_Edit::Clone(INIT_DESC* pArg)
{
    CEffectContainer_Edit* instance = new CEffectContainer_Edit(*this);

    if (FAILED(instance->Initialize(pArg)))
    {
        MSG_BOX("Object Clone Failed : CEffectContainer_Edit");
        Safe_Release(instance);
    }

    return instance;
}

void CEffectContainer_Edit::Free()
{
    __super::Free();

	for (auto& [key, texture] : m_Textures)
		Safe_Release(texture);
}

void CEffectContainer_Edit::Play()
{
	if (ImGui::Button("Play"))
	{
		for (auto& node : m_Nodes)
			node->Play();
	}
}

void CEffectContainer_Edit::AddNode()
{
	CGameObject* pNode = nullptr;

    if (ImGui::Button("Add Sprite Node"))
    {
		CSpriteNode_Edit::SPRITE_NODE_EDIT_DESC* pDesc = new CSpriteNode_Edit::SPRITE_NODE_EDIT_DESC;
		pDesc->pContext = &m_Context;

		pNode = Builder::Create_Object({ "EffectEdit_Level","Proto_GameObject_SpriteNode" }).Add_ObjDesc(pDesc).Build("SpriteNode");
        m_Nodes.push_back(static_cast<CEffectNode*>(pNode));
    }
	if (ImGui::Button("Add Particle Node"))
	{
		pNode = Builder::Create_Object({ "EffectEdit_Level","Proto_GameObject_ParticleNode" }).Build("ParticleNode");
		m_Nodes.push_back(static_cast<CEffectNode*>(pNode));
	}
	if (ImGui::Button("Add Mesh Node"))
	{

	}

	if (pNode)
		Get_Component<CObjectContainer>()->Add_Child(pNode);
}

void CEffectContainer_Edit::RemoveLastNode()
{
	if (m_Nodes.empty())
		return;

	if (ImGui::Button("Remove Last Node"))
	{
		CGameInstance::GetInstance()->Get_ObjectMgr()->Remove_Object(m_Nodes.back());
		m_Nodes.pop_back();

		m_iNumNodes = m_Nodes.size();
	}
}

void CEffectContainer_Edit::ContextClear()
{
	if (ImGui::Button("Context Clear"))
	{
		m_SelectIndices.clear();
		m_Context.TextureTags.clear();
		m_Context.Textures.clear();
	}
}

void CEffectContainer_Edit::LoadTextureFromDirectory(const string& dirPath)
{
	namespace fs = std::filesystem;
	ID3D11Device* pDevice = CGameInstance::GetInstance()->Get_Device();
	auto resource = CGameInstance::GetInstance()->Get_ResourceMgr();

	for (auto& entry : fs::recursive_directory_iterator(dirPath))
	{
		if (!entry.is_regular_file())
			continue;

		string path = entry.path().string();
		string textureKey = entry.path().filename().string();
		resource->Add_ResourcePath(textureKey, path);
		auto tex = resource->Load_Texture(G_GlobalLevelKey, textureKey);

		if (!tex)
		{
			MSG_BOX("Failed to load : %s", path.c_str());
			continue;
		}
		
		m_Textures.emplace(textureKey, tex);
		Safe_AddRef(tex);
	}

	m_Context.pAllTextures = &m_Textures;
}

void CEffectContainer_Edit::DisplayAllTextures()
{
	ImGui::Begin("Texture");
	float avail = ImGui::GetWindowSize().x;
	int   cols = (std::max)(1, (int)((avail + 8.f) / (128.f + 8.f)));
	float cellW = (avail - 8.f * (cols - 1)) / cols;
	float cellH = cellW;

	_uint idx = 0;
	_uint count = 0;
	for (auto& pair : m_Textures)
	{
		if (idx % cols != 0) ImGui::SameLine();
		ImGui::PushID(idx);

		ImGui::BeginGroup();
		ImVec2 p0 = ImGui::GetCursorScreenPos();
		ImVec2 size(cellW, cellH);
		_bool isPressed = ImGui::InvisibleButton(("imgbtn" + std::to_string(idx)).c_str(), size);

		_bool isLeftClick = ImGui::IsItemClicked(ImGuiMouseButton_Left);
		_bool isRightClick = ImGui::IsItemClicked(ImGuiMouseButton_Right);

		_float iw = 1.f;
		_float ih = 1.f;
		_float scale = (std::min)(size.x / iw, size.y / ih);
		ImVec2 drawSize(iw * scale, ih * scale);
		ImVec2 offset((size.x - drawSize.x) * 0.5f, (size.y - drawSize.y) * 0.5f);

		ImTextureID texID = (ImTextureID)pair.second->Get_SRV();
		ImGui::GetWindowDrawList()->AddImage(texID,
			ImVec2(p0.x + offset.x, p0.y + offset.y),
			ImVec2(p0.x + offset.x + drawSize.x, p0.y + offset.y + drawSize.y));

		for (const auto& select : m_SelectIndices)
		{
			_bool isHovered = ImGui::IsItemHovered();
			_bool isSelected = (select == static_cast<_int>(idx));
			if (isHovered || isSelected)
			{
				ImU32 col = isSelected ? IM_COL32(80, 170, 255, 255) : IM_COL32(255, 255, 255, 180);

				ImVec2 rMin = ImGui::GetItemRectMin();
				ImVec2 rMax = ImGui::GetItemRectMax();
				ImGui::GetWindowDrawList()->AddRect(rMin, rMax, col, 6.0f, 0, 2.0f);
			}
		}

		if (isLeftClick)
		{
			auto iter = std::find(m_SelectIndices.begin(), m_SelectIndices.end(), idx);
			if (iter == m_SelectIndices.end())
			{
				m_SelectIndices.push_back(idx);
				m_Context.TextureTags.push_back(pair.first);
				m_Context.Textures.push_back(pair.second);
			}
		}

		ImGui::TextWrapped("%s", pair.first.c_str());
		ImGui::EndGroup();
		ImGui::PopID();
		++idx;
	}
	ImGui::End();
}
