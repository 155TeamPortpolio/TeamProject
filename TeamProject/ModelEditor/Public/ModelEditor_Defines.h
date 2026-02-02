#pragma once
#include <windows.h>
#include <d3d11.h>


namespace ModelEdit {
	inline unsigned int g_iWinSizeX = 2560;
	inline unsigned int g_iWinSizeY = 1360;
	inline unsigned int g_iMainFrame = 144;
}

extern HWND g_hWnd;
extern HINSTANCE g_hInstance; 


using namespace ModelEdit;

#include "Engine_Defines.h"   // 엔진 디버그 new 세팅이 여기서 완료된 상태

#pragma push_macro("new")
#undef new

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#pragma pop_macro("new")

struct TextureInfo
{
    TEXTURE_INFO_HEADER header;
};

struct TextureFile
{
    TEXTURE_FILE_HEADER header;
    vector<TextureInfo> textures;
};

struct MaterialInfo
{
    MATERIAL_INFO_HEADER header;
    vector<TextureFile> textureTypes;
};

struct MaterialFile
{
    MATERIAL_FILE_HEADER header;
    vector<MaterialInfo> materials;
};

static bool HasExtension(const string& path, const char* ext)
{
    return std::filesystem::path(path).extension() == ext;
}

struct MeshView
{
    string meshName;
    _int materialIndex = -1;
    _uint verticesCount = 0;
    _uint indicesCount = 0;

    MaterialInfo* linkedMaterial = nullptr;

    std::streamoff meshHeaderOffset = 0;   // 파일에서 MESH_INFO_HEADER 시작 위치
    MESH_INFO_HEADER meshHeaderRaw{};      // 읽어온 원본 헤더(수정 후 다시 써도 됨)
};

// 안전한 c-string 복사 (헤더가 char 배열이면)
template<size_t N>
static void CopyCStr(char(&dst)[N], const string& src)
{
    memset(dst, 0, N);
#ifdef _MSC_VER
    strncpy_s(dst, src.c_str(), N - 1);
#else
    strncpy(dst, src.c_str(), N - 1);
#endif
}

struct CombinedAsset
{
    string modelPath;
    string materialPath;

    MODEL_FILE_HEADER modelHeader{};
    vector<MeshView> meshes;

    MaterialFile materialFile{};

    bool loadedModel = false;
    bool loadedMaterial = false;
    bool reverseMeshOrder = false; // "역순" 필요하면 true
};

static const Engine::TEXTURE_TYPE g_TextureTypeList[] =
{
    Engine::TEXTURE_TYPE::DIFFUSE,
    Engine::TEXTURE_TYPE::SPECULAR,
    Engine::TEXTURE_TYPE::AMBIENT,
    Engine::TEXTURE_TYPE::EMISSIVE,
    Engine::TEXTURE_TYPE::NORMALS,
    Engine::TEXTURE_TYPE::OPACITY,
    Engine::TEXTURE_TYPE::NOISE,
    Engine::TEXTURE_TYPE::DISSOLVE,
    Engine::TEXTURE_TYPE::DISTORTION,
    Engine::TEXTURE_TYPE::ALPHA_MASK,
    Engine::TEXTURE_TYPE::BASE_COLOR,
    Engine::TEXTURE_TYPE::METALNESS,
    Engine::TEXTURE_TYPE::AMBIENT_OCCLUSION,
    Engine::TEXTURE_TYPE::GLTF_METALLIC_ROUGHNESS,
};
