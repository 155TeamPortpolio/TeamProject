#pragma once
#include "Component.h"
NS_BEGIN(Engine)

class ENGINE_DLL CMaterial :
    public CComponent
{
protected:
    CMaterial();
    CMaterial(const CMaterial& rhs);
    virtual ~CMaterial() DEFAULT;

public:
   virtual HRESULT Initialize_Prototype() override;
   virtual HRESULT Initialize(COMPONENT_DESC* pArg) override;

    HRESULT Link_Material(const string& levelKey, const string& materialKey);
    HRESULT Insert_MaterialInstance(class CMaterialInstance* pInstance, _uint* outIndex);
    HRESULT Set_RimLightInfo(_float3 RimColor, _float Power = 1.f);
    HRESULT Set_OutLineInfo(_float4 OutLineColor= _float4(0.f, 0.f, 0.f, 0.f), _float OutLineThickness = 0.02);

public:
    void Apply_Material(ID3D11DeviceContext* pContext, _uint Index);
    vector<class CMaterialInstance*>& Get_MaterialInstances() { return m_MaterialInstances; }
    class CMaterialInstance* Get_MaterialInstanceByName(const string& MaterialName);
    class CMaterialInstance* Get_MaterialInstance(_uint Index);

public:
    class CShader* Get_Shader(_uint Index);
    _uint Get_ShaderID(_uint Index);
    _uint Get_MaterialDataID(_uint Index);
    const string& GetPassConstant(_uint Index);
    CMaterialInstance* Find_MaterialByName(const string& MaterialName);

public:
    virtual void Render_GUI() override;

protected:
    vector<class CMaterialInstance*> m_MaterialInstances;
    _bool m_bMaterialTabOpen = {false};
    _float3 m_vRimLightColor = _float3(0, 0, 0);
    _float  m_fRimLightPower = 0.f;
    _float4 m_vOutLineColor = _float4(0.f, 0.f, 0.f, 0.f);
    _float m_fOutLineThickness = 0.02f;

public:
    static CMaterial* Create();
    virtual CComponent* Clone() override;
    virtual void Free() override;
};
NS_END
