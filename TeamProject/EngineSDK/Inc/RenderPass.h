#pragma once
#include "Engine_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)

class RenderPass abstract : public CBase {
protected:
	RenderPass(class CRenderSystem* pRenderSystem);
	virtual ~RenderPass() DEFAULT;
public:
	virtual void Execute(ID3D11DeviceContext* pContext) PURE;
protected:
	void BindConstant(ID3D11DeviceContext* pContext,class CModel* pModel, class CMaterial* pMaterial , _uint DrawIndex, _uint MaterialIndex);
	void BindConstant(ID3D11DeviceContext* pContext,class CSprite2D* pSprite , string passConstant);
protected:
	class CRenderSystem* m_pRenderSystem = { nullptr };
	class CShader* pCurShader = { nullptr };

public:
	virtual void Free();
};

#pragma region SHADOW_PASS
class ShadowPass final : public RenderPass {
private:
	ShadowPass(class CRenderSystem* pRenderSystem) :RenderPass{ pRenderSystem } {};
	virtual ~ShadowPass() DEFAULT;
public:
	void Execute(ID3D11DeviceContext* pContext) override;
	void Submit(OPAQUE_PACKET packet);
	void SubmitInstance(INSTANCE_PACKET packet);

private:
	void Execute_Opaque(ID3D11DeviceContext* pContext) ;
	void Execute_Instance(ID3D11DeviceContext* pContext) ;

private:
	vector<OPAQUE_PACKET> m_Packets;
	vector<OPAQUE_PACKET> m_VisiblePackets;
	vector<INSTANCE_PACKET> m_InstancePackets;
public:
	static ShadowPass* Create(class CRenderSystem* pRenderSystem) { return new ShadowPass(pRenderSystem); }
	virtual void Free() override { __super::Free(); m_Packets.clear(); };
};
#pragma endregion

#pragma region PRIORITY_PASS
class PriorityPass final : public RenderPass {
private:
	PriorityPass(class CRenderSystem* pRenderSystem) :RenderPass{ pRenderSystem } {};
	virtual ~PriorityPass() DEFAULT;
public:
	void Execute(ID3D11DeviceContext* pContext) override;
	void Submit(OPAQUE_PACKET packet);

private:
	vector<OPAQUE_PACKET> m_Packets;
	vector<OPAQUE_PACKET> m_VisiblePackets;
public:
	static PriorityPass* Create(class CRenderSystem* pRenderSystem) { return new PriorityPass(pRenderSystem); }
	virtual void Free() override { __super::Free(); m_Packets.clear(); };
};
#pragma endregion

#pragma region OPAQUE_PASS
class OpaquePass final : public RenderPass {
private:
	OpaquePass(class CRenderSystem* pRenderSystem) :RenderPass{ pRenderSystem } {};
	virtual ~OpaquePass() DEFAULT;
public:
	void Execute(ID3D11DeviceContext* pContext) override;
	void Submit(OPAQUE_PACKET packet);

private:
	vector<OPAQUE_PACKET> m_Packets;
	vector<OPAQUE_PACKET> m_VisiblePackets;

public:
	static OpaquePass* Create(class CRenderSystem* pRenderSystem) { return new OpaquePass(pRenderSystem); }
	virtual void Free() override {__super::Free(); m_Packets.clear();};
};
#pragma endregion


#pragma region INSTANCE_PASS
class InstancePass final : public RenderPass {
private:
	InstancePass(class CRenderSystem* pRenderSystem) :RenderPass{ pRenderSystem } {};
	virtual ~InstancePass() DEFAULT;
public:
	void Execute(ID3D11DeviceContext* pContext) override;
	void Submit(INSTANCE_PACKET packet);
private:
	vector<INSTANCE_PACKET> m_Packets;

public:
	static InstancePass* Create(class CRenderSystem* pRenderSystem) { return new InstancePass(pRenderSystem); }
	virtual void Free() override { __super::Free(); m_Packets.clear(); };
};
#pragma endregion



#pragma region BLENDED_PASS
class BlendedPass final : public RenderPass {
private:
	BlendedPass(class CRenderSystem* pRenderSystem) :RenderPass{ pRenderSystem } {};
	virtual ~BlendedPass() DEFAULT;
public:
	void Execute(ID3D11DeviceContext* pContext) override;
	void Submit(BLENDED_PACKET packet);

private:
	vector<BLENDED_PACKET> m_Packets;
	vector<BLENDED_PACKET> m_VisiblePackets;

public:
	static BlendedPass* Create(class CRenderSystem* pRenderSystem) { return new BlendedPass(pRenderSystem); }
	virtual void Free() override { __super::Free(); m_Packets.clear(); };
};
#pragma endregion

#pragma region PARTICLE_PASS
class ParticlePass final : public RenderPass {
	typedef struct tagParticleDrawData
	{
		_uint iOffset{};
		_uint iParticleCount{};
	}PARTICLE_DRAW_DATA;

private:
	ParticlePass(class CRenderSystem* pRenderSystem) : RenderPass{ pRenderSystem } {};
	virtual ~ParticlePass() DEFAULT;
public:
	void Execute(ID3D11DeviceContext* pContext) override;
	void Subimit(PARTICLE_PACKET packet);

private:
	vector<PARTICLE_PACKET> m_Packets;
	vector<VTX_INSTANCE_POINT> m_InstanceDatas;
	vector<PARTICLE_DRAW_DATA> m_DrawDatas;

public:
	static ParticlePass* Create(class CRenderSystem* pRenderSystem) { return new ParticlePass(pRenderSystem); }
	virtual void Free()override { __super::Free(); m_Packets.clear(); }

};
#pragma endregion


#pragma region UI_PASS
class UIPass final : public RenderPass {
private:
	UIPass(class CRenderSystem* pRenderSystem) :RenderPass{ pRenderSystem } {};
	virtual ~UIPass() DEFAULT;
public:
	void Execute(ID3D11DeviceContext* pContext) override;
	void Submit(SPRITE_PACKET packet);
private:
	vector<SPRITE_PACKET> m_Packets;
public:
	static UIPass* Create(class CRenderSystem* pRenderSystem) { return new UIPass(pRenderSystem); }
	virtual void Free() override { __super::Free(); m_Packets.clear(); };
};
#pragma endregion


#pragma region DEBUG_PASS
class DebugPass final : public RenderPass {
private:
	DebugPass(class CRenderSystem* pRenderSystem) :RenderPass{ pRenderSystem } {};
	virtual ~DebugPass() DEFAULT;
public:
	void Execute(ID3D11DeviceContext* pContext) override;
	void Submit(DEBUG_PACKET packet);
private:
	vector<DEBUG_PACKET> m_Packets;
public:
	static DebugPass* Create(class CRenderSystem* pRenderSystem) { return new DebugPass(pRenderSystem); }
	virtual void Free() override { __super::Free(); m_Packets.clear(); };
};
#pragma endregion
NS_END