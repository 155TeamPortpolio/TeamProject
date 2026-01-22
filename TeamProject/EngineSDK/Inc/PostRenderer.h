#pragma once
#include "Renderer.h"

NS_BEGIN(Engine)
class CTexture;
class CPostRenderer final :
    public CRenderer
{
private:
    CPostRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CPostRenderer();
    HRESULT Initialize(class CTarget_Manager* pTargetManager, class CPipeLine* pPipeLine);

public:
    void Set_FogDesc(FOG_DESC desc) { fogDesc = desc; };
    FOG_DESC Get_FogDesc() const { return fogDesc; };

public:
    HRESULT Render_HDRBloom();
    HRESULT Render_RadialBlur();
    HRESULT Render_Fog();
    HRESULT Render_Final();
    void    Add_PostProcessCommand(const POST_PROCESS_COMMAND& command);
    HRESULT Add_NoiseTexture(string strName, CTexture* noiseTexture);
    void    Apply_Noise(vector<string> strNames, _float duration);

    void    Apply_RadialBlur(_float duration,  _float2 center = _float2(0.5,0.5));
    void    Set_AddictiveColor(_float3* color);

    void    Register_AddictiveColor(_float3* pColor);
    void    UnRegister_AddictiveColor();

public:
    void Update(_float dt);

private:
    virtual HRESULT Ready_Target() override;
    virtual HRESULT Ready_MRT() override;
    HRESULT Bind_NoiseTexture();

private:
    HRESULT Process_PostProcessQueue();
    HRESULT Clear_PostProcess();

private:
    vector<POST_PROCESS_COMMAND> m_PostCommands;
    //*노이즈*
    unordered_map<string, CTexture*> m_pNoiseTextures;
    vector<CTexture*>   m_pApplyNoiseTextures;
    _float              m_fNoiseDuration;
    //*안개*
    FOG_DESC            fogDesc;
    //*블러*
    _float              m_fRadialDuration = 0.f;
    _float              m_fRadialTotalDuration = 0.f;
    _float3*            m_pAddictiveColor = nullptr; //_float3(0.1, 0.54, 0.58)
    _float2             m_fRadialCenter = _float2(0.5, 0.5);

    _float              m_fScreenWidth;
    _float              m_fScreenHeight;

public:
    static CPostRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
        CTarget_Manager* pTargetManager, CPipeLine* pPipeLine);
    virtual void Free() override;

};

NS_END
