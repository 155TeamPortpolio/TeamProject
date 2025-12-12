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

    void Import();
    void Export();
    void Play();
    void AddNode();
    void RemoveLastNode();
    void ContextClear();
    void LoadTextureFromDirectory(const string& dirPath);
    void DisplayAllTextures();
};

NS_END