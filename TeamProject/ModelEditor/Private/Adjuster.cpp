#include "pch.h"
#include "Adjuster.h"
#include "MaterialData.h"
#include "ModelData.h"
#include "Helper_Func.h"
#include "GameInstance.h"
#include "Texture.h"

#include <fstream>
#include <filesystem>
#include <unordered_map>

static void CopyToFixedChar(char* dst, size_t dstCount, const string& src)
{
    if (!dst || dstCount == 0) return;
    memset(dst, 0, dstCount);

    const size_t copyCount = min(dstCount - 1, src.size());
    memcpy(dst, src.data(), copyCount);
    dst[copyCount] = '\0';
}

static string FixedCharToString(const char* src, size_t srcCount)
{
    if (!src || srcCount == 0) return {};
    size_t length = 0;
    while (length < srcCount && src[length] != '\0') ++length;
    return string(src, src + length);
}

static void RemapModelMaterialIndices(vector<MeshView>& meshViews, const vector<int>& oldToNew)
{
    for (auto& meshView : meshViews)
    {
        const int oldIndex = meshView.materialIndex;
        if (oldIndex < 0) continue;
        if (oldIndex >= (int)oldToNew.size()) continue;

        const int newIndex = oldToNew[oldIndex];
        meshView.materialIndex = newIndex;
        meshView.meshHeaderRaw.MaterialIndex = (_uint)max(newIndex, 0);
    }
}

CAdjuster::CAdjuster() {}
CAdjuster::CAdjuster(const CAdjuster& rhs) : CGameObject(rhs) {}

HRESULT CAdjuster::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    return S_OK;
}

HRESULT CAdjuster::Initialize(INIT_DESC* initDesc)
{
    __super::Initialize(initDesc);
    return S_OK;
}

void CAdjuster::Priority_Update(_float) {}
void CAdjuster::Update(_float) {}
void CAdjuster::Late_Update(_float) {}

void CAdjuster::Free()
{
    if (m_PreviewTex)
        Safe_Release(m_PreviewTex);

    __super::Free();
}

// -------------------- IO --------------------

HRESULT CAdjuster::Load_MaterialFile(const string& fileKey, const string& path, MaterialFile& outFile)
{
    ifstream inputStream(path, ios::binary);
    if (!inputStream.is_open())
        return E_FAIL;

    outFile = {};

    inputStream.read(reinterpret_cast<char*>(&outFile.header), sizeof(MATERIAL_FILE_HEADER));
    if (!inputStream.good())
        return E_FAIL;

    const filesystem::path matDir = filesystem::path(path).parent_path();

    outFile.materials.clear();
    outFile.materials.reserve(outFile.header.MaterialDataCount);

    for (size_t materialIndex = 0; materialIndex < outFile.header.MaterialDataCount; ++materialIndex)
    {
        MaterialInfo materialInfo{};
        inputStream.read(reinterpret_cast<char*>(&materialInfo.header), sizeof(MATERIAL_INFO_HEADER));
        if (!inputStream.good())
            return E_FAIL;

        materialInfo.textureTypes.clear();
        materialInfo.textureTypes.reserve(materialInfo.header.TextureTypeCount);

        for (size_t typeIndex = 0; typeIndex < materialInfo.header.TextureTypeCount; ++typeIndex)
        {
            TextureFile textureFile{};
            inputStream.read(reinterpret_cast<char*>(&textureFile.header), sizeof(TEXTURE_FILE_HEADER));
            if (!inputStream.good())
                return E_FAIL;

            textureFile.textures.clear();
            textureFile.textures.reserve(textureFile.header.TextureCount);

            for (size_t texIndex = 0; texIndex < textureFile.header.TextureCount; ++texIndex)
            {
                TextureInfo textureInfo{};
                inputStream.read(reinterpret_cast<char*>(&textureInfo.header), sizeof(TEXTURE_INFO_HEADER));
                if (!inputStream.good())
                    return E_FAIL;

                const string textureKey =
                    FixedCharToString(textureInfo.header.TextureKey, IM_ARRAYSIZE(textureInfo.header.TextureKey));

                if (!textureKey.empty())
                {
                    const filesystem::path texturePath = matDir / textureKey;
                    ResourceManager()->Add_ResourcePath(textureKey, texturePath.string());
                }

                textureFile.textures.push_back(textureInfo);
            }

            materialInfo.textureTypes.push_back(textureFile);
        }

        outFile.materials.push_back(materialInfo);
    }

    return S_OK;
}

HRESULT CAdjuster::Save_MaterialFile(const string& path, const MaterialFile& file)
{
    ofstream outputStream(path, ios::binary);
    if (!outputStream.is_open())
        return E_FAIL;

    MATERIAL_FILE_HEADER fileHeader = file.header;
    fileHeader.MaterialDataCount = (uint32_t)file.materials.size();
    outputStream.write(reinterpret_cast<const char*>(&fileHeader), sizeof(fileHeader));
    if (!outputStream.good()) return E_FAIL;

    for (size_t materialIndex = 0; materialIndex < file.materials.size(); ++materialIndex)
    {
        const MaterialInfo& materialInfo = file.materials[materialIndex];

        MATERIAL_INFO_HEADER materialHeader = materialInfo.header;
        materialHeader.TextureTypeCount = (uint32_t)materialInfo.textureTypes.size();
        outputStream.write(reinterpret_cast<const char*>(&materialHeader), sizeof(materialHeader));
        if (!outputStream.good()) return E_FAIL;

        for (size_t typeIndex = 0; typeIndex < materialInfo.textureTypes.size(); ++typeIndex)
        {
            const TextureFile& textureFile = materialInfo.textureTypes[typeIndex];

            TEXTURE_FILE_HEADER textureHeader = textureFile.header;
            textureHeader.TextureCount = (uint32_t)textureFile.textures.size();
            outputStream.write(reinterpret_cast<const char*>(&textureHeader), sizeof(textureHeader));
            if (!outputStream.good()) return E_FAIL;

            for (size_t texIndex = 0; texIndex < textureFile.textures.size(); ++texIndex)
            {
                const TextureInfo& textureInfo = textureFile.textures[texIndex];
                outputStream.write(reinterpret_cast<const char*>(&textureInfo.header), sizeof(TEXTURE_INFO_HEADER));
                if (!outputStream.good()) return E_FAIL;
            }
        }
    }

    outputStream.flush();
    return outputStream.good() ? S_OK : E_FAIL;
}

HRESULT CAdjuster::Load_ModelHeaderAndMeshViews(const string& path, CombinedAsset& inOut)
{
    ifstream inputStream(path, ios::binary);
    if (!inputStream.is_open())
        return E_FAIL;

    inOut.modelHeader = {};
    inputStream.read(reinterpret_cast<char*>(&inOut.modelHeader), sizeof(MODEL_FILE_HEADER));
    if (!inputStream.good())
        return E_FAIL;

    inOut.meshes.clear();
    inOut.meshes.reserve(inOut.modelHeader.MeshCount);

    const bool isAnimate = inOut.modelHeader.isAnimate;

    for (int meshIndex = 0; meshIndex < inOut.modelHeader.MeshCount; ++meshIndex)
    {
        const streamoff headerPos = inputStream.tellg();

        MESH_INFO_HEADER meshHeader{};
        inputStream.read(reinterpret_cast<char*>(&meshHeader), sizeof(MESH_INFO_HEADER));
        if (!inputStream.good())
            return E_FAIL;

        MeshView view{};
        view.meshName = string(meshHeader.MeshName);
        view.materialIndex = static_cast<_int>(meshHeader.MaterialIndex);
        view.verticesCount = static_cast<_uint>(meshHeader.VerticesCount);
        view.indicesCount = static_cast<_uint>(meshHeader.IndicesCount);

        view.meshHeaderOffset = headerPos;
        view.meshHeaderRaw = meshHeader;

        if (inOut.reverseMeshOrder)
            inOut.meshes.insert(inOut.meshes.begin(), view);
        else
            inOut.meshes.push_back(view);

        const size_t vertexStride = isAnimate ? sizeof(VTXSKINMESH) : sizeof(VTXMESH);
        const size_t indexStride = 4;

        const size_t vertexBytes = (size_t)meshHeader.VerticesCount * vertexStride;
        const size_t indexBytes = (size_t)meshHeader.IndicesCount * indexStride;
        const size_t offsetBytes = (size_t)meshHeader.offsetCount * sizeof(MESH_OFFSET);

        inputStream.seekg((streamoff)(vertexBytes + indexBytes + offsetBytes), ios::cur);
        if (!inputStream.good())
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CAdjuster::Save_ModelFile_UpdateMaterialIndex(const string& path, const CombinedAsset& inOut)
{
    if (!inOut.loadedModel)
        return E_FAIL;

    fstream io(path, ios::binary | ios::in | ios::out);
    if (!io.is_open())
        return E_FAIL;

    for (const auto& meshView : inOut.meshes)
    {
        if (meshView.meshHeaderOffset <= 0)
            continue;

        MESH_INFO_HEADER headerToWrite = meshView.meshHeaderRaw;
        headerToWrite.MaterialIndex = static_cast<_uint>(max(meshView.materialIndex, 0));

        io.seekp(meshView.meshHeaderOffset, ios::beg);
        if (!io.good())
            return E_FAIL;

        io.write(reinterpret_cast<const char*>(&headerToWrite), sizeof(MESH_INFO_HEADER));
        if (!io.good())
            return E_FAIL;
    }

    io.flush();
    return S_OK;
}

void CAdjuster::Link_ModelToMaterial(CombinedAsset& inOut)
{
    for (auto& meshView : inOut.meshes)
        meshView.linkedMaterial = nullptr;

    if (!inOut.loadedModel || !inOut.loadedMaterial)
        return;

    const size_t materialCount = inOut.materialFile.materials.size();

    for (auto& meshView : inOut.meshes)
    {
        if (meshView.materialIndex < 0)
            continue;

        const size_t idx = (size_t)meshView.materialIndex;
        if (idx >= materialCount)
            continue;

        meshView.linkedMaterial = &inOut.materialFile.materials[idx];
    }
}

void CAdjuster::RebuildMaterialCounts()
{
    MaterialFile& materialFile = m_Combined.materialFile;

    materialFile.header.MaterialDataCount = (uint32_t)materialFile.materials.size();

    for (size_t materialIndex = 0; materialIndex < materialFile.materials.size(); ++materialIndex)
    {
        MaterialInfo& materialInfo = materialFile.materials[materialIndex];
        materialInfo.header.TextureTypeCount = (uint32_t)materialInfo.textureTypes.size();

        for (size_t typeIndex = 0; typeIndex < materialInfo.textureTypes.size(); ++typeIndex)
        {
            TextureFile& textureFile = materialInfo.textureTypes[typeIndex];
            textureFile.header.TextureCount = (uint32_t)textureFile.textures.size();
        }
    }
}

// -------------------- Mapping cache --------------------

void CAdjuster::RebuildMaterialCentricCache()
{
    m_MappingCache = {};

    if (!m_Combined.loadedMaterial)
        return;

    const int materialCount = (int)m_Combined.materialFile.materials.size();
    if (materialCount <= 0)
        return;

    m_MappingCache.meshesByMaterial.resize((size_t)materialCount);
    m_MappingCache.useCount.resize((size_t)materialCount, 0);

    if (!m_Combined.loadedModel)
        return;

    const int meshCount = (int)m_Combined.meshes.size();
    for (int meshIndex = 0; meshIndex < meshCount; ++meshIndex)
    {
        const MeshView& meshView = m_Combined.meshes[(size_t)meshIndex];
        const int matIndex = meshView.materialIndex;
        if (matIndex < 0 || matIndex >= materialCount)
            continue;

        m_MappingCache.meshesByMaterial[(size_t)matIndex].push_back(meshIndex);
        m_MappingCache.useCount[(size_t)matIndex] += 1;
    }
}

void CAdjuster::RequestScrollToMesh(int meshIndex)
{
    m_RequestScrollToMesh = true;
    m_ScrollToMeshIndex = meshIndex;
}

void CAdjuster::RequestScrollToMaterial(int materialIndex)
{
    m_RequestScrollToMaterial = true;
    m_ScrollToMaterialIndex = materialIndex;
}

void CAdjuster::ApplyMeshMaterialMapping(int meshIndex, int materialIndex)
{
    if (!m_Combined.loadedModel)
        return;

    const int meshCount = (int)m_Combined.meshes.size();
    if (meshIndex < 0 || meshIndex >= meshCount)
        return;

    int clampedMaterialIndex = materialIndex;

    if (m_Combined.loadedMaterial)
    {
        const int materialCount = (int)m_Combined.materialFile.materials.size();
        if (materialCount > 0)
        {
            if (clampedMaterialIndex < 0) clampedMaterialIndex = 0;
            if (clampedMaterialIndex >= materialCount) clampedMaterialIndex = materialCount - 1;
        }
    }

    MeshView& meshView = m_Combined.meshes[(size_t)meshIndex];
    meshView.materialIndex = clampedMaterialIndex;
    meshView.meshHeaderRaw.MaterialIndex = (_uint)max(clampedMaterialIndex, 0);

    Link_ModelToMaterial(m_Combined);
    RebuildMaterialCentricCache();
}

// -------------------- Material ops --------------------

void CAdjuster::Material_Up(int index) { Move_MaterialIndex(index, index - 1); }
void CAdjuster::Material_Down(int index) { Move_MaterialIndex(index, index + 1); }

void CAdjuster::Move_MaterialIndex(int fromIndex, int toIndex)
{
    if (!m_Combined.loadedMaterial) return;

    auto& materials = m_Combined.materialFile.materials;
    const int count = (int)materials.size();
    if (fromIndex < 0 || fromIndex >= count) return;
    if (toIndex < 0 || toIndex >= count) return;
    if (fromIndex == toIndex) return;

    vector<int> oldToNew(count);
    for (int index = 0; index < count; ++index) oldToNew[index] = index;

    MaterialInfo moved = materials[(size_t)fromIndex];
    materials.erase(materials.begin() + fromIndex);
    materials.insert(materials.begin() + toIndex, moved);

    if (fromIndex < toIndex)
    {
        for (int oldIndex = fromIndex + 1; oldIndex <= toIndex; ++oldIndex) oldToNew[oldIndex] = oldIndex - 1;
        oldToNew[fromIndex] = toIndex;
    }
    else
    {
        for (int oldIndex = toIndex; oldIndex <= fromIndex - 1; ++oldIndex) oldToNew[oldIndex] = oldIndex + 1;
        oldToNew[fromIndex] = toIndex;
    }

    if (m_Combined.loadedModel)
        RemapModelMaterialIndices(m_Combined.meshes, oldToNew);

    if ((int)m_SelectedMaterial == fromIndex)
        m_SelectedMaterial = (size_t)toIndex;
    else
    {
        const int selectedMaterialIndex = (int)m_SelectedMaterial;
        if (selectedMaterialIndex >= 0 && selectedMaterialIndex < count)
            m_SelectedMaterial = (size_t)max(oldToNew[selectedMaterialIndex], 0);
    }

    Link_ModelToMaterial(m_Combined);
    RebuildMaterialCounts();
    RebuildMaterialCentricCache();

    m_PreviewTexKey.clear();
}

void CAdjuster::Material_Duplicate(int sourceIndex)
{
    if (!m_Combined.loadedMaterial) return;

    auto& materials = m_Combined.materialFile.materials;
    const int count = (int)materials.size();
    if (sourceIndex < 0 || sourceIndex >= count) return;

    MaterialInfo cloned = materials[(size_t)sourceIndex];

    string originalKey = FixedCharToString(cloned.header.materialDataKey, IM_ARRAYSIZE(cloned.header.materialDataKey));
    string newKey = originalKey + "_Copy";
    CopyToFixedChar(cloned.header.materialDataKey, IM_ARRAYSIZE(cloned.header.materialDataKey), newKey);

    const int insertIndex = sourceIndex + 1;
    materials.insert(materials.begin() + insertIndex, cloned);

    if (m_Combined.loadedModel)
    {
        for (auto& meshView : m_Combined.meshes)
        {
            if (meshView.materialIndex >= insertIndex)
                meshView.materialIndex += 1;
            meshView.meshHeaderRaw.MaterialIndex = (_uint)max(meshView.materialIndex, 0);
        }
    }

    m_SelectedMaterial = (size_t)insertIndex;
    m_SelectedType = 0;
    m_SelectedTexture = 0;

    Link_ModelToMaterial(m_Combined);
    RebuildMaterialCounts();
    RebuildMaterialCentricCache();

    m_PreviewTexKey.clear();
}

void CAdjuster::Material_Delete(int deleteIndex)
{
    if (!m_Combined.loadedMaterial) return;

    auto& materials = m_Combined.materialFile.materials;
    const int count = (int)materials.size();
    if (deleteIndex < 0 || deleteIndex >= count) return;

    materials.erase(materials.begin() + deleteIndex);

    if (m_Combined.loadedModel)
    {
        for (auto& meshView : m_Combined.meshes)
        {
            if (meshView.materialIndex == deleteIndex)
                meshView.materialIndex = 0;
            else if (meshView.materialIndex > deleteIndex)
                meshView.materialIndex -= 1;

            meshView.meshHeaderRaw.MaterialIndex = (_uint)max(meshView.materialIndex, 0);
        }
    }

    if (materials.empty())
        m_SelectedMaterial = 0;
    else
        m_SelectedMaterial = (size_t)min((int)m_SelectedMaterial, (int)materials.size() - 1);

    m_SelectedType = 0;
    m_SelectedTexture = 0;

    Link_ModelToMaterial(m_Combined);
    RebuildMaterialCounts();
    RebuildMaterialCentricCache();

    m_PreviewTexKey.clear();
}

// -------------------- Texture ops --------------------

void CAdjuster::Texture_DeleteSelected()
{
    if (!m_Combined.loadedMaterial) return;

    auto& mats = m_Combined.materialFile.materials;
    if (mats.empty()) return;
    if (m_SelectedMaterial >= mats.size()) return;

    MaterialInfo& mat = mats[m_SelectedMaterial];
    if (m_SelectedType >= mat.textureTypes.size()) return;

    TextureFile& texFile = mat.textureTypes[m_SelectedType];
    if (m_SelectedTexture >= texFile.textures.size()) return;

    const string deleteKey =
        FixedCharToString(texFile.textures[m_SelectedTexture].header.TextureKey,
            IM_ARRAYSIZE(texFile.textures[m_SelectedTexture].header.TextureKey));

    texFile.textures.erase(texFile.textures.begin() + (int)m_SelectedTexture);

    int foundIndex = -1;
    for (int textureIndex = 0; textureIndex < (int)texFile.textures.size(); ++textureIndex)
    {
        const string key =
            FixedCharToString(texFile.textures[textureIndex].header.TextureKey,
                IM_ARRAYSIZE(texFile.textures[textureIndex].header.TextureKey));

        if (key == deleteKey)
        {
            foundIndex = textureIndex;
            break;
        }
    }

    if (foundIndex >= 0) m_SelectedTexture = (size_t)foundIndex;
    else
    {
        if (texFile.textures.empty()) m_SelectedTexture = 0;
        else if (m_SelectedTexture >= texFile.textures.size()) m_SelectedTexture = texFile.textures.size() - 1;
    }

    RebuildMaterialCounts();
    m_PreviewTexKey.clear();
}

void CAdjuster::TextureType_RemoveIfEmpty()
{
    if (!m_Combined.loadedMaterial) return;

    auto& mats = m_Combined.materialFile.materials;
    if (mats.empty()) return;
    if (m_SelectedMaterial >= mats.size()) return;

    MaterialInfo& mat = mats[m_SelectedMaterial];
    if (m_SelectedType >= mat.textureTypes.size()) return;

    TextureFile& texFile = mat.textureTypes[m_SelectedType];
    if (!texFile.textures.empty()) return;

    mat.textureTypes.erase(mat.textureTypes.begin() + (int)m_SelectedType);

    if (mat.textureTypes.empty()) m_SelectedType = 0;
    else if (m_SelectedType >= mat.textureTypes.size()) m_SelectedType = mat.textureTypes.size() - 1;

    m_SelectedTexture = 0;

    RebuildMaterialCounts();
    m_PreviewTexKey.clear();
}

void CAdjuster::UpdatePreviewTextureIfNeeded()
{
    if (!m_Combined.loadedMaterial) return;

    auto& mats = m_Combined.materialFile.materials;
    if (mats.empty()) return;
    if (m_SelectedMaterial >= mats.size()) return;

    MaterialInfo& mat = mats[m_SelectedMaterial];
    if (m_SelectedType >= mat.textureTypes.size()) return;

    TextureFile& texFile = mat.textureTypes[m_SelectedType];
    if (m_SelectedTexture >= texFile.textures.size()) return;

    TextureInfo& tex = texFile.textures[m_SelectedTexture];

    const string texKey = FixedCharToString(tex.header.TextureKey, IM_ARRAYSIZE(tex.header.TextureKey));
    if (texKey.empty()) return;

    if (texKey == m_PreviewTexKey)
        return;

    if (m_PreviewTex)
        Safe_Release(m_PreviewTex);

    m_PreviewTexKey = texKey;

    m_PreviewTex = ResourceManager()->Load_Texture(G_GlobalLevelKey, texKey);
    if (m_PreviewTex)
        Safe_AddRef(m_PreviewTex);
}

// -------------------- DnD helpers --------------------

bool CAdjuster::BeginDragMaterial(int materialIndex)
{
    if (!ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
        return false;

    ImGui::SetDragDropPayload("DND_MATERIAL_INDEX", &materialIndex, sizeof(int));

    const string displayKey =
        (m_Combined.loadedMaterial && materialIndex >= 0 && materialIndex < (int)m_Combined.materialFile.materials.size())
        ? FixedCharToString(m_Combined.materialFile.materials[(size_t)materialIndex].header.materialDataKey,
            IM_ARRAYSIZE(m_Combined.materialFile.materials[(size_t)materialIndex].header.materialDataKey))
        : string("Material");

    ImGui::Text("Assign Material [%d]", materialIndex);
    ImGui::TextDisabled("%s", displayKey.c_str());

    ImGui::EndDragDropSource();
    return true;
}

bool CAdjuster::BeginDragMesh(int meshIndex)
{
    if (!ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
        return false;

    ImGui::SetDragDropPayload("DND_MESH_INDEX", &meshIndex, sizeof(int));

    const string meshName =
        (m_Combined.loadedModel && meshIndex >= 0 && meshIndex < (int)m_Combined.meshes.size())
        ? m_Combined.meshes[(size_t)meshIndex].meshName
        : string("Mesh");

    ImGui::Text("Assign Mesh");
    ImGui::TextDisabled("%s", meshName.c_str());

    ImGui::EndDragDropSource();
    return true;
}

bool CAdjuster::AcceptDropMaterialToMesh(int targetMeshIndex)
{
    if (!ImGui::BeginDragDropTarget())
        return false;

    bool changed = false;

    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_MATERIAL_INDEX"))
    {
        if (payload->Data && payload->DataSize == sizeof(int))
        {
            const int droppedMaterialIndex = *reinterpret_cast<const int*>(payload->Data);
            ApplyMeshMaterialMapping(targetMeshIndex, droppedMaterialIndex);

            m_SelectedMesh = targetMeshIndex;
            m_SelectedMaterial = (size_t)max(droppedMaterialIndex, 0);
            RequestScrollToMaterial(droppedMaterialIndex);

            changed = true;
        }
    }

    ImGui::EndDragDropTarget();
    return changed;
}

bool CAdjuster::AcceptDropMeshToMaterial(int targetMaterialIndex)
{
    if (!ImGui::BeginDragDropTarget())
        return false;

    bool changed = false;

    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_MESH_INDEX"))
    {
        if (payload->Data && payload->DataSize == sizeof(int))
        {
            const int droppedMeshIndex = *reinterpret_cast<const int*>(payload->Data);
            ApplyMeshMaterialMapping(droppedMeshIndex, targetMaterialIndex);

            m_SelectedMesh = droppedMeshIndex;
            m_SelectedMaterial = (size_t)max(targetMaterialIndex, 0);
            RequestScrollToMesh(droppedMeshIndex);

            changed = true;
        }
    }

    ImGui::EndDragDropTarget();
    return changed;
}

void CAdjuster::DrawMaterialUseTooltip(int materialIndex)
{
    if (!m_Combined.loadedModel || !m_Combined.loadedMaterial)
        return;

    if (materialIndex < 0 || materialIndex >= (int)m_MappingCache.meshesByMaterial.size())
        return;

    if (!ImGui::IsItemHovered())
        return;

    ImGui::BeginTooltip();
    const int useCount = (materialIndex >= 0 && materialIndex < (int)m_MappingCache.useCount.size())
        ? m_MappingCache.useCount[(size_t)materialIndex]
        : 0;

    ImGui::Text("Use Count: %d", useCount);
    ImGui::Separator();

    const auto& meshes = m_MappingCache.meshesByMaterial[(size_t)materialIndex];
    if (meshes.empty())
    {
        ImGui::TextDisabled("No meshes mapped.");
    }
    else
    {
        const int maxLines = 24;
        int shownLines = 0;
        for (int meshIndex : meshes)
        {
            if (meshIndex < 0 || meshIndex >= (int)m_Combined.meshes.size())
                continue;

            ImGui::BulletText("%s", m_Combined.meshes[(size_t)meshIndex].meshName.c_str());
            shownLines += 1;
            if (shownLines >= maxLines)
            {
                if ((int)meshes.size() > maxLines)
                    ImGui::TextDisabled("... +%d more", (int)meshes.size() - maxLines);
                break;
            }
        }
    }

    ImGui::EndTooltip();
}

// -------------------- GUI --------------------

void CAdjuster::DrawSplitter(const char* id, bool isVertical, float& size, float minSize, float maxSize)
{
    ImGui::PushID(id);

    ImVec2 avail = ImGui::GetContentRegionAvail();
    ImVec2 splitterSize = isVertical ? ImVec2(6.0f, avail.y) : ImVec2(avail.x, 6.0f);

    ImGui::InvisibleButton("##splitter", splitterSize);
    if (ImGui::IsItemActive())
    {
        float delta = isVertical ? ImGui::GetIO().MouseDelta.x : ImGui::GetIO().MouseDelta.y;
        size += delta;
        if (size < minSize) size = minSize;
        if (size > maxSize) size = maxSize;
    }

    if (ImGui::IsItemHovered() || ImGui::IsItemActive())
        ImGui::SetMouseCursor(isVertical ? ImGuiMouseCursor_ResizeEW : ImGuiMouseCursor_ResizeNS);

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 rectMin = ImGui::GetItemRectMin();
    ImVec2 rectMax = ImGui::GetItemRectMax();
    drawList->AddRectFilled(rectMin, rectMax, IM_COL32(90, 90, 90, 120));

    ImGui::PopID();
}

void CAdjuster::Render_GUI()
{
    Render_AdjustTab();
}

void CAdjuster::Render_AdjustTab()
{
    ImGui::SetNextWindowSize(ImVec2(1200, 760), ImGuiCond_FirstUseEver);
    ImGui::Begin("Adjust_Binary_Combined", &isTabOpen, ImGuiWindowFlags_NoCollapse);

    DrawTopToolbar();
    ImGui::Separator();

    const ImVec2 avail = ImGui::GetContentRegionAvail();
    const float height = avail.y;
    const float minPane = 260.0f;
    const float splitter = 6.0f;

    const float total = avail.x;
    const float maxModel = total - (minPane + m_MaterialPaneWidth + splitter * 2);
    if (m_ModelPaneWidth < minPane) m_ModelPaneWidth = minPane;
    if (m_ModelPaneWidth > maxModel) m_ModelPaneWidth = max(minPane, maxModel);

    const float maxMat = total - (minPane + m_ModelPaneWidth + splitter * 2);
    if (m_MaterialPaneWidth < minPane) m_MaterialPaneWidth = minPane;
    if (m_MaterialPaneWidth > maxMat) m_MaterialPaneWidth = max(minPane, maxMat);

    DrawModelPane(m_ModelPaneWidth, height);

    ImGui::SameLine();
    DrawSplitter("SPLIT_MODEL", true, m_ModelPaneWidth, minPane, total - minPane * 2 - splitter * 2);
    ImGui::SameLine();

    DrawMaterialPane(m_MaterialPaneWidth, height);

    ImGui::SameLine();
    DrawSplitter("SPLIT_MAT", true, m_MaterialPaneWidth, minPane, total - minPane - splitter * 2);
    ImGui::SameLine();

    DrawDetailPane(0.0f, height);

    ImGui::End();
}

void CAdjuster::DrawTopToolbar()
{
    if (ImGui::Button("Load .mat"))
    {
        string picked = Helper::OpenFile_Dialogue();
        if (!picked.empty() && HasExtension(picked, ".mat"))
        {
            m_Combined.materialPath = picked;
            const string fileName = filesystem::path(picked).filename().string();

            m_Combined.loadedMaterial = SUCCEEDED(Load_MaterialFile(fileName, picked, m_Combined.materialFile));
            RebuildMaterialCounts();
            Link_ModelToMaterial(m_Combined);

            m_SelectedMaterial = 0;
            m_SelectedType = 0;
            m_SelectedTexture = 0;
            m_PreviewTexKey.clear();

            if (m_PreviewTex) Safe_Release(m_PreviewTex);

            RebuildMaterialCentricCache();
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Load .model"))
    {
        string picked = Helper::OpenFile_Dialogue();
        if (!picked.empty() && HasExtension(picked, ".model"))
        {
            m_Combined.modelPath = picked;
            m_Combined.loadedModel = SUCCEEDED(Load_ModelHeaderAndMeshViews(picked, m_Combined));
            Link_ModelToMaterial(m_Combined);
            RebuildMaterialCentricCache();
        }
    }

    ImGui::SameLine();
    ImGui::Dummy(ImVec2(12, 0));
    ImGui::SameLine();

    if (ImGui::Button("Save .mat"))
    {
        if (m_Combined.loadedMaterial && !m_Combined.materialPath.empty())
        {
            RebuildMaterialCounts();
            Save_MaterialFile(m_Combined.materialPath, m_Combined.materialFile);
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Save .model"))
    {
        if (m_Combined.loadedModel && !m_Combined.modelPath.empty())
        {
            for (auto& meshView : m_Combined.meshes)
                meshView.meshHeaderRaw.MaterialIndex = (_uint)max(meshView.materialIndex, 0);

            Save_ModelFile_UpdateMaterialIndex(m_Combined.modelPath, m_Combined);
        }
    }

    ImGui::SameLine();
    ImGui::Dummy(ImVec2(12, 0));
    ImGui::SameLine();

    ImGui::Checkbox("Reverse Mesh Order", &m_Combined.reverseMeshOrder);

    ImGui::SameLine();
    ImGui::Checkbox("Show Drop Hints", &m_ShowDropHints);

    ImGui::SameLine();
    if (ImGui::Button("Clear All"))
    {
        m_Combined = {};
        m_SelectedMesh = -1;
        m_SelectedMaterial = 0;
        m_SelectedType = 0;
        m_SelectedTexture = 0;

        m_PreviewTexKey.clear();
        if (m_PreviewTex) Safe_Release(m_PreviewTex);

        m_MappingCache = {};
        m_RequestScrollToMaterial = false;
        m_RequestScrollToMesh = false;
    }

    ImGui::SameLine();
    ImGui::TextDisabled("Model:%s  Mat:%s",
        m_Combined.loadedModel ? "OK" : "None",
        m_Combined.loadedMaterial ? "OK" : "None");
}
void CAdjuster::DrawModelPane(float width, float height)
{
    ImGui::BeginChild("##ModelPane", ImVec2(width, height), true);
    ImGui::SeparatorText("Model");

    ImGui::SetNextItemWidth(-1);
    ImGui::InputTextWithHint("##modelFilter", "Search mesh...", m_ModelFilter, sizeof(m_ModelFilter));
    ImGui::Spacing();

    if (!m_Combined.loadedModel)
    {
        ImGui::TextUnformatted("No model loaded.");
        ImGui::EndChild();
        return;
    }

    const int meshCount = (int)m_Combined.meshes.size();
    const int materialCount = m_Combined.loadedMaterial ? (int)m_Combined.materialFile.materials.size() : 0;

    ImGui::Text("Meshes: %d", meshCount);
    ImGui::Separator();

    const ImGuiTableFlags tableFlags =
        ImGuiTableFlags_RowBg |
        ImGuiTableFlags_BordersInnerV |
        ImGuiTableFlags_ScrollY |
        ImGuiTableFlags_Resizable;

    if (ImGui::BeginTable("##MeshTable", 2, tableFlags, ImVec2(0, 0)))
    {
        ImGui::TableSetupColumn("Mesh", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Material", ImGuiTableColumnFlags_WidthFixed, 190.0f);
        ImGui::TableHeadersRow();

        const ImVec2 buttonSize(44.0f, 24.0f);

        for (int meshIndex = 0; meshIndex < meshCount; ++meshIndex)
        {
            MeshView& meshView = m_Combined.meshes[(size_t)meshIndex];

            if (m_ModelFilter[0] != '\0')
            {
                const string filterLower = Helper::ToLower(string(m_ModelFilter));
                if (Helper::ToLower(meshView.meshName).find(filterLower) == string::npos)
                    continue;
            }

            ImGui::TableNextRow();

            // ---------------- Column 0 ----------------
            ImGui::TableSetColumnIndex(0);
            ImGui::PushID(meshIndex);

            const bool isSelectedMesh = (m_SelectedMesh == meshIndex);

            // Scroll request
            if (m_RequestScrollToMesh && m_ScrollToMeshIndex == meshIndex)
            {
                ImGui::SetScrollHereY(0.25f);
                m_RequestScrollToMesh = false;
            }

            // "관계 강조" : 현재 선택된 머티리얼(m_SelectedMaterial)에 매핑된 메쉬들
            const bool isMappedToSelectedMaterial =
                (m_Combined.loadedMaterial &&
                    meshView.materialIndex >= 0 &&
                    (size_t)meshView.materialIndex == m_SelectedMaterial);

            // 색: 선택된 메쉬 > (선택된 머티리얼에 매핑된 메쉬)
            if (isSelectedMesh)
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 230, 140, 255)); // 메쉬 직접 선택(노랑)
            else if (isMappedToSelectedMaterial)
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 200, 120, 255)); // 관계 강조(주황)
            else
                ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_Text));

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6.0f, 6.0f));
            const bool clicked = ImGui::Selectable(meshView.meshName.c_str(), isSelectedMesh);
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();

            if (clicked)
            {
                m_SelectedMesh = meshIndex;

                // "메쉬 기준으로 머티리얼 색 변화"가 확실히 보이도록:
                // 1) 머티리얼 리스트에서 해당 인덱스를 강조하려면 m_SelectedMesh만으로도 충분(SelectedMeshMatIndex로)
                // 2) 동시에 머티리얼 쪽으로 스크롤까지 시키면 체감이 더 큼
                if (meshView.materialIndex >= 0)
                    RequestScrollToMaterial(meshView.materialIndex);

                // 원하면 메쉬 클릭 시 머티리얼도 같이 '선택'으로 맞춰버릴 수 있음(취향)
                if (meshView.materialIndex >= 0)
                    m_SelectedMaterial = (size_t)meshView.materialIndex;
            }

            // Drag source: mesh -> material
            BeginDragMesh(meshIndex);

            // Drop target: material -> mesh
            const bool droppedOnMesh = AcceptDropMaterialToMesh(meshIndex);

            if (m_ShowDropHints)
            {
                if (ImGui::BeginDragDropTarget())
                {
                    if (ImGui::GetDragDropPayload() != nullptr)
                    {
                        ImGui::SameLine();
                        ImGui::TextDisabled("  (Drop to assign)");
                    }
                    ImGui::EndDragDropTarget();
                }
            }

            // ---------------- Column 1 ----------------
            ImGui::TableSetColumnIndex(1);

            int newMatIndex = meshView.materialIndex;

            ImGui::AlignTextToFramePadding();
            ImGui::TextDisabled("Mat");
            ImGui::SameLine();

            ImGui::SetNextItemWidth(90.0f);
            const ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_EnterReturnsTrue;
            const bool changedByEnter = ImGui::InputInt("##matIndex", &newMatIndex, 1, 10, inputFlags);

            ImGui::SameLine(0.0f, 6.0f);

            ImGui::BeginDisabled(materialCount <= 0);
            if (ImGui::Button("Clamp", ImVec2(60.0f, buttonSize.y)))
            {
                if (newMatIndex < 0) newMatIndex = 0;
                if (newMatIndex >= materialCount) newMatIndex = materialCount - 1;
                ApplyMeshMaterialMapping(meshIndex, newMatIndex);
            }
            ImGui::EndDisabled();

            ImGui::SameLine(0.0f, 6.0f);

            if (ImGui::Button("Apply", ImVec2(60.0f, buttonSize.y)) || changedByEnter)
            {
                ApplyMeshMaterialMapping(meshIndex, newMatIndex);
            }

            // Visual mapping info
            if (meshView.materialIndex >= 0 && materialCount > 0 && meshView.materialIndex < materialCount)
            {
                const MaterialInfo& mappedMat = m_Combined.materialFile.materials[(size_t)meshView.materialIndex];
                const string matKey = FixedCharToString(mappedMat.header.materialDataKey,
                    IM_ARRAYSIZE(mappedMat.header.materialDataKey));

                ImGui::SameLine(0.0f, 8.0f);
                ImGui::TextDisabled("%s", matKey.c_str());
            }
            else
            {
                ImGui::SameLine(0.0f, 8.0f);
                ImGui::TextDisabled("None");
            }

            if (droppedOnMesh)
            {
                // refresh selection to reflect mapping
                m_SelectedMesh = meshIndex;
                if (meshView.materialIndex >= 0)
                {
                    // 드롭했을 때도 머티리얼 쪽 강조/스크롤이 보이게
                    RequestScrollToMaterial(meshView.materialIndex);
                    // 원하면 선택 동기화:
                    m_SelectedMaterial = (size_t)meshView.materialIndex;
                }
            }

            ImGui::PopID();
        }

        ImGui::EndTable();
    }

    ImGui::EndChild();
}

void CAdjuster::DrawMaterialPane(float width, float height)
{
    ImGui::BeginChild("##MaterialPane", ImVec2(width, height), true);
    ImGui::SeparatorText("Materials");

    ImGui::SetNextItemWidth(-1);
    ImGui::InputTextWithHint("##matFilter", "Search material...", m_MatFilter, sizeof(m_MatFilter));
    ImGui::Spacing();

    if (!m_Combined.loadedMaterial)
    {
        ImGui::TextUnformatted("No material loaded.");
        ImGui::EndChild();
        return;
    }

    auto& materials = m_Combined.materialFile.materials;
    ImGui::Text("Count: %d", (int)materials.size());
    ImGui::Separator();

    const ImVec2 buttonSize(44.0f, 24.0f);
    const ImVec2 buttonSizeWide(56.0f, 24.0f);

    const ImGuiTableFlags tableFlags =
        ImGuiTableFlags_RowBg |
        ImGuiTableFlags_BordersInnerV |
        ImGuiTableFlags_ScrollY |
        ImGuiTableFlags_Resizable;

    if (ImGui::BeginTable("##MatTable", 3, tableFlags, ImVec2(0, 0)))
    {
        ImGui::TableSetupColumn("Material", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Use", ImGuiTableColumnFlags_WidthFixed, 60.0f);
        ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed,
            4.0f * buttonSize.x + 2.0f * buttonSizeWide.x + 24.0f);
        ImGui::TableHeadersRow();

        for (int matIndex = 0; matIndex < (int)materials.size(); ++matIndex)
        {
            MaterialInfo& mat = materials[(size_t)matIndex];
            const string key = FixedCharToString(mat.header.materialDataKey, IM_ARRAYSIZE(mat.header.materialDataKey));

            if (m_MatFilter[0] != '\0')
            {
                if (Helper::ToLower(key).find(Helper::ToLower(string(m_MatFilter))) == string::npos)
                    continue;
            }

            ImGui::TableNextRow();
            ImGui::PushID(matIndex);

            // Scroll request
            if (m_RequestScrollToMaterial && m_ScrollToMaterialIndex == matIndex)
            {
                ImGui::SetScrollHereY(0.25f);
                m_RequestScrollToMaterial = false;
            }

            const bool isSelected = ((int)m_SelectedMaterial == matIndex);

            int useCount = 0;
            if (matIndex >= 0 && matIndex < (int)m_MappingCache.useCount.size())
                useCount = m_MappingCache.useCount[(size_t)matIndex];

            // ---- Column 0 : Material row (single selectable) ----
            ImGui::TableSetColumnIndex(0);
            const int selectedMeshMatIndex = GetSelectedMeshMaterialIndex();
            const bool emphasizeSelected = isSelected;
            const bool emphasizeMappedFromMesh =
                (selectedMeshMatIndex >= 0 && selectedMeshMatIndex == matIndex);
            const bool emphasizeUsed = (useCount > 0);

            // 우선순위: "현재 머티리얼 선택" > "선택 메쉬의 머티리얼" > "사용중"
            if (emphasizeSelected)
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 230, 140, 255));  // 머티리얼 직접 선택(노랑)
            else if (emphasizeMappedFromMesh)
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 200, 120, 255));  // 선택된 메쉬의 머티리얼(주황)
            else if (emphasizeUsed)
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 255, 200, 255));  // 사용중(연녹)
            else
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));  // 기본(연회)

            const string rowLabel = "[" + to_string(matIndex) + "] " + key;

            // padding
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6.0f, 6.0f));
            const bool clicked = ImGui::Selectable(rowLabel.c_str(), isSelected, ImGuiSelectableFlags_None, ImVec2(0.0f, 0.0f));
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();

            if (clicked)
            {
                m_SelectedMaterial = (size_t)matIndex;
                m_SelectedType = 0;
                m_SelectedTexture = 0;
                m_PreviewTexKey.clear();

                // 선택한 머티리얼을 쓰는 메쉬로 스크롤(옵션)
                if (matIndex >= 0 && matIndex < (int)m_MappingCache.meshesByMaterial.size())
                {
                    const auto& meshList = m_MappingCache.meshesByMaterial[(size_t)matIndex];
                    if (!meshList.empty())
                        RequestScrollToMesh(meshList.front());
                }
            }

            // Drag material -> drop on mesh (source)
            BeginDragMaterial(matIndex);

            // Drop target: mesh -> material
            // 힌트까지 여기서 같이 처리(중복 BeginDragDropTarget 제거)
            bool isDropTargetActive = false;
            if (ImGui::BeginDragDropTarget())
            {
                isDropTargetActive = (ImGui::GetDragDropPayload() != nullptr);

                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_MESH_INDEX"))
                {
                    if (payload->Data && payload->DataSize == sizeof(int))
                    {
                        const int droppedMeshIndex = *reinterpret_cast<const int*>(payload->Data);
                        ApplyMeshMaterialMapping(droppedMeshIndex, matIndex);

                        m_SelectedMesh = droppedMeshIndex;
                        m_SelectedMaterial = (size_t)max(matIndex, 0);
                        RequestScrollToMesh(droppedMeshIndex);
                    }
                }
                ImGui::EndDragDropTarget();
            }

            if (m_ShowDropHints && isDropTargetActive)
            {
                ImGui::SameLine();
                ImGui::TextDisabled("  (Drop to assign)");
            }

            // Tooltip(원하면 유지 / 싫으면 이 라인 삭제)
            DrawMaterialUseTooltip(matIndex);

            // ---- Column 1 : Use count ----
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%d", useCount);

            // ---- Column 2 : Actions ----
            ImGui::TableSetColumnIndex(2);

            ImGui::BeginGroup();

            ImGui::BeginDisabled(matIndex == 0);
            if (ImGui::Button("Up", buttonSize))
                Material_Up(matIndex);
            ImGui::EndDisabled();

            ImGui::SameLine(0.0f, 6.0f);

            ImGui::BeginDisabled(matIndex == (int)materials.size() - 1);
            if (ImGui::Button("Down", buttonSize))
                Material_Down(matIndex);
            ImGui::EndDisabled();

            ImGui::SameLine(0.0f, 10.0f);

            if (ImGui::Button("Dup", buttonSizeWide))
                Material_Duplicate(matIndex);

            ImGui::SameLine(0.0f, 6.0f);

            ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(170, 60, 60, 255));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(190, 70, 70, 255));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(150, 50, 50, 255));
            if (ImGui::Button("Del", buttonSizeWide))
                ImGui::OpenPopup("##ConfirmDeleteMat");
            ImGui::PopStyleColor(3);

            if (ImGui::BeginPopupModal("##ConfirmDeleteMat", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Delete material index %d?", matIndex);
                ImGui::Separator();

                if (ImGui::Button("Delete", ImVec2(120, 0)))
                {
                    Material_Delete(matIndex);
                    ImGui::CloseCurrentPopup();
                    ImGui::EndPopup();
                    ImGui::EndGroup();
                    ImGui::PopID();
                    break;
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel", ImVec2(120, 0)))
                    ImGui::CloseCurrentPopup();

                ImGui::EndPopup();
            }

            ImGui::EndGroup();
            ImGui::PopID();
        }

        ImGui::EndTable();
    }

    ImGui::EndChild();
}
int CAdjuster::GetSelectedMeshMaterialIndex() const
{
    if (!m_Combined.loadedModel) return -1;
    if (m_SelectedMesh < 0 || m_SelectedMesh >= (int)m_Combined.meshes.size()) return -1;

    const MeshView& meshView = m_Combined.meshes[(size_t)m_SelectedMesh];
    return meshView.materialIndex;
}
void CAdjuster::DrawDetailPane(float width, float height)
{
    ImGui::BeginChild("##DetailPane", ImVec2(width, height), true);
    ImGui::SeparatorText("Detail");

    const float total = ImGui::GetContentRegionAvail().x;
    const float leftWidth = max(360.0f, total * 0.58f);

    if (!m_Combined.loadedMaterial || m_Combined.materialFile.materials.empty())
    {
        ImGui::TextUnformatted("Select a material.");
        ImGui::EndChild();
        return;
    }

    if (m_SelectedMaterial >= m_Combined.materialFile.materials.size())
        m_SelectedMaterial = 0;

    MaterialInfo& mat = m_Combined.materialFile.materials[m_SelectedMaterial];

    // ---- Material header ----
    ImGui::Text("Material Index: %d", (int)m_SelectedMaterial);
    ImGui::InputText("Material Key", mat.header.materialDataKey, IM_ARRAYSIZE(mat.header.materialDataKey));
    ImGui::InputText("Shader Key", mat.header.ShaderKey, IM_ARRAYSIZE(mat.header.ShaderKey));
    ImGui::InputText("Shader Pass", mat.header.passConstant, IM_ARRAYSIZE(mat.header.passConstant));

    ImGui::Separator();

    // ---- Left : Texture list ----
    ImGui::BeginChild("##TexListPane", ImVec2(leftWidth, 0), true);
    ImGui::SeparatorText("Textures");

    // Toolbar row: Filter | Add | Browse | options
    {
        const float rowWidth = ImGui::GetContentRegionAvail().x;

        // Filter
        ImGui::SetNextItemWidth(rowWidth - 330.0f); // 버튼 영역 확보 (대충)
        ImGui::InputTextWithHint("##texFilter", "Search texture key...", m_TexFilter, sizeof(m_TexFilter));

        ImGui::SameLine();

        if (ImGui::Button("Add", ImVec2(60.0f, 0.0f)))
            OpenAddTexturePopup_Default();

        ImGui::SameLine();

        if (ImGui::Button("Browse...", ImVec2(90.0f, 0.0f)))
            OpenAddTexturePopup_Browse();

        ImGui::SameLine();
        ImGui::Checkbox("Auto", &m_AddTexAutoSelect);

        ImGui::SameLine();
        ImGui::Checkbox("Dup", &m_AddTexAllowDuplicateKey);
    }

    DrawAddTexturePopup(mat);
    ImGui::Separator();

    // Types / textures
    for (size_t typeIdx = 0; typeIdx < mat.textureTypes.size(); ++typeIdx)
    {
        TextureFile& texFile = mat.textureTypes[typeIdx];

        const string typeName = ConvertToConstant((TEXTURE_TYPE)texFile.header.typeID);
        const string nodeLabel = typeName + "  (" + to_string(texFile.header.TextureCount) + ")##Type" + to_string(typeIdx);

        if (!ImGui::TreeNode(nodeLabel.c_str()))
            continue;

        for (size_t texIdx = 0; texIdx < texFile.textures.size(); ++texIdx)
        {
            TextureInfo& info = texFile.textures[texIdx];
            const string texKey = FixedCharToString(info.header.TextureKey, IM_ARRAYSIZE(info.header.TextureKey));

            if (m_TexFilter[0] != '\0')
            {
                const string filterLower = Helper::ToLower(string(m_TexFilter));
                if (Helper::ToLower(texKey).find(filterLower) == string::npos)
                    continue;
            }

            const bool isSelected = (m_SelectedType == typeIdx && m_SelectedTexture == texIdx);

            ImGui::PushID((int)(typeIdx * 100000 + texIdx));
            if (ImGui::Selectable(texKey.c_str(), isSelected))
            {
                m_SelectedType = typeIdx;
                m_SelectedTexture = texIdx;
                UpdatePreviewTextureIfNeeded();
            }
            ImGui::PopID();
        }

        ImGui::TreePop();
    }

    ImGui::EndChild();

    ImGui::SameLine();

    // ---- Right : Preview / Edit ----
    ImGui::BeginChild("##TexDetailPane", ImVec2(0, 0), true);
    ImGui::SeparatorText("Preview / Edit");

    if (m_SelectedType < mat.textureTypes.size())
    {
        TextureFile& texFile = mat.textureTypes[m_SelectedType];
        if (m_SelectedTexture < texFile.textures.size())
        {
            TextureInfo& tex = texFile.textures[m_SelectedTexture];
            const string texKey = FixedCharToString(tex.header.TextureKey, IM_ARRAYSIZE(tex.header.TextureKey));

            ImGui::Text("Selected: %s", texKey.c_str());

            UpdatePreviewTextureIfNeeded();

            // 프리뷰 크기 축소
            const float previewSide = 256.0f; // 320 -> 256
            ImVec2 previewSize(previewSide, previewSide);

            if (m_PreviewTex && m_PreviewTex->Get_SRV())
            {
                ImGui::Image((ImTextureID)m_PreviewTex->Get_SRV(), previewSize);
            }
            else
            {
                ImGui::BeginChild("##NoPreview", previewSize, true);
                ImGui::TextDisabled("No preview texture.");
                ImGui::EndChild();
            }

            ImGui::Spacing();
            ImGui::SeparatorText("Rename");

            if (m_LastRenameMaterial != m_SelectedMaterial ||
                m_LastRenameType != m_SelectedType ||
                m_LastRenameTex != m_SelectedTexture)
            {
                CopyToFixedChar(m_TexRenameBuf, sizeof(m_TexRenameBuf), texKey);
                m_LastRenameMaterial = m_SelectedMaterial;
                m_LastRenameType = m_SelectedType;
                m_LastRenameTex = m_SelectedTexture;
            }

            ImGui::SetNextItemWidth(-1);
            ImGui::InputTextWithHint("##rename", "New TextureKey...", m_TexRenameBuf, sizeof(m_TexRenameBuf));

            if (ImGui::Button("Apply Rename"))
            {
                CopyToFixedChar(tex.header.TextureKey, IM_ARRAYSIZE(tex.header.TextureKey), string(m_TexRenameBuf));
                RebuildMaterialCounts();
                m_PreviewTexKey.clear();
            }

            ImGui::SameLine();
            if (ImGui::Button("Delete"))
                ImGui::OpenPopup("Confirm Delete Texture");

            if (ImGui::BeginPopupModal("Confirm Delete Texture", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::TextUnformatted("Delete selected texture entry?");
                ImGui::Separator();

                if (ImGui::Button("Delete", ImVec2(120, 0)))
                {
                    Texture_DeleteSelected();
                    TextureType_RemoveIfEmpty();
                    m_PreviewTexKey.clear();
                    ImGui::CloseCurrentPopup();
                }

                ImGui::SameLine();
                if (ImGui::Button("Cancel", ImVec2(120, 0)))
                    ImGui::CloseCurrentPopup();

                ImGui::EndPopup();
            }
        }
        else
        {
            ImGui::TextDisabled("Select a texture.");
        }
    }
    else
    {
        ImGui::TextDisabled("Select a texture.");
    }

    ImGui::EndChild();
    ImGui::EndChild();
}
void CAdjuster::DrawAddTexturePopup(MaterialInfo& mat)
{
    if (!ImGui::BeginPopupModal("##AddTexturePopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        return;

    ImGui::TextUnformatted("Add Texture Entry");
    ImGui::Separator();

    Engine::TEXTURE_TYPE selectedType = (Engine::TEXTURE_TYPE)m_AddTexTypeId;

    ImGui::TextUnformatted("Type");
    ImGui::SetNextItemWidth(-1);
    if (ImGui::BeginCombo("##AddTexType", ConvertToConstant(selectedType).c_str()))
    {
        for (Engine::TEXTURE_TYPE type : g_TextureTypeList)
        {
            const bool isSel = (type == selectedType);
            const string label = ConvertToConstant(type);
            if (ImGui::Selectable(label.c_str(), isSel))
                m_AddTexTypeId = (int)type;
            if (isSel) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::Spacing();
    ImGui::TextUnformatted("TextureKey");
    ImGui::SetNextItemWidth(-1);
    ImGui::InputTextWithHint("##AddTexKey", "ex) T_MyDiffuse.dds", m_AddTexKeyBuf, sizeof(m_AddTexKeyBuf));

    const string newKey = string(m_AddTexKeyBuf);
    const bool isEmptyKey = newKey.empty();

    const bool isDupInType = (!isEmptyKey)
        ? IsTextureKeyExistsInType(mat, (Engine::TEXTURE_TYPE)m_AddTexTypeId, newKey)
        : false;

    const bool canCreate = (!isEmptyKey && (m_AddTexAllowDuplicateKey || !isDupInType));

    if (isEmptyKey)
        ImGui::TextDisabled("Key is empty.");
    else if (isDupInType && !m_AddTexAllowDuplicateKey)
        ImGui::TextDisabled("Duplicate key in same type.");

    ImGui::Separator();

    // 버튼 라인
    if (!canCreate) ImGui::BeginDisabled(true);
    if (ImGui::Button("Create", ImVec2(120, 0)))
    {
        const int createdTypeIndex = EnsureTextureType(mat, (Engine::TEXTURE_TYPE)m_AddTexTypeId);
        const int createdTexIndex = AddTextureEntry(mat, createdTypeIndex, newKey);

        RebuildMaterialCounts();
        m_PreviewTexKey.clear();

        if (m_AddTexAutoSelect)
        {
            m_SelectedType = (size_t)createdTypeIndex;
            m_SelectedTexture = (size_t)createdTexIndex;
            UpdatePreviewTextureIfNeeded();
        }

        memset(m_AddTexKeyBuf, 0, sizeof(m_AddTexKeyBuf));
        ImGui::CloseCurrentPopup();
    }
    if (!canCreate) ImGui::EndDisabled();

    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(120, 0)))
        ImGui::CloseCurrentPopup();

    ImGui::EndPopup();
}

void CAdjuster::OpenAddTexturePopup_Default()
{
    // 기본: 키 입력만 하게
    if (m_AddTexKeyBuf[0] == '\0')
        CopyToFixedChar(m_AddTexKeyBuf, sizeof(m_AddTexKeyBuf), "T_");

    ImGui::OpenPopup("##AddTexturePopup");
}

void CAdjuster::OpenAddTexturePopup_Browse()
{
    // 파일 선택 -> 키 자동 세팅
    const string picked = Helper::OpenFile_Dialogue(); // 네 기존 함수
    if (!picked.empty())
    {
        const string relKey = MakeTextureKeyRelativeToMaterial(picked);
        CopyToFixedChar(m_AddTexKeyBuf, sizeof(m_AddTexKeyBuf), relKey);

        RegisterTexturePathForKey(relKey, picked);
        ImGui::OpenPopup("##AddTexturePopup");
    }
}

string CAdjuster::MakeTextureKeyRelativeToMaterial(const string& absolutePath) const
{
    if (absolutePath.empty())
        return {};

    filesystem::path pickedPath = filesystem::path(absolutePath);

    // materialPath가 있으면 그 폴더 기준 상대경로를 우선
    if (!m_Combined.materialPath.empty())
    {
        filesystem::path matDir = filesystem::path(m_Combined.materialPath).parent_path();

        std::error_code err;
        filesystem::path rel = filesystem::relative(pickedPath, matDir, err);
        if (!err)
        {
            // 상대경로를 key로 (구분자는 '/'로 통일 추천)
            string relStr = rel.generic_string();
            return relStr;
        }
    }

    // fallback: 파일명만
    return pickedPath.filename().string();
}

void CAdjuster::RegisterTexturePathForKey(const string& textureKey, const string& absolutePath)
{
    if (textureKey.empty() || absolutePath.empty())
        return;

    ResourceManager()->Add_ResourcePath(textureKey, absolutePath);
}

int CAdjuster::EnsureTextureType(MaterialInfo& mat, Engine::TEXTURE_TYPE type)
{
    const int typeId = (int)type;

    for (int typeIndex = 0; typeIndex < (int)mat.textureTypes.size(); ++typeIndex)
    {
        TextureFile& texFile = mat.textureTypes[(size_t)typeIndex];
        if ((int)texFile.header.typeID == typeId)
            return typeIndex;
    }

    TextureFile newType = {};
    newType.header.typeID = (uint32_t)typeId;
    newType.header.TextureCount = 0;

    mat.textureTypes.push_back(newType);
    return (int)mat.textureTypes.size() - 1;
}
int CAdjuster::AddTextureEntry(MaterialInfo& mat, int typeIndex, const string& textureKey)
{
    if (typeIndex < 0 || typeIndex >= (int)mat.textureTypes.size())
        return -1;

    TextureFile& texFile = mat.textureTypes[(size_t)typeIndex];

    TextureInfo newTex = {};
    CopyToFixedChar(newTex.header.TextureKey, IM_ARRAYSIZE(newTex.header.TextureKey), textureKey);

    texFile.textures.push_back(newTex);
    return (int)texFile.textures.size() - 1;
}

bool CAdjuster::IsTextureKeyExistsInType(const MaterialInfo& mat, Engine::TEXTURE_TYPE type, const string& textureKey) const
{
    const int typeId = (int)type;

    for (const TextureFile& texFile : mat.textureTypes)
    {
        if ((int)texFile.header.typeID != typeId)
            continue;

        for (const TextureInfo& tex : texFile.textures)
        {
            const string key = FixedCharToString(tex.header.TextureKey, IM_ARRAYSIZE(tex.header.TextureKey));
            if (key == textureKey)
                return true;
        }
        return false;
    }
    return false;
}

string CAdjuster::ConvertToConstant(TEXTURE_TYPE type)
{
    switch (type)
    {
    case Engine::TEXTURE_TYPE::DIFFUSE: return "DiffuseTexture";
    case Engine::TEXTURE_TYPE::SPECULAR: return "SpecularTexture";
    case Engine::TEXTURE_TYPE::AMBIENT: return "AmbientTexture";
    case Engine::TEXTURE_TYPE::EMISSIVE: return "EmissiveTexture";
    case Engine::TEXTURE_TYPE::HEIGHT: return "HeightTexture";
    case Engine::TEXTURE_TYPE::NORMALS: return "NormalTexture";
    case Engine::TEXTURE_TYPE::SHININESS: return "ShinessTexture";
    case Engine::TEXTURE_TYPE::OPACITY: return "OpacityTexture";
    case Engine::TEXTURE_TYPE::DISPLACEMENT: return "DisplacementTexture";
    case Engine::TEXTURE_TYPE::LIGHTMAP: return "LightTexture";
    case Engine::TEXTURE_TYPE::REFLECTION: return "ReflectionTexture";
    case Engine::TEXTURE_TYPE::BASE_COLOR: return "BaseColorTexture";
    case Engine::TEXTURE_TYPE::NORMAL_CAMERA: return "NormalCameraTexture";
    case Engine::TEXTURE_TYPE::EMISSION_COLOR: return "EmmisionTexture";
    case Engine::TEXTURE_TYPE::METALNESS: return "MetalnessTexture";
    case Engine::TEXTURE_TYPE::DIFFUSE_ROUGHNESS: return "DiffuseRoughness";
    case Engine::TEXTURE_TYPE::AMBIENT_OCCLUSION: return "AmbientOcclusion";
    case Engine::TEXTURE_TYPE::SHEEN: return "SheenTexture";
    case Engine::TEXTURE_TYPE::CLEARCOAT: return "ClearCoatTexture";
    case Engine::TEXTURE_TYPE::TRANSMISSION: return "TransmissionTexture";
    case Engine::TEXTURE_TYPE::MAYA_BASE: return "MayaBaseTexture";
    case Engine::TEXTURE_TYPE::MAYA_SPECULAR: return "MayaSpecularTexture";
    case Engine::TEXTURE_TYPE::MAYA_SPECULAR_COLOR: return "MayaSpecularColorTexture";
    case Engine::TEXTURE_TYPE::MAYA_SPECULAR_ROUGHNESS: return "MayaSpecularRoughnessTexture";
    case Engine::TEXTURE_TYPE::ANISOTROPY: return "AnisotropyTexture";
    case Engine::TEXTURE_TYPE::GLTF_METALLIC_ROUGHNESS: return "GltfMetalicRoughnessTexture";
    case Engine::TEXTURE_TYPE::NOISE: return "NoiseTexture";
    case Engine::TEXTURE_TYPE::DISSOLVE: return "DissolveTexture";
    case Engine::TEXTURE_TYPE::ALPHA_MASK: return "AlphaMaskTexture";
    case Engine::TEXTURE_TYPE::DISTORTION: return "DistortionTexture";
    default: break;
    }
    return string();
}

// -------------------- Factory --------------------

CAdjuster* CAdjuster::Create()
{
    CAdjuster* instance = new CAdjuster();
    if (FAILED(instance->Initialize_Prototype()))
    {
        MSG_BOX("Object Create Failed : CAdjuster");
        Safe_Release(instance);
    }
    return instance;
}

CGameObject* CAdjuster::Clone(INIT_DESC* initDesc)
{
    CAdjuster* instance = new CAdjuster(*this);
    if (FAILED(instance->Initialize(initDesc)))
    {
        MSG_BOX("Object Clone Failed : CAdjuster");
        Safe_Release(instance);
    }
    return instance;
}
