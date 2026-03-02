#pragma once
#include "EffectContainer.h"

NS_BEGIN(Engine)
class CTexture;
NS_END

NS_BEGIN(EffectTool)

class CEffectContainer_Edit final:
    public CEffectContainer
{
public:
    typedef struct tagEffectEditContext
    {
        vector<string> TextureTags;
        vector<CTexture*> Textures;
        map<string, CTexture*>* pAllTextures;

        vector<string> ModelTags;
        vector<string> MaterialTags;
        _int iSelectModelIndex = -1;
        _int iSelectMaterialIndex = -1;

    }EFFECT_EDIT_CONTEXT;
private:
    CEffectContainer_Edit();
    CEffectContainer_Edit(const CEffectContainer_Edit& rhs);
    virtual ~CEffectContainer_Edit() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void Awake() override;
    void Priority_Update(_float dt) override;
    void Update(_float dt) override;
    void Late_Update(_float dt) override;

public:
    void Render_GUI() override;

public:
    static CEffectContainer_Edit* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;

private:
    vector<pair<string, string>> m_TextureKeyPath;
    map<string, CTexture*> m_Textures;
    vector<_int> m_SelectIndices;
    EFFECT_EDIT_CONTEXT m_Context{};

    /* For Edit */
    _bool m_OpenAttachBonePopup = false;
    _char m_BoneNameBuf[128] = ""; // 입력 버퍼
    string m_PendingBoneName; // 확정된 본 이름 (OK 눌렀을 때)

    _float3 m_vBoneOffsetPosition{};
    _float3 m_vBoneOffsetRotation{};

    void Import();
    void Export();
    void Play();
    void AttachToModel();
    void ReloadDirectory();
    void AddNode();
    void RemoveLastNode();
    void ContextClear();
    void LoadTextureFromDirectory(const string& dirPath, _bool isSRGB = false);
    void LoadMeshFromDirectory(const string& dirPath);
    void LoadMaterialFromDirectory(const string& dirPath);
    void DisplayAllTextures();
    void DisplayModels();
    void DisplayMaterial();
    void SetUp_EffectContainer();
};

NS_END