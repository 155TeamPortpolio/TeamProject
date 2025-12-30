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

public:
    HRESULT Render_EffectBloom();
    HRESULT Render_HDRBloom();
    HRESULT Render_Distortion();
    HRESULT Render_Fog();
    HRESULT Render_Final();
    void Add_PostProcessCommand(const POST_PROCESS_COMMAND& command);
    HRESULT Add_NoiseTexture(string strName, CTexture* noiseTexture);
    void Apply_Noise(vector<string> strNames, _float duration);

private:
    virtual HRESULT Ready_Target() override;
    virtual HRESULT Ready_MRT() override;
    HRESULT Bind_NoiseTexture();

private:
    HRESULT Process_PostProcessQueue();
    HRESULT Clear_PostProcess();

private:
    ID3D11ShaderResourceView* m_pDistortionNoiseTexture = { nullptr };
    vector<POST_PROCESS_COMMAND> m_PostCommands;

    unordered_map<string, CTexture*> m_pNoiseTextures;
    vector<CTexture*> m_pApplyNoiseTextures;
    _float m_fNoiseDuration;
    FOG_DESC fogDesc;

public:
    static CPostRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
        CTarget_Manager* pTargetManager, CPipeLine* pPipeLine);
    virtual void Free() override;

};

NS_END
