#pragma once
#include "IRenderService.h"
#include "RenderPass.h"

NS_BEGIN(Engine)

class CRenderSystem final : public IRenderService
{
private:
	CRenderSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderSystem();
	HRESULT Initialize();

public: 
	virtual HRESULT Render() override;
	virtual void Submit_Opaque(const OPAQUE_PACKET& packet) override { m_pOpaquePass->Submit(packet); };
	virtual void Submit_Shadow(const OPAQUE_PACKET& packet) override { m_pShadowPass->Submit(packet); };
	virtual void Submit_Shadow(const INSTANCE_PACKET& packet) override { m_pShadowPass->SubmitInstance(packet); };
	virtual void Submit_Instance(const INSTANCE_PACKET& packet) override { m_pInstancePass->Submit(packet); };
	virtual void Submit_Priority(const OPAQUE_PACKET& packet) override { m_pPriorityPass->Submit(packet); };
	virtual void Submit_UI(const SPRITE_PACKET& packet) override {m_pUIPass->Submit(packet);};
	virtual void Submit_UI3D(const OPAQUE_PACKET& packet)override { m_pUI3DPass->Submit(packet); }
	virtual void Submit_Debug(const DEBUG_PACKET& packet) override { m_pDebugPass->Submit(packet); };
	virtual void Submit_Blend(const BLENDED_PACKET& packet)override { m_pBlendedPass->Submit(packet); }; ;
	virtual void Submit_NonLight(const OPAQUE_PACKET& packet)override { m_pNonLightPass->Submit(packet); };
	virtual void Submit_Particle(const PARTICLE_PACKET& packet)override { m_pParticlePass->Submit(packet); }
	virtual void Submit_Effect(const EFFECT_PACKET& packet)override { m_pEffectPass->Submit(packet); }

public:
	virtual class CRenderer* GetRenderer(RENDERER_TYPE eType) override;
	virtual void SetRimLightMode(RIMLIGHT eMode) override;
public:
	virtual _bool GetOn() const  override{ return IsOn; }
	virtual void SetOn(_bool On) override { IsOn = On; }

#ifdef _USING_GUI
	void Render_GUI();
#endif // _USING_GUI

#pragma region RenderTarget
public:
	virtual void Add_RenderCommand(const RENDER_CUSTOM_COMMAND& command, CUSTOMTARGET eCustom) override;
	virtual void Add_OutLineCommand(const OUTLINE_COMMAND& command) override;
	virtual void Add_PostProcessCommand(const POST_PROCESS_COMMAND& command)override;
	virtual ID3D11ShaderResourceView* Get_CustomTargetSRV(const string strTag) override;
	virtual ID3D11ShaderResourceView* Get_EngineTargetSRV(const string strTag) override;
#pragma endregion

public:
	class CPipeLine* Get_Pipeline() { return m_pPipeLine; }

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = {nullptr};
	class CPipeLine* m_pPipeLine = { nullptr };
	class CTarget_Manager* m_pTargetManager = { nullptr };

	PriorityPass* m_pPriorityPass = { nullptr};
	OpaquePass* m_pOpaquePass = { nullptr};
	ShadowPass* m_pShadowPass = { nullptr};
	InstancePass* m_pInstancePass = { nullptr};
	BlendedPass* m_pBlendedPass = { nullptr};
	NonLightPass* m_pNonLightPass = { nullptr };
	ParticlePass* m_pParticlePass = { nullptr };
	UIPass* m_pUIPass = { nullptr };
	DebugPass* m_pDebugPass = { nullptr };
	UI3DPass* m_pUI3DPass = { nullptr };
	EffectPass* m_pEffectPass = { nullptr };

	class CForwardRenderer* m_pForward;
	class CPostRenderer* m_pPost;
	class CUIRenderer* m_pUI;
	class CEffectRenderer* m_pEffect;

	_bool IsOn = true;

public:
	static CRenderSystem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END