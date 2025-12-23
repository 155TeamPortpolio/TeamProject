#pragma once
#include "Renderer.h"

NS_BEGIN(Engine)

class CPostRenderer final :
    public CRenderer
{
private:
    CPostRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CPostRenderer();
    HRESULT Initialize(class CTarget_Manager* pTargetManager, class CPipeLine* pPipeLine);

public:
    HRESULT Render_EffectBloom();
    HRESULT Render_HDRBloom();
    HRESULT Render_Distortion();
    HRESULT Downsample_Bloom(string strTarget, string strMRT, _float width, _float height);
    HRESULT Upsample_Bloom(string strTarget, string addTarget, string strMRT, _float width, _float height);
    HRESULT Render_Final();
    void Add_PostProcessCommand(const POST_PROCESS_COMMAND& command);

private:
    virtual HRESULT Ready_Target() override;
    virtual HRESULT Ready_MRT() override;

private:
    HRESULT CreateDistortionNoiseTexture();
    HRESULT Process_PostProcessQueue();
    HRESULT Clear_PostProcess();

private:
    ID3D11ShaderResourceView* m_pDistortionNoiseTexture = { nullptr };
    vector<POST_PROCESS_COMMAND> m_PostCommands;

public:
    static CPostRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
        CTarget_Manager* pTargetManager, CPipeLine* pPipeLine);
    virtual void Free() override;

};

NS_END
