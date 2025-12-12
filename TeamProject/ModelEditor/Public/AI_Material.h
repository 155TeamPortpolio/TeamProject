#pragma once
#include "Material.h"
NS_BEGIN(ModelEdit)
class CAI_Material :
    public CMaterial
{
private:
    CAI_Material();
    CAI_Material(const CAI_Material& rhs);
    virtual ~CAI_Material() DEFAULT;
public:
    HRESULT Initialize();
    virtual void Render_GUI() override;
    HRESULT Load_Material(_uint materialNum, aiMaterial* material[], const string& filePath);
    HRESULT Save_Material();
    HRESULT Save_Material(const string& SavePath, const string& fileName);
    void LinkShader(const string& shader);
    void Override_ShaderKey(const string& overrideKey) { m_OverrideShaderKey = overrideKey; };

private:
    vector<class CAIMaterial*> m_AIMaterialDatas;
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    string m_MaterialFileKey = {};
    string m_OverrideShaderKey = {};

public:
    static CAI_Material* Create();
    virtual CComponent* Clone() override;
    virtual void Free() override;
};

NS_END