#include "Engine_Defines.h"
#include "RenderPass.h"
#include "RenderSystem.h"
#include "PipeLine.h"
#include "Model.h"
#include "Material.h"
#include "Transform.h"
#include "Shader.h"
#include "Animator3D.h"
#include "SkeletonFollower.h"
#include "GameInstance.h"
#include "IResourceService.h"
#include "DebugRender.h"
#include "SkeletalModel.h"
#include "InstanceModel.h"
#include "Sprite2D.h"
#include "VIBuffer.h"
#include "VI_InstancePoint.h"
#include "ParticleSystem.h"
#include "Renderer.h"

RenderPass::RenderPass(CRenderSystem* pRenderSystem)
	:m_pRenderSystem(pRenderSystem)
{
}

void RenderPass::BindConstant(ID3D11DeviceContext* pContext, CModel* pModel, CMaterial* pMaterial, _uint DrawIndex, _uint MaterialIndex, CRenderer* pRenderer)
{
	CPipeLine* pPipeLine = m_pRenderSystem->Get_Pipeline();
	pCurShader = pMaterial->Get_Shader(MaterialIndex);
	pCurShader->SetConstantBuffer("FrameBuffer", pPipeLine->Get_FrameBuffer());

	SHADER_PARAM ObjectMaticedParam = {};
	ObjectMaticedParam.iSize = sizeof(_float4x4) * g_iMaxTransform;
	ObjectMaticedParam.typeName = "StructuredBuffer";
	ObjectMaticedParam.pData = pPipeLine->Get_ObjectResource();
	pCurShader->Bind_Value("ObjectBufferArray", ObjectMaticedParam);

	SHADER_PARAM SkinningMatricedParam = {};
	SkinningMatricedParam.iSize = sizeof(_float4x4) * g_iMaxNumBones;
	SkinningMatricedParam.typeName = "StructuredBuffer";
	SkinningMatricedParam.pData = pPipeLine->Get_SkinningResource();
	pCurShader->Bind_Value("g_BoneMatrices", SkinningMatricedParam);

	ID3D11InputLayout* pLayout;
	pRenderer->Get_InputLayout(pModel, pCurShader, DrawIndex, pMaterial->GetPassConstant(MaterialIndex), &pLayout);
	pContext->IASetInputLayout(pLayout);
}

void RenderPass::BindConstant(ID3D11DeviceContext* pContext,  CSprite2D* pSprite, string passConstant, class CRenderer* pRenderer)
{
	CPipeLine* pPipeLine = m_pRenderSystem->Get_Pipeline();
	pCurShader = pSprite->Get_Shader();
	pCurShader->SetConstantBuffer("FrameBuffer", pPipeLine->Get_FrameBuffer());

	SHADER_PARAM ObjectMaticedParam = {};
	ObjectMaticedParam.iSize = sizeof(_float4x4) * g_iMaxTransform;
	ObjectMaticedParam.typeName = "StructuredBuffer";
	ObjectMaticedParam.pData = pPipeLine->Get_ObjectResource();

	pCurShader->Bind_Value("ObjectBufferArray", ObjectMaticedParam);

	ID3D11InputLayout* pLayout;
	pRenderer->Get_BufferInputLayout(pSprite->Get_Buffer(), pCurShader, passConstant, &pLayout);
	pContext->IASetInputLayout(pLayout);
}


void RenderPass::Free()
{
}

#pragma region OPAQUE_PASS
void OpaquePass::Execute(ID3D11DeviceContext* pContext, CRenderer* pRenderer)
{
	/*이건 전역적으로 셰이더에 값 넣어주는 역할*/
	CPipeLine* pPipeLine = m_pRenderSystem->Get_Pipeline();
	pCurShader = { nullptr };

	/*같은 셰이더, 같은 머티리얼, 같은 모델끼리 정렬 */
	sort(m_Packets.begin(), m_Packets.end(),
		[](const OPAQUE_PACKET& a, const OPAQUE_PACKET& b) {
			return a.GetKey() < b.GetKey();
		});

	/*패킷이 비어 있으면 리턴*/
	if (m_Packets.empty())
		return;
	
	/*상수 버퍼 및 SRV 세팅*/
	pPipeLine->Begin_ObjectBuffer(pContext);
	pPipeLine->Begin_SkinningBuffer(pContext);
	  
	for (auto& packet : m_Packets)     
	{
		if (!packet.bSkinning) {
			if (!pPipeLine->isVisible(packet.pModel->Get_MeshBoundingBox(packet.DrawIndex), XMLoadFloat4x4(packet.pWorldMatrix)))
				continue;
		}
		//여기서 인덱스 추가 저장해줌
		_uint TransformIndex = pPipeLine->Write_ObjectData(*packet.pWorldMatrix);
		_uint SkinningOffset = 0;
		if (packet.bSkinning) {
			if (holds_alternative<CAnimator3D*>(packet.pPayLoad))
				SkinningOffset = pPipeLine->Write_SkinningBuffer(get<CAnimator3D*>(packet.pPayLoad)->Get_BoneMatrices(packet.DrawIndex));
			else if (holds_alternative<CSkeletonFollower*>(packet.pPayLoad))
				SkinningOffset = pPipeLine->Write_SkinningBuffer(get<CSkeletonFollower*>(packet.pPayLoad)->Get_BoneMatrices(packet.DrawIndex));
			else
				SkinningOffset = pPipeLine->Write_SkinningBuffer(dynamic_cast<CSkeletalModel*>(packet.pModel)->Get_BoneMatrices(packet.DrawIndex));
		}

		packet.TransformIndex = TransformIndex;
		packet.SkinningOffset = SkinningOffset;

		m_VisiblePackets.push_back(packet);
	}

	pPipeLine->End_ObjectBuffer(pContext);
	pPipeLine->End_SkinningBuffer(pContext);

	/*드로우콜 시작*/
	for (auto& packet : m_VisiblePackets)
	{
		if (packet.pMaterial->Get_Shader(packet.MaterialIndex) != pCurShader) {
			BindConstant(pContext, packet.pModel, packet.pMaterial, packet.DrawIndex, packet.MaterialIndex, pRenderer);
		}

		SHADER_PARAM WorldMatParam{ &packet.TransformIndex, "uint",sizeof(UINT) };
		pCurShader->Bind_Value("TransformIndex", WorldMatParam);

		if (packet.bSkinning) {
			SHADER_PARAM SkinningBoneParam{ &packet.SkinningOffset , "uint",sizeof(UINT) };
			pCurShader->Bind_Value("SkinningOffset", SkinningBoneParam);
		}

		packet.pMaterial->Apply_Material(pContext, packet.MaterialIndex);
		packet.pModel->Draw(pContext, packet.DrawIndex);
	}

	m_Packets.clear();
	m_VisiblePackets.clear();
}

void OpaquePass::Submit(OPAQUE_PACKET packet)
{
	if (packet.pModel == nullptr || packet.pMaterial == nullptr) return;
	m_Packets.push_back(packet);
}

#pragma endregion

#pragma region PRIORITY_PASS

void PriorityPass::Execute(ID3D11DeviceContext* pContext, CRenderer* pRenderer)
{
	/*이건 전역적으로 셰이더에 값 넣어주는 역할*/
	CPipeLine* pPipeLine = m_pRenderSystem->Get_Pipeline();
	pCurShader = { nullptr };

	/*같은 셰이더, 같은 머티리얼, 같은 모델끼리 정렬 */
	sort(m_Packets.begin(), m_Packets.end(),
		[](const OPAQUE_PACKET& a, const OPAQUE_PACKET& b) {
			return a.GetKey() < b.GetKey();
		});

	/*패킷이 비어 있으면 리턴*/
	if (m_Packets.empty())
		return;

	/*상수 버퍼 및 SRV 세팅*/
	pPipeLine->Begin_ObjectBuffer(pContext);
	pPipeLine->Begin_SkinningBuffer(pContext);

	for (auto& packet : m_Packets)
	{
		//여기서 인덱스 추가 저장해줌
		_uint TransformIndex = pPipeLine->Write_ObjectData(*packet.pWorldMatrix);
		_uint SkinningOffset = 0;
		if (packet.bSkinning) {
			if (holds_alternative<CAnimator3D*>(packet.pPayLoad))
				SkinningOffset = pPipeLine->Write_SkinningBuffer(get<CAnimator3D*>(packet.pPayLoad)->Get_BoneMatrices(packet.DrawIndex));
			else if (holds_alternative<CSkeletonFollower*>(packet.pPayLoad))
				SkinningOffset = pPipeLine->Write_SkinningBuffer(get<CSkeletonFollower*>(packet.pPayLoad)->Get_BoneMatrices(packet.DrawIndex));
			else
				SkinningOffset = pPipeLine->Write_SkinningBuffer(dynamic_cast<CSkeletalModel*>(packet.pModel)->Get_BoneMatrices(packet.DrawIndex));
		}

		packet.TransformIndex = TransformIndex;
		packet.SkinningOffset = SkinningOffset;

		m_VisiblePackets.push_back(packet);
	}

	pPipeLine->End_ObjectBuffer(pContext);
	pPipeLine->End_SkinningBuffer(pContext);

	/*드로우콜 시작*/
	for (auto& packet : m_VisiblePackets)
	{
		if (packet.pMaterial->Get_Shader(packet.MaterialIndex) != pCurShader) {
			BindConstant(pContext, packet.pModel, packet.pMaterial, packet.DrawIndex, packet.MaterialIndex, pRenderer);
		}

		SHADER_PARAM WorldMatParam{ &packet.TransformIndex, "uint",sizeof(UINT) };
		pCurShader->Bind_Value("TransformIndex", WorldMatParam);

		if (packet.bSkinning) {
			SHADER_PARAM SkinningBoneParam{ &packet.SkinningOffset , "uint",sizeof(UINT) };
			pCurShader->Bind_Value("SkinningOffset", SkinningBoneParam);
		}

		packet.pMaterial->Apply_Material(pContext, packet.MaterialIndex);
		packet.pModel->Draw(pContext, packet.DrawIndex);
	}

	m_Packets.clear();
	m_VisiblePackets.clear();
}

void PriorityPass::Submit(OPAQUE_PACKET packet)
{
	if (packet.pModel == nullptr || packet.pMaterial == nullptr) return;
	m_Packets.push_back(packet);
}
#pragma endregion

#pragma region BLENDED_PASS

void BlendedPass::Execute(ID3D11DeviceContext* pContext, CRenderer* pRenderer)
{
	/*이건 전역적으로 셰이더에 값 넣어주는 역할*/
	CPipeLine* pPipeLine = m_pRenderSystem->Get_Pipeline();
	pCurShader = { nullptr };

	/*같은 셰이더, 같은 머티리얼, 같은 모델끼리 정렬 */
	sort(m_Packets.begin(), m_Packets.end(),
		[](const BLENDED_PACKET& a, const BLENDED_PACKET& b) {
			return a.GetKey() < b.GetKey();
		});

	/*패킷이 비어 있으면 리턴*/
	if (m_Packets.empty())
		return;

	/*상수 버퍼 및 SRV 세팅*/
	pPipeLine->Begin_ObjectBuffer(pContext);
	pPipeLine->Begin_SkinningBuffer(pContext);

	for (auto& packet : m_Packets)
	{
		if (!packet.bSkinning) {
			if (!pPipeLine->isVisible(packet.pModel->Get_MeshBoundingBox(packet.DrawIndex), XMLoadFloat4x4(packet.pWorldMatrix)))
				continue;
		}

		//여기서 인덱스 추가 저장해줌
		_uint TransformIndex = pPipeLine->Write_ObjectData(*packet.pWorldMatrix);
		_uint SkinningOffset = 0;
		if (packet.bSkinning) {
			if (holds_alternative<CAnimator3D*>(packet.pPayLoad))
				SkinningOffset = pPipeLine->Write_SkinningBuffer(get<CAnimator3D*>(packet.pPayLoad)->Get_BoneMatrices(packet.DrawIndex));
			else if (holds_alternative<CSkeletonFollower*>(packet.pPayLoad))
				SkinningOffset = pPipeLine->Write_SkinningBuffer(get<CSkeletonFollower*>(packet.pPayLoad)->Get_BoneMatrices(packet.DrawIndex));
			else
				SkinningOffset = pPipeLine->Write_SkinningBuffer(dynamic_cast<CSkeletalModel*>(packet.pModel)->Get_BoneMatrices(packet.DrawIndex));
		}

		packet.TransformIndex = TransformIndex;
		packet.SkinningOffset = SkinningOffset;

		m_VisiblePackets.push_back(packet);
	}

	pPipeLine->End_ObjectBuffer(pContext);
	pPipeLine->End_SkinningBuffer(pContext);

	/*드로우콜 시작*/
	for (auto& packet : m_VisiblePackets)
	{
		if (packet.pMaterial->Get_Shader(packet.MaterialIndex) != pCurShader) {
			BindConstant(pContext, packet.pModel, packet.pMaterial, packet.DrawIndex, packet.MaterialIndex, pRenderer);
		}

		SHADER_PARAM WorldMatParam{ &packet.TransformIndex, "uint",sizeof(UINT) };
		pCurShader->Bind_Value("TransformIndex", WorldMatParam);

		if (packet.bSkinning) {
			SHADER_PARAM SkinningBoneParam{ &packet.SkinningOffset , "uint",sizeof(UINT) };
			pCurShader->Bind_Value("SkinningOffset", SkinningBoneParam);
		}

		packet.pMaterial->Apply_Material(pContext, packet.MaterialIndex);
		packet.pModel->Draw(pContext, packet.DrawIndex);
	}

	m_Packets.clear();
	m_VisiblePackets.clear();
}


void BlendedPass::Submit(BLENDED_PACKET packet)
{
	if (packet.pModel == nullptr || packet.pMaterial == nullptr) return;
	m_Packets.push_back(packet);
}

#pragma endregion

#pragma region PARTICLE_PASS

void ParticlePass::Execute(ID3D11DeviceContext* pContext, CRenderer* pRenderer)
{
	/*이건 전역적으로 셰이더에 값 넣어주는 역할*/
	CPipeLine* pPipeLine = m_pRenderSystem->Get_Pipeline();
	pCurShader = { nullptr };

	/*각 파티클시스템 렌더링*/
	for (_uint i = 0; i < m_Packets.size(); ++i)
	{
		auto& packet = m_Packets[i];

		if (m_Packets[i].pMaterial->Get_Shader(0) != pCurShader) {
			BindConstant(pContext, packet.pParticleSystem, packet.pMaterial, 0, 0, pRenderer);
		}

		SHADER_PARAM param = {};
		param.iSize = sizeof(_float4x4);
		param.typeName = "float4x4";
		param.pData = &packet.WorldMatrix;
		pCurShader->Bind_Value("g_WorldMatrix", param);

		packet.pParticleSystem->Bind_Buffer(pContext);
		packet.pMaterial->Apply_Material(pContext, 0);
		packet.pParticleSystem->Draw(pContext);
	}

	m_Packets.clear();
}

void ParticlePass::Submit(PARTICLE_PACKET packet)
{
	//if (packet.pSprite2D == nullptr) return;
	m_Packets.push_back(packet);
}

#pragma endregion


#pragma region INSTANCE_PASS

void InstancePass::Execute(ID3D11DeviceContext* pContext, CRenderer* pRenderer)
{
 	CPipeLine* pPipeLine = m_pRenderSystem->Get_Pipeline();
	pCurShader = { nullptr };

	/*패킷이 비어 있으면 리턴*/
	if (m_Packets.empty())
		return;

	/*드로우콜 시작*/
	for (auto& packet : m_Packets)
	{
		if (packet.pMaterial->Get_Shader(packet.MaterialIndex) != pCurShader) {
			pCurShader = packet.pMaterial->Get_Shader(packet.MaterialIndex);
			pCurShader->SetConstantBuffer("FrameBuffer", pPipeLine->Get_FrameBuffer());
		}

		ID3D11InputLayout* pLayout;
		pRenderer->Get_InputLayout(packet.pModel, pCurShader, packet.DrawIndex,
			packet.pMaterial->GetPassConstant(packet.MaterialIndex), &pLayout);
		SHADER_PARAM param = {};
		param.iSize = sizeof(_float4x4);
		param.typeName = "float4x4";
		param.pData = packet.pWorldMatrix;

		pCurShader->Bind_Value("g_WorldMatrix", param);
		pContext->IASetInputLayout(pLayout);

		packet.pMaterial->Apply_Material(pContext, packet.MaterialIndex);
		packet.pModel->Bind_Buffer(pContext, packet.DrawIndex);
		packet.pModel->Draw(pContext, packet.DrawIndex);
	}
	m_Packets.clear();
}

void InstancePass::Submit(INSTANCE_PACKET packet)
{
	if (packet.pModel == nullptr || packet.pMaterial == nullptr) return;
		m_Packets.push_back(packet);
}
#pragma endregion

#pragma region UI_PASS
void UIPass::Execute(ID3D11DeviceContext* pContext, CRenderer* pRenderer)
{
 	CPipeLine* pPipeLine = m_pRenderSystem->Get_Pipeline();
	pCurShader = { nullptr };

	/*패킷이 비어 있으면 리턴*/
	if (m_Packets.empty())
		return;

	/*상수 버퍼 및 SRV 세팅*/
	pPipeLine->Begin_ObjectBuffer(pContext);
	for (auto& packet : m_Packets)
	{
		_uint TransformIndex = pPipeLine->Write_ObjectData(*packet.pWorldMatrix);
		packet.TransformIndex = TransformIndex;
	}
	pPipeLine->End_ObjectBuffer(pContext);

	for (auto& packet : m_Packets)
	{
		if (packet.pSprite2D->Get_Shader() != pCurShader) {
			
			BindConstant(pContext, packet.pSprite2D, packet.pSprite2D->Get_PassConstant(), pRenderer);
		}
	
		SHADER_PARAM WorldMatParam{ &packet.TransformIndex, "uint",sizeof(UINT) };
		pCurShader->Bind_Value("TransformIndex", WorldMatParam);
		pCurShader->Bind_Value("vColor", {packet.pColor, "float4", sizeof(_float4)});
		packet.pSprite2D->Apply_Shader(pContext);
		packet.pSprite2D->Draw_Sprite(pContext);


		CGameInstance::GetInstance()->Get_FontSystem()->Render_TextFont(packet.pSprite2D->Get_TextKey());
	}

	m_Packets.clear();
}

void UIPass::Submit(SPRITE_PACKET packet)
{
	if (packet.pSprite2D == nullptr ) return;
	m_Packets.push_back(packet);
}
#pragma endregion

#pragma region DEBUG_PASS
void DebugPass::Execute(ID3D11DeviceContext* pContext, CRenderer* pRenderer)
{
	CPipeLine* pPipeLine = m_pRenderSystem->Get_Pipeline();
	if (pCurShader == nullptr) {
		pCurShader = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_Shader(G_GlobalLevelKey, "VTX_Debug.hlsl");
	}
	if (m_Packets.empty())
		return;

	CModel* pCurModel = { nullptr };
	pPipeLine->Begin_ObjectBuffer(pContext);
	for (auto& packet : m_Packets)
	{
		_uint TransformIndex = pPipeLine->Write_ObjectData(*packet.pWorldMatrix);
		packet.TransformIndex = TransformIndex;
	}
	pPipeLine->End_ObjectBuffer(pContext);

	SHADER_PARAM ObjectMaticedParam = {};
	ObjectMaticedParam.iSize = sizeof(_float4x4) * g_iMaxTransform;
	ObjectMaticedParam.typeName = "StructuredBuffer";
	ObjectMaticedParam.pData = pPipeLine->Get_ObjectResource();
	pCurShader->Bind_Value("ObjectBufferArray", ObjectMaticedParam);
	pCurShader->SetConstantBuffer("FrameBuffer", pPipeLine->Get_FrameBuffer());

	for (auto& packet : m_Packets)
	{
		if (packet.pModel != pCurModel) {
		ID3D11InputLayout* pLayout;
		pRenderer->Get_InputLayout(packet.pModel, pCurShader,
			0, "Debug", &pLayout);
   		pContext->IASetInputLayout(pLayout);
			pCurModel = packet.pModel;
		}
		
		SHADER_PARAM WorldMatParam{ &packet.TransformIndex, "uint",sizeof(UINT) };
		pCurShader->Bind_Value("TransformIndex", WorldMatParam);
		pCurShader->Apply("Debug", pContext);
		packet.pDebug->Render_DebugBox(pContext, packet.DrawIndex);
	}

	m_Packets.clear();
}

void DebugPass::Submit(DEBUG_PACKET packet)
{
	if (packet.pModel == nullptr || packet.pDebug == nullptr) return;
	m_Packets.push_back(packet);
}
#pragma endregion

#pragma region SHADOW_PASS
void ShadowPass::Execute(ID3D11DeviceContext* pContext, CRenderer* pRenderer)
{
	Execute_Opaque(pContext,pRenderer);
	Execute_Instance(pContext, pRenderer);
}

void ShadowPass::Submit(OPAQUE_PACKET packet)
{
	if (packet.pModel == nullptr) return;
	m_Packets.push_back(packet);
}

void ShadowPass::SubmitInstance(INSTANCE_PACKET packet)
{
	if (packet.pModel == nullptr || packet.pMaterial == nullptr) return;
		m_InstancePackets.push_back(packet);
}

void ShadowPass::Execute_Opaque(ID3D11DeviceContext* pContext,CRenderer* pRenderer)
{
	/*이건 전역적으로 셰이더에 값 넣어주는 역할*/
	CPipeLine* pPipeLine = m_pRenderSystem->Get_Pipeline();
	pCurShader = { nullptr };

	/*같은 셰이더, 같은 머티리얼, 같은 모델끼리 정렬 */
	sort(m_Packets.begin(), m_Packets.end(),
		[](const OPAQUE_PACKET& a, const OPAQUE_PACKET& b) {
			return a.GetKey() < b.GetKey();
		});

	/*패킷이 비어 있으면 리턴*/
	if (m_Packets.empty())
		return;

	/*상수 버퍼 및 SRV 세팅*/
	pPipeLine->Begin_ObjectBuffer(pContext);
	pPipeLine->Begin_SkinningBuffer(pContext);

	for (auto& packet : m_Packets)
	{
		if (!packet.bSkinning) {
			if (!pPipeLine->isVisible(packet.pModel->Get_MeshBoundingBox(packet.DrawIndex), XMLoadFloat4x4(packet.pWorldMatrix)))
				continue;
		}

		//여기서 인덱스 추가 저장해줌
		_uint TransformIndex = pPipeLine->Write_ObjectData(*packet.pWorldMatrix);
		_uint SkinningOffset = 0;
		if (packet.bSkinning) {
			if (holds_alternative<CAnimator3D*>(packet.pPayLoad))
				SkinningOffset = pPipeLine->Write_SkinningBuffer(get<CAnimator3D*>(packet.pPayLoad)->Get_BoneMatrices(packet.DrawIndex));
			else if (holds_alternative<CSkeletonFollower*>(packet.pPayLoad))
				SkinningOffset = pPipeLine->Write_SkinningBuffer(get<CSkeletonFollower*>(packet.pPayLoad)->Get_BoneMatrices(packet.DrawIndex));
			else
				SkinningOffset = pPipeLine->Write_SkinningBuffer(dynamic_cast<CSkeletalModel*>(packet.pModel)->Get_BoneMatrices(packet.DrawIndex));
		}

		packet.TransformIndex = TransformIndex;
		packet.SkinningOffset = SkinningOffset;
		m_VisiblePackets.push_back(packet);
	}

	pPipeLine->End_ObjectBuffer(pContext);
	pPipeLine->End_SkinningBuffer(pContext);

	/*드로우콜 시작*/
	for (auto& packet : m_VisiblePackets)
	{
		if (packet.pMaterial->Get_Shader(packet.MaterialIndex) != pCurShader) {
			CPipeLine* pPipeLine = m_pRenderSystem->Get_Pipeline();
			pCurShader = packet.pMaterial->Get_Shader(packet.MaterialIndex);
			SHADER_PARAM ObjectMaticedParam = {};
			ObjectMaticedParam.iSize = sizeof(_float4x4) * g_iMaxTransform;
			ObjectMaticedParam.typeName = "StructuredBuffer";
			ObjectMaticedParam.pData = pPipeLine->Get_ObjectResource();
			pCurShader->Bind_Value("ObjectBufferArray", ObjectMaticedParam);
			pCurShader->SetConstantBuffer("ShadowBuffer", pPipeLine->Get_ShadowBuffer());

			SHADER_PARAM SkinningMatricedParam = {};
			SkinningMatricedParam.iSize = sizeof(_float4x4) * g_iMaxNumBones;
			SkinningMatricedParam.typeName = "StructuredBuffer";
			SkinningMatricedParam.pData = pPipeLine->Get_SkinningResource();
			pCurShader->Bind_Value("g_BoneMatrices", SkinningMatricedParam);

			ID3D11InputLayout* pLayout;
			pRenderer->Get_InputLayout(packet.pModel, pCurShader, packet.DrawIndex, "Shadow", &pLayout);
			pContext->IASetInputLayout(pLayout);
		}

		SHADER_PARAM WorldMatParam{ &packet.TransformIndex, "uint",sizeof(UINT) };
		pCurShader->Bind_Value("TransformIndex", WorldMatParam);
		if (packet.bSkinning) {
			SHADER_PARAM SkinningBoneParam{ &packet.SkinningOffset , "uint",sizeof(UINT) };
			pCurShader->Bind_Value("SkinningOffset", SkinningBoneParam);
		}

		//packet.pMaterial->Apply_Material(pContext, packet.MaterialIndex);
		pCurShader->Apply("Shadow", pContext);
		packet.pModel->Draw(pContext, packet.DrawIndex);
	}

	m_Packets.clear();
	m_VisiblePackets.clear();
	pContext->OMSetDepthStencilState(nullptr, 0);
	pContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
	pContext->IASetInputLayout(nullptr);
}

void ShadowPass::Execute_Instance(ID3D11DeviceContext* pContext, CRenderer* pRenderer)
{
	CPipeLine* pPipeLine = m_pRenderSystem->Get_Pipeline();
	pCurShader = { nullptr };

	/*패킷이 비어 있으면 리턴*/
	if (m_InstancePackets.empty())
		return;

	/*드로우콜 시작*/
	for (auto& packet : m_InstancePackets)
	{
		if (packet.pMaterial->Get_Shader(packet.MaterialIndex) != pCurShader) {
			pCurShader = packet.pMaterial->Get_Shader(packet.MaterialIndex);
			pCurShader->SetConstantBuffer("ShadowBuffer", pPipeLine->Get_ShadowBuffer());
		}

		ID3D11InputLayout* pLayout;
		pRenderer->Get_InputLayout(packet.pModel, pCurShader, packet.DrawIndex,
		"InstanceShadow", &pLayout);

		SHADER_PARAM param = {};
		param.iSize = sizeof(_float4x4);
		param.typeName = "float4x4";
		param.pData = packet.pWorldMatrix;

		pCurShader->Bind_Value("g_WorldMatrix", param);
		pContext->IASetInputLayout(pLayout);

		pCurShader->Apply("InstanceShadow", pContext);
		packet.pModel->Bind_Buffer(pContext, packet.DrawIndex);
		packet.pModel->Draw(pContext, packet.DrawIndex);
	}
	m_InstancePackets.clear();
}
#pragma endregion


#pragma region NONLIGHT_PASS

void NonLightPass::Execute(ID3D11DeviceContext* pContext, CRenderer* pRenderer)
{
	/*이건 전역적으로 셰이더에 값 넣어주는 역할*/
	CPipeLine* pPipeLine = m_pRenderSystem->Get_Pipeline();
	pCurShader = { nullptr };

	/*같은 셰이더, 같은 머티리얼, 같은 모델끼리 정렬 */
	sort(m_Packets.begin(), m_Packets.end(),
		[](const OPAQUE_PACKET& a, const OPAQUE_PACKET& b) {
			return a.GetKey() < b.GetKey();
		});

	/*패킷이 비어 있으면 리턴*/
	if (m_Packets.empty())
		return;

	/*상수 버퍼 및 SRV 세팅*/
	pPipeLine->Begin_ObjectBuffer(pContext);
	pPipeLine->Begin_SkinningBuffer(pContext);

	for (auto& packet : m_Packets)
	{
		if (!packet.bSkinning) {
			if (!pPipeLine->isVisible(packet.pModel->Get_MeshBoundingBox(packet.DrawIndex), XMLoadFloat4x4(packet.pWorldMatrix)))
				continue;
		}

		//여기서 인덱스 추가 저장해줌
		_uint TransformIndex = pPipeLine->Write_ObjectData(*packet.pWorldMatrix);
		_uint SkinningOffset = 0;
		if (packet.bSkinning) {
			if (holds_alternative<CAnimator3D*>(packet.pPayLoad))
				SkinningOffset = pPipeLine->Write_SkinningBuffer(get<CAnimator3D*>(packet.pPayLoad)->Get_BoneMatrices(packet.DrawIndex));
			else if (holds_alternative<CSkeletonFollower*>(packet.pPayLoad))
				SkinningOffset = pPipeLine->Write_SkinningBuffer(get<CSkeletonFollower*>(packet.pPayLoad)->Get_BoneMatrices(packet.DrawIndex));
			else
				SkinningOffset = pPipeLine->Write_SkinningBuffer(dynamic_cast<CSkeletalModel*>(packet.pModel)->Get_BoneMatrices(packet.DrawIndex));
		}

		packet.TransformIndex = TransformIndex;
		packet.SkinningOffset = SkinningOffset;

		m_VisiblePackets.push_back(packet);
	}

	pPipeLine->End_ObjectBuffer(pContext);
	pPipeLine->End_SkinningBuffer(pContext);

	/*드로우콜 시작*/
	for (auto& packet : m_VisiblePackets)
	{
		if (packet.pMaterial->Get_Shader(packet.MaterialIndex) != pCurShader) {
			BindConstant(pContext, packet.pModel, packet.pMaterial, packet.DrawIndex, packet.MaterialIndex, pRenderer);
		}

		SHADER_PARAM WorldMatParam{ &packet.TransformIndex, "uint",sizeof(UINT) };
		pCurShader->Bind_Value("TransformIndex", WorldMatParam);

		if (packet.bSkinning) {
			SHADER_PARAM SkinningBoneParam{ &packet.SkinningOffset , "uint",sizeof(UINT) };
			pCurShader->Bind_Value("SkinningOffset", SkinningBoneParam);
		}

		packet.pMaterial->Apply_Material(pContext, packet.MaterialIndex);
		packet.pModel->Draw(pContext, packet.DrawIndex);
	}

	m_Packets.clear();
	m_VisiblePackets.clear();
}


void NonLightPass::Submit(OPAQUE_PACKET packet)
{
	if (packet.pModel == nullptr || packet.pMaterial == nullptr) return;
	m_Packets.push_back(packet);
}

#pragma endregion

#pragma region EFFECT_PASS

void EffectPass::Execute(ID3D11DeviceContext* pContext, CRenderer* pRenderer)
{
	/*이건 전역적으로 셰이더에 값 넣어주는 역할*/
	CPipeLine* pPipeLine = m_pRenderSystem->Get_Pipeline();
	pCurShader = { nullptr };

	/*같은 셰이더, 같은 머티리얼, 같은 모델끼리 정렬 */
	sort(m_Packets.begin(), m_Packets.end(),
		[](const EFFECT_PACKET& a, const EFFECT_PACKET& b) {
			return a.GetKey() < b.GetKey();
		});

	/*패킷이 비어 있으면 리턴*/
	if (m_Packets.empty())
		return;

	/*상수 버퍼 및 SRV 세팅*/
	pPipeLine->Begin_ObjectBuffer(pContext);
	pPipeLine->Begin_SkinningBuffer(pContext);

	for (auto& packet : m_Packets)
	{
		if (!packet.bSkinning) {
			if (!pPipeLine->isVisible(packet.pModel->Get_MeshBoundingBox(packet.DrawIndex), XMLoadFloat4x4(packet.pWorldMatrix)))
				continue;
		}

		//여기서 인덱스 추가 저장해줌
		_uint TransformIndex = pPipeLine->Write_ObjectData(*packet.pWorldMatrix);
		_uint SkinningOffset = 0;
		if (packet.bSkinning) {
			if (holds_alternative<CAnimator3D*>(packet.pPayLoad))
				SkinningOffset = pPipeLine->Write_SkinningBuffer(get<CAnimator3D*>(packet.pPayLoad)->Get_BoneMatrices(packet.DrawIndex));
			else if (holds_alternative<CSkeletonFollower*>(packet.pPayLoad))
				SkinningOffset = pPipeLine->Write_SkinningBuffer(get<CSkeletonFollower*>(packet.pPayLoad)->Get_BoneMatrices(packet.DrawIndex));
			else
				SkinningOffset = pPipeLine->Write_SkinningBuffer(dynamic_cast<CSkeletalModel*>(packet.pModel)->Get_BoneMatrices(packet.DrawIndex));
		}

		packet.TransformIndex = TransformIndex;
		packet.SkinningOffset = SkinningOffset;

		m_VisiblePackets.push_back(packet);
	}

	pPipeLine->End_ObjectBuffer(pContext);
	pPipeLine->End_SkinningBuffer(pContext);

	/*드로우콜 시작*/
	for (auto& packet : m_VisiblePackets)
	{
		if (packet.pMaterial->Get_Shader(packet.MaterialIndex) != pCurShader) {
			BindConstant(pContext, packet.pModel, packet.pMaterial, packet.DrawIndex, packet.MaterialIndex, pRenderer);
		}

		SHADER_PARAM WorldMatParam{ &packet.TransformIndex, "uint",sizeof(UINT) };
		pCurShader->Bind_Value("TransformIndex", WorldMatParam);

		if (packet.bSkinning) {
			SHADER_PARAM SkinningBoneParam{ &packet.SkinningOffset , "uint",sizeof(UINT) };
			pCurShader->Bind_Value("SkinningOffset", SkinningBoneParam);
		}

		packet.pMaterial->Apply_Material(pContext, packet.MaterialIndex);
		packet.pModel->Draw(pContext, packet.DrawIndex);
	}

	m_Packets.clear();
	m_VisiblePackets.clear();
}


void EffectPass::Submit(EFFECT_PACKET packet)
{
	if (packet.pModel == nullptr || packet.pMaterial == nullptr) return;
	m_Packets.push_back(packet);
}

#pragma endregion

#pragma region 3DUI_PASS
void UI3DPass::Execute(ID3D11DeviceContext* pContext, CRenderer* pRenderer)
{/*이건 전역적으로 셰이더에 값 넣어주는 역할*/
	CPipeLine* pPipeLine = m_pRenderSystem->Get_Pipeline();
	pCurShader = { nullptr };

	/*같은 셰이더, 같은 머티리얼, 같은 모델끼리 정렬 */
	sort(m_Packets.begin(), m_Packets.end(),
		[](const OPAQUE_PACKET& a, const OPAQUE_PACKET& b) {
			return a.GetKey() < b.GetKey();
		});

	/*패킷이 비어 있으면 리턴*/
	if (m_Packets.empty())
		return;

	/*상수 버퍼 및 SRV 세팅*/
	pPipeLine->Begin_ObjectBuffer(pContext);
	pPipeLine->Begin_SkinningBuffer(pContext);

	for (auto& packet : m_Packets)
	{
		if (!packet.bSkinning) {
			if (!pPipeLine->isVisible(packet.pModel->Get_MeshBoundingBox(packet.DrawIndex), XMLoadFloat4x4(packet.pWorldMatrix)))
				continue;
		}

		//여기서 인덱스 추가 저장해줌
		_uint TransformIndex = pPipeLine->Write_ObjectData(*packet.pWorldMatrix);
		_uint SkinningOffset = 0;
		if (packet.bSkinning) {
			if (holds_alternative<CAnimator3D*>(packet.pPayLoad))
				SkinningOffset = pPipeLine->Write_SkinningBuffer(get<CAnimator3D*>(packet.pPayLoad)->Get_BoneMatrices(packet.DrawIndex));
			else if (holds_alternative<CSkeletonFollower*>(packet.pPayLoad))
				SkinningOffset = pPipeLine->Write_SkinningBuffer(get<CSkeletonFollower*>(packet.pPayLoad)->Get_BoneMatrices(packet.DrawIndex));
			else
				SkinningOffset = pPipeLine->Write_SkinningBuffer(dynamic_cast<CSkeletalModel*>(packet.pModel)->Get_BoneMatrices(packet.DrawIndex));
		}

		packet.TransformIndex = TransformIndex;
		packet.SkinningOffset = SkinningOffset;

		m_VisiblePackets.push_back(packet);
	}

	pPipeLine->End_ObjectBuffer(pContext);
	pPipeLine->End_SkinningBuffer(pContext);

	/*드로우콜 시작*/
	for (auto& packet : m_VisiblePackets)
	{
		if (packet.pMaterial->Get_Shader(packet.MaterialIndex) != pCurShader) {
			BindConstant(pContext, packet.pModel, packet.pMaterial, packet.DrawIndex, packet.MaterialIndex, pRenderer);
		}

		SHADER_PARAM WorldMatParam{ &packet.TransformIndex, "uint",sizeof(UINT) };
		pCurShader->Bind_Value("TransformIndex", WorldMatParam);

		if (packet.bSkinning) {
			SHADER_PARAM SkinningBoneParam{ &packet.SkinningOffset , "uint",sizeof(UINT) };
			pCurShader->Bind_Value("SkinningOffset", SkinningBoneParam);
		}

		packet.pMaterial->Apply_Material(pContext, packet.MaterialIndex);
		packet.pModel->Draw(pContext, packet.DrawIndex);
	}

	m_Packets.clear();
	m_VisiblePackets.clear();
}

void UI3DPass::Submit(OPAQUE_PACKET packet)
{
	if (packet.pModel == nullptr || packet.pMaterial == nullptr) return;
	m_Packets.push_back(packet);
}

#pragma endregion