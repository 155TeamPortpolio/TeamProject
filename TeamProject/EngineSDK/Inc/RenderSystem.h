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
	virtual void Submit_Debug(const DEBUG_PACKET& packet) override { m_pDebugPass->Submit(packet); };
	virtual void Submit_Blend(const BLENDED_PACKET& packet)override { m_pBlendedPass->Submit(packet); }; ;
	virtual void Submit_Particle(const PARTICLE_PACKET& packet)override { m_pParticlePass->Subimit(packet); }

public:
	HRESULT Render_LightAcc();
	HRESULT Render_Combined();
	HRESULT Render_Blended();
	HRESULT Render_Bright();
	void Render_Shadow();
	HRESULT Render_Final();
#ifdef _USING_GUI
	void Render_GUI();
#endif // _USING_GUI

#pragma region RenderTarget
public:
	virtual HRESULT Create_RenderTarget(const RenderTargetDesc& desc) override;
	virtual void Add_RenderCommand(const RENDER_CUSTOM_COMMAND& command) override;
	virtual void Add_PostProcessCommand(const POST_PROCESS_COMMAND& command)override;
	virtual void DrawTo(const string& targetKey, function<void(ID3D11DeviceContext*)> drawCall) override;
	virtual ID3D11ShaderResourceView* Get_CustomTargetSRV(const string strTag) override;
	virtual ID3D11ShaderResourceView* Get_EngineTargetSRV(const string strTag) override;

private:
	HRESULT Ready_GBuffer();
	void Process_RenderCommand();
	void Process_PostProcessQueue();
#pragma endregion

public:
	virtual HRESULT Get_InputLayout(class CModel* pModel, class CShader* pShader, _uint DrawIndex, 
		const string& passConstant,  ID3D11InputLayout** ppInputLayout) override;
	virtual  HRESULT Get_BufferInputLayout(class CVIBuffer* pBuffer, class CShader* pShader,
		const string& passConstant,  ID3D11InputLayout** ppInputLayout)override;
	class CPipeLine* Get_Pipeline() { return m_pPipeLine; }

private:

	HRESULT Change_Viewport(_uint iWidth, _uint iHeight);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = {nullptr};
	unordered_map<string, ID3D11InputLayout*> m_InputLayouts;

	/*PipeLine*/
	class CPipeLine* m_pPipeLine = { nullptr };

	/*RendetTarget*/
	class CTarget_Manager* m_pTargetManager = { nullptr };
	class CVIBuffer* m_pVIBuffer = { nullptr };
	class CShader* m_pShader = { nullptr };
	_float4x4				m_WorldMatrix;

	/*Pass*/
	PriorityPass* m_pPriorityPass = { nullptr};
	OpaquePass* m_pOpaquePass = { nullptr};
	ShadowPass* m_pShadowPass = { nullptr};
	InstancePass* m_pInstancePass = { nullptr};
	BlendedPass* m_pBlendedPass = { nullptr};
	ParticlePass* m_pParticlePass = { nullptr };
	UIPass* m_pUIPass = { nullptr };
	DebugPass* m_pDebugPass = { nullptr };

	vector<RENDER_CUSTOM_COMMAND> m_RenderCommands;
	vector<POST_PROCESS_COMMAND> m_PostCommands;
public:
	static CRenderSystem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END