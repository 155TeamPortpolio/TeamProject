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
    virtual void Submit_Opaque(const OPAQUE_PACKET& packet) PURE;
    virtual void Submit_Shadow(const OPAQUE_PACKET& packet) PURE;
    virtual void Submit_Shadow(const INSTANCE_PACKET& packet) PURE;
    virtual void Submit_Instance(const INSTANCE_PACKET& packet) PURE;
    virtual void Submit_UI(const SPRITE_PACKET& packet) PURE;
    virtual void Submit_UI3D(const OPAQUE_PACKET& packet) PURE;
    virtual void Submit_Debug(const DEBUG_PACKET& packet) PURE;
    virtual void Submit_Priority(const OPAQUE_PACKET& packet) PURE;
    virtual void Submit_Blend(const BLENDED_PACKET& packet) PURE;
    virtual void Submit_NonLight(const OPAQUE_PACKET& packet) PURE;
    virtual void Submit_Particle(const PARTICLE_PACKET& packet) PURE;
    virtual void Submit_Effect(const EFFECT_PACKET& packet) PURE;
    virtual void SetRimLightMode(RIMLIGHT eMode) PURE;

    virtual _bool GetOn() const  PURE;
    virtual void SetOn(_bool On) PURE;

public:
    virtual class CRenderer* GetRenderer(RENDERER_TYPE eType = RENDERER_TYPE::FORWARD) PURE;

public:
    virtual class CPipeLine* Get_Pipeline()PURE;

public:
   virtual void Add_RenderCommand(const RENDER_CUSTOM_COMMAND& command, CUSTOMTARGET eCustom) PURE;
    virtual void Add_PostProcessCommand(const POST_PROCESS_COMMAND& command)PURE;
    virtual ID3D11ShaderResourceView* Get_CustomTargetSRV(const string strTag) PURE;
    virtual ID3D11ShaderResourceView* Get_EngineTargetSRV(const string strTag) PURE;
#ifdef _USING_GUI
    virtual void Render_GUI() PURE;
#endif // _USING_GUI
    };
NS_END
