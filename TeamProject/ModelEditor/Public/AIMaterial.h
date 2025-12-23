#pragma once
#include "MaterialData.h"

NS_BEGIN(ModelEdit)

class CAIMaterial :
    public CMaterialData
{
private:
    CAIMaterial();
    virtual ~CAIMaterial() DEFAULT;
public:
    virtual HRESULT Initialize(const aiMaterial* pAIMaterial, const string& fileDirectory);
public:
    void Save_MaterialData(ID3D11DeviceContext* pContext, ofstream& ofs, const string& directory, const string& overrideKey = {});
    void Render_GUI(vector<_uint>& TextureIndexes) override;
    void LinkShader(const string& shader);

public:
    _bool Get_Specific() { return m_bSpecific; }

private:
    HRESULT LoadByAssimp(const string& fileDirectory, const aiMaterial* pAIMaterial);
    void ReCheck_Material(const string& fileDirectory);

private:
    void Render_MaterialAdd();
    void Add_AdditionalTexture(const string& fileDirectory, const string& preFix, const string& typeAdd, TEXTURE_TYPE type);

private:
    int m_currentPassIndex = {};
    _bool MaterialTabOpened = { false };
    int m_currentTextureTypeIndex = {};
    vector<_uint> textureTypes;
    vector<string> m_LogMsgs;
    _bool m_bSpecific = false;
public:
    static CAIMaterial* Create(const aiMaterial* pAIMaterial, const string& fileDirectory);
    static CAIMaterial* Create(const string& name);
    virtual void Free() override;
};

NS_END