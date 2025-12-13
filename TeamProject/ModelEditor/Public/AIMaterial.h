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
    void Render_GUI() override;
    void Render_GUI(vector<_uint>& TextureIndexes) override;
    void LinkShader(const string& shader);

private:
    void Render_MaterialAdd();

private:
    int m_currentPassIndex = {};
    _bool MaterialTabOpened = { false };
    int m_currentTextureTypeIndex = {};
    vector<_uint> textureTypes;

public:
    static CAIMaterial* Create(const aiMaterial* pAIMaterial, const string& fileDirectory);
    virtual void Free() override;
};

NS_END