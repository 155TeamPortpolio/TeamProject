#pragma once
#include "IService.h"
NS_BEGIN(Engine)
class ENGINE_DLL IRenderService abstract:
    public IService
{
protected:
    virtual  ~IRenderService() DEFAULT;
public:
    virtual HRESULT Render() PURE;
    virtual void Submit_StaticMesh_Opaque(const OPAQUE_PACKET& packet) PURE;
    virtual void Submit_SkinnedMesh_Opaque(const OPAQUE_PACKET& packet) PURE;
    virtual void Submit_StaticShadow(const OPAQUE_PACKET& packet) PURE;
    virtual void Submit_SkinnedShadow(const OPAQUE_PACKET& packet) PURE;
    virtual void Submit_Instance(const INSTANCE_PACKET& packet) PURE;
    virtual void Submit_UI(const SPRITE_PACKET& packet) PURE;
    virtual void Submit_UI3D(const OPAQUE_PACKET& packet) PURE;
    virtual void Submit_Priority(const OPAQUE_PACKET& packet) PURE;
    virtual void Submit_Blend(const BLENDED_PACKET& packet) PURE;
    virtual void Submit_NonLight(const OPAQUE_PACKET& packet) PURE;
    virtual void Submit_Particle(const PARTICLE_PACKET& packet) PURE;
    virtual void Submit_Effect(const EFFECT_PACKET& packet) PURE;
    virtual void SetRimLightMode(RIMLIGHT eMode) PURE;

    virtual _bool GetOn() const  PURE;
    virtual void SetOn(_bool On) PURE;

    virtual void Update(_float dt) PURE;

public:
    virtual class CRenderer* GetRenderer(RENDERER_TYPE eType = RENDERER_TYPE::FORWARD) PURE;
    virtual HRESULT Create_RenderTarget(const RenderTargetDesc& desc) PURE;
    virtual void Set_FogDesc(FOG_DESC desc)PURE;
    virtual _bool Get_FogDesc(FOG_DESC& outResult) PURE;

    virtual void Set_NoiseTexture(NOISE_FXTYPE eNoise, class CTexture* noiseTexture) PURE;
    virtual CTexture* Get_NoiseTexture(NOISE_FXTYPE eNoise) PURE;

    virtual void Apply_RadialBlur(_float duration, _float2 center = _float2(0.5, 0.5)) PURE;
    virtual void Register_AddictiveColor(_float3* pColor)   PURE;
    virtual void UnRegister_AddictiveColor()                PURE;

public:
    virtual class CPipeLine* Get_Pipeline()PURE;

public:
   virtual void Add_RenderCommand(const RENDER_CUSTOM_COMMAND& command, CUSTOMTARGET eCustom) PURE;
   virtual void Add_OutLineCommand(const OUTLINE_COMMAND& command)PURE;
   virtual void Add_MotionBlurCommand(const MOTIONBLUR_COMMAND& command) PURE;
   virtual void Add_PostProcessCommand(const POST_PROCESS_COMMAND& command)PURE;
    virtual ID3D11ShaderResourceView* Get_CustomTargetSRV(const string strTag) PURE;
    virtual ID3D11ShaderResourceView* Get_EngineTargetSRV(const string strTag) PURE;
#ifdef _USING_GUI
    virtual void Render_GUI() PURE;
#endif // _USING_GUI
    };
NS_END
