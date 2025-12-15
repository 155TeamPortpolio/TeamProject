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
    virtual void Submit_Debug(const DEBUG_PACKET& packet) PURE;
    virtual void Submit_Priority(const OPAQUE_PACKET& packet) PURE;
    virtual void Submit_Blend(const BLENDED_PACKET& packet) PURE;
    virtual void Submit_Particle(const PARTICLE_PACKET& packet) PURE;

    virtual HRESULT Get_InputLayout(class CModel* pModel, class CShader* pShader, _uint DrawIndex,
        const string& passConstant, ID3D11InputLayout** ppInputLayout) PURE;
    virtual  HRESULT Get_BufferInputLayout(class CVIBuffer* pBuffer, class CShader* pShader,
        const string& passConstant, ID3D11InputLayout** ppInputLayout)PURE;
    virtual class CPipeLine* Get_Pipeline()PURE;
public:
    virtual HRESULT Create_RenderTarget(const RenderTargetDesc& desc) PURE;
    virtual void Add_RenderCommand(const RENDER_CUSTOM_COMMAND& command) PURE;
    virtual void Add_PostProcessCommand(const POST_PROCESS_COMMAND& command)PURE;
    virtual void DrawTo(const string& targetKey, function<void(ID3D11DeviceContext*)> drawCall) PURE;
    virtual ID3D11ShaderResourceView* Get_CustomTargetSRV(const string strTag) PURE;
    virtual ID3D11ShaderResourceView* Get_EngineTargetSRV(const string strTag) PURE;
#ifdef _USING_GUI
    virtual void Render_GUI() PURE;
#endif // _USING_GUI
    };
NS_END
