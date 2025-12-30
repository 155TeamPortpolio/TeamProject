#pragma once
#include "Component.h"
NS_BEGIN(Engine)
class CTexture; class CShader; class CVI_Point; class C_VIBuffer;

class ENGINE_DLL CSprite2D final : public CComponent
{
private:
    CSprite2D() {}
    CSprite2D(const CSprite2D& rhs);
    virtual ~CSprite2D() {}
public:
    virtual HRESULT Initialize_Prototype()           override;
    virtual HRESULT Initialize(COMPONENT_DESC* pArg) override;

public:
    /*텍스처는 부모 폴더와 함께 ParentName + TextureKey*/
    HRESULT         Add_Texture(const string& levelKey, const string& texKey);
    HRESULT         Change_Texture(_uint index, const string& levelKey, const string& texKey);
    HRESULT         Link_Shader(const string& levelKey, const string& shaderKey);
    HRESULT         ChangePass(const string& passConstant);
    HRESULT         Set_TextKey(const string& textKey);
    const string&   Get_TextKey() { return m_TextKey; }
    CTexture*       Get_Texture(_uint idx) { return m_pTextures[idx]; }
    CTexture*       Get_CurTexture()       { return m_pTextures[m_iDrawIndex];  }

public:
    void    Apply_Shader(ID3D11DeviceContext* pContext);
    void    Draw_Sprite(ID3D11DeviceContext* pContext);
    HRESULT ChangeSprite(_uint Index);

    _bool   HitTest_AlphaUV(_float u, _float v, _float alphaThreshold);

public:
    CShader*   Get_Shader()       { return m_pShader; }
    CVIBuffer* Get_Buffer();
    const string&    Get_PassConstant() { return m_PassConstant; }

    HRESULT       Set_Param(const string& ConstantName, const SHADER_PARAM& parameter);
    SHADER_PARAM* Get_Param(const string& ConstantName);

public:
    _bool IsValid();
    void  Render_GUI();

private:
    _uint  m_iDrawIndex   = {};
    string m_PassConstant = {"Opaque"};
    string m_TextKey      = "";

    CVI_Point* m_pPoint  = { nullptr };
    CShader*   m_pShader = { nullptr };
    vector<CTexture*> m_pTextures;
    unordered_map<string, SHADER_PARAM> m_DynamicSlots;

public:
    static CSprite2D* Create();
    virtual CComponent* Clone() override { return new CSprite2D(*this); }
    virtual void Free() override;
};
NS_END

