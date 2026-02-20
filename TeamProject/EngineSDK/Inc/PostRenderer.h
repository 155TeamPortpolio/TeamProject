#pragma once
#include "Renderer.h"

NS_BEGIN(Engine)
class CTexture;
class ENGINE_DLL CPostRenderer final :
    public CRenderer
{
private:
    CPostRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CPostRenderer();
    HRESULT Initialize(class CTarget_Manager* pTargetManager, class CPipeLine* pPipeLine);

public:
    template<typename T>
    T* GetCommand()
    {
        auto it = m_CommandMap.find(typeid(T));

        if (it != m_CommandMap.end())
            return dynamic_cast<T*>(it->second);

        return nullptr;
    }

public:
    HRESULT Render_PostProcessCommand();
    HRESULT Render_HDRBloom_Internal();
    HRESULT Render_RadialBlur_Internal();
    HRESULT Render_Fog_Internal();
    HRESULT Render_Addictive_Internal();
    HRESULT Render_Glitch_Internal();
    HRESULT Render_GuassianBlur_Internal();
    HRESULT Render_Saturation_Internal();
    HRESULT Render_Distortion_Internal();
    HRESULT Render_Flare_Internal();
    HRESULT Render_Final();

public:
    void Update(_float dt);

private:
    virtual HRESULT Ready_Target() override;
    virtual HRESULT Ready_MRT() override;

private:
    class CHDRBloomCommand*             m_pHDRBloomCommand;
    class CGlitchCommand*               m_pGlitchCommand;
    class CRadialBlurCommand*           m_pRadialBlurCommand;
    class CFogCommand*                  m_pFogCommand;
    class CGuassianBlurCommand*         m_pGuassianBlurCommand;
    class CAddictiveColorCommand*       m_pAddictiveColorCommand;
    class CSaturationCommand*           m_pSaturationCommand;
    class CDistortionCommand*           m_pDistortionCommand;
    class CFlareCommand*                m_pFlareCommand;

    map<type_index, class CPostProcessCommand*> m_CommandMap;

private:
    _float              m_fScreenWidth;
    _float              m_fScreenHeight;
    
    _float              m_fAccTime = 0.f;
    string              m_strLastTargetName = "Target_Final";

public:
    static CPostRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
        CTarget_Manager* pTargetManager, CPipeLine* pPipeLine);
    virtual void Free() override;

};

NS_END
