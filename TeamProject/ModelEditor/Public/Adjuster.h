#pragma once
#include "GameObject.h"

#include <string>
#include <vector>

NS_BEGIN(Engine)
class CTexture;
NS_END

NS_BEGIN(ModelEdit)

class CAdjuster : public CGameObject
{
protected:
    CAdjuster();
    CAdjuster(const CAdjuster& rhs);
    virtual ~CAdjuster() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* initDesc) override;

    void Priority_Update(_float deltaTime) override;
    void Update(_float deltaTime) override;
    void Late_Update(_float deltaTime) override;

public:
    void Render_GUI() override;

public:
    static CAdjuster* Create();
    CGameObject* Clone(INIT_DESC* initDesc) override;
    void Free() override;

private:
    // ---- UI root ----
    void Render_AdjustTab();
    void DrawTopToolbar();
    void DrawModelPane(float width, float height);
    void DrawMaterialPane(float width, float height);
    int GetSelectedMeshMaterialIndex() const;
    void DrawDetailPane(float width, float height);
    void DrawSplitter(const char* id, bool isVertical, float& size, float minSize, float maxSize);

private:
    // ---- IO / linking ----
    HRESULT Load_MaterialFile(const std::string& fileKey, const std::string& path, MaterialFile& outFile);
    HRESULT Save_MaterialFile(const std::string& path, const MaterialFile& file);

    HRESULT Load_ModelHeaderAndMeshViews(const std::string& path, CombinedAsset& inOut);
    HRESULT Save_ModelFile_UpdateMaterialIndex(const std::string& path, const CombinedAsset& inOut);

    void Link_ModelToMaterial(CombinedAsset& inOut);
    void RebuildMaterialCounts();

private:
    // ---- Material ops ----
    void Material_Up(int index);
    void Material_Down(int index);
    void Move_MaterialIndex(int fromIndex, int toIndex);

    void Material_Duplicate(int sourceIndex);
    void Material_Delete(int deleteIndex);

    // ---- Texture ops ----
    void Texture_DeleteSelected();
    void TextureType_RemoveIfEmpty();
    void UpdatePreviewTextureIfNeeded();

private:
    // ---- Mapping / cache / DnD ----
    struct MaterialCentricCache
    {
        std::vector<std::vector<int>> meshesByMaterial;
        std::vector<int> useCount;
    };

    void RebuildMaterialCentricCache();

    void RequestScrollToMesh(int meshIndex);
    void RequestScrollToMaterial(int materialIndex);

    void ApplyMeshMaterialMapping(int meshIndex, int materialIndex);

    bool BeginDragMaterial(int materialIndex);
    bool BeginDragMesh(int meshIndex);

    bool AcceptDropMaterialToMesh(int targetMeshIndex);
    bool AcceptDropMeshToMaterial(int targetMaterialIndex);

    void DrawMaterialUseTooltip(int materialIndex);

private:
    std::string ConvertToConstant(TEXTURE_TYPE type);

private:
    // ---- UI state ----
    float m_ModelPaneWidth = 320.0f;
    float m_MaterialPaneWidth = 380.0f;

    char m_ModelFilter[128] = {};
    char m_MatFilter[128] = {};
    char m_TexFilter[128] = {};

    bool m_ShowDropHints = true;

    int m_HoverMeshIndex = -1;
    int m_HoverMaterialIndex = -1;

    bool m_RequestScrollToMesh = false;
    int  m_ScrollToMeshIndex = -1;

    bool m_RequestScrollToMaterial = false;
    int  m_ScrollToMaterialIndex = -1;

private:
    // ---- Selection ----
    int m_SelectedMesh = -1;
    size_t m_SelectedMaterial = 0;
    size_t m_SelectedType = 0;
    size_t m_SelectedTexture = 0;

private:
    // Rename buffer (선택 변경시만 갱신)
    char m_TexRenameBuf[256] = {};
    size_t m_LastRenameMaterial = (size_t)-1;
    size_t m_LastRenameType = (size_t)-1;
    size_t m_LastRenameTex = (size_t)-1;

private:
    // Preview cache
    CTexture* m_PreviewTex = nullptr;
    std::string m_PreviewTexKey;

private:
    CombinedAsset m_Combined{};
    MaterialCentricCache m_MappingCache{};

    _bool isTabOpen = { false };
};

NS_END
