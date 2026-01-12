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

#pragma region STATICMESH_OPAQUE_PASS
void StaticOpaquePass::Execute(ID3D11DeviceContext* pContext, CRenderer* pRenderer)
{/*�̰� ���������� ���̴��� �� �־��ִ� ����*/
	CPipeLine* pPipeLine = m_pRenderSystem->Get_Pipeline();
	pCurShader = { nullptr };

	/*���� ���̴�, ���� ��Ƽ����, ���� �𵨳��� ���� */
	sort(m_Packets.begin(), m_Packets.end(),
		[](const OPAQUE_PACKET& a, const OPAQUE_PACKET& b) {
			return a.GetKey() < b.GetKey();
		});

	/*��Ŷ�� ��� ������ ����*/
	if (m_Packets.empty())
		return;

	/*��� ���� �� SRV ����*/
	pPipeLine->Begin_ObjectBuffer(pContext);

	vector<OPAQUE_PACKET> frustums;
	for (auto& packet : m_Packets)
	{
		if (!pPipeLine->isVisible(packet.pModel->Get_MeshBoundingBox(packet.DrawIndex), XMLoadFloat4x4(packet.pWorldMatrix)))
			continue;
		_uint TransformIndex = pPipeLine->Write_ObjectData(*packet.pWorldMatrix);
		packet.TransformIndex = TransformIndex;
		frustums.push_back(packet);
	}

	pPipeLine->End_ObjectBuffer(pContext);

	m_VisiblePackets = pPipeLine->OcculsionCulling(frustums);
	/*��ο��� ����*/
	for (auto& packet : m_VisiblePackets)
	{
		if (packet.pMaterial->Get_Shader(packet.MaterialIndex) != pCurShader) {
			BindConstant(pContext, packet.pModel, packet.pMaterial, packet.DrawIndex, packet.MaterialIndex, pRenderer);
		}

		SHADER_PARAM WorldMatParam{ &packet.TransformIndex, "uint",sizeof(UINT) };
		pCurShader->Bind_Value("TransformIndex", WorldMatParam);

		SHADER_PARAM LookParam{ &packet.LookVector, "vector",sizeof(_vector) };
		pCurShader->Bind_Value("vLookVector", LookParam);

		packet.pMaterial->Apply_Material(pContext, packet.MaterialIndex);
		packet.pModel->Draw(pContext, packet.DrawIndex);
		packet.pMaterial->ResetMaterial(packet.DrawIndex);
	}

	m_Packets.clear();
	m_VisiblePackets.clear();
}

void StaticOpaquePass::Submit(OPAQUE_PACKET packet)
{
	if (packet.pModel == nullptr || packet.pMaterial == nullptr) return;
	m_Packets.push_back(packet);
}
#pragma endregion

#pragma region SKINNEDMESH_OPAQUE_PASS
void SkinnedOpaquePass::Execute(ID3D11DeviceContext* pContext, CRenderer* pRenderer)
{
	CPipeLine* pPipeLine = m_pRenderSystem->Get_Pipeline();
	pCurShader = { nullptr };

	/*���� ���̴�, ���� ��Ƽ����, ���� �𵨳��� ���� */
	sort(m_Packets.begin(), m_Packets.end(),
		[](const OPAQUE_PACKET& a, const OPAQUE_PACKET& b) {
			return a.GetKey() < b.GetKey();
		});

	/*��Ŷ�� ��� ������ ����*/
	if (m_Packets.empty())
		return;

	/*��� ���� �� SRV ����*/
	pPipeLine->Begin_ObjectBuffer(pContext);
	pPipeLine->Begin_SkinningBuffer(pContext);

	for (auto& packet : m_Packets)
	{
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


	/*��ο��� ����*/
	for (auto& packet : m_VisiblePackets)
	{
		if (packet.pMaterial->Get_Shader(packet.MaterialIndex) != pCurShader) {
			BindConstant(pContext, packet.pModel, packet.pMaterial, packet.DrawIndex, packet.MaterialIndex, pRenderer);
		}

		SHADER_PARAM WorldMatParam{ &packet.TransformIndex, "uint",sizeof(UINT) };
		pCurShader->Bind_Value("TransformIndex", WorldMatParam);

		SHADER_PARAM LookParam{ &packet.LookVector, "vector",sizeof(_vector) };
		pCurShader->Bind_Value("vLookVector", LookParam);

		if (packet.bSkinning) {
			SHADER_PARAM SkinningBoneParam{ &packet.SkinningOffset , "uint",sizeof(UINT) };
			pCurShader->Bind_Value("SkinningOffset", SkinningBoneParam);
		}

		packet.pMaterial->Apply_Material(pContext, packet.MaterialIndex);
		packet.pModel->Draw(pContext, packet.DrawIndex);
		packet.pMaterial->ResetMaterial(packet.DrawIndex);
	}

	m_Packets.clear();
	m_VisiblePackets.clear();
}

void SkinnedOpaquePass::Submit(OPAQUE_PACKET packet)
{
	if (packet.pModel == nullptr || packet.pMaterial == nullptr) return;
	m_Packets.push_back(packet);
}
#pragma endregion

#pragma region PRIORITY_PASS

void PriorityPass::Execute(ID3D11DeviceContext* pContext, CRenderer* pRenderer)
{
	/*�̰� ���������� ���̴��� �� �־��ִ� ����*/
	CPipeLine* pPipeLine = m_pRenderSystem->Get_Pipeline();
	pCurShader = { nullptr };

	/*���� ���̴�, ���� ��Ƽ����, ���� �𵨳��� ���� */
	sort(m_Packets.begin(), m_Packets.end(),
		[](const OPAQUE_PACKET& a, const OPAQUE_PACKET& b) {
			return a.GetKey() < b.GetKey();
		});

	/*��Ŷ�� ��� ������ ����*/
	if (m_Packets.empty())
		return;

	/*��� ���� �� SRV ����*/
	pPipeLine->Begin_ObjectBuffer(pContext);
	pPipeLine->Begin_SkinningBuffer(pContext);

	for (auto& packet : m_Packets)
	{
		//���⼭ �ε��� �߰� ��������
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

	/*��ο��� ����*/
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
		packet.pMaterial->ResetMaterial(packet.DrawIndex);
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
	/*�̰� ���������� ���̴��� �� �־��ִ� ����*/
	CPipeLine* pPipeLine = m_pRenderSystem->Get_Pipeline();
	pCurShader = { nullptr };

	/*���� ���̴�, ���� ��Ƽ����, ���� �𵨳��� ���� */
	sort(m_Packets.begin(), m_Packets.end(),
		[](const BLENDED_PACKET& a, const BLENDED_PACKET& b) {
			return a.GetKey() < b.GetKey();
		});

	/*��Ŷ�� ��� ������ ����*/
	if (m_Packets.empty())
		return;

	/*��� ���� �� SRV ����*/
	pPipeLine->Begin_ObjectBuffer(pContext);
	pPipeLine->Begin_SkinningBuffer(pContext);

	for (auto& packet : m_Packets)
	{
		if (!packet.bSkinning) {
			if (!pPipeLine->isVisible(packet.pModel->Get_MeshBoundingBox(packet.DrawIndex), XMLoadFloat4x4(packet.pWorldMatrix)))
				continue;
		}

		//���⼭ �ε��� �߰� ��������
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

	/*��ο��� ����*/
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
		packet.pMaterial->ResetMaterial(packet.DrawIndex);
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
	/*�̰� ���������� ���̴��� �� �־��ִ� ����*/
	CPipeLine* pPipeLine = m_pRenderSystem->Get_Pipeline();
	pCurShader = { nullptr };

	/*�� ��ƼŬ�ý��� ������*/
	for (_uint i = 0; i < m_Packets.size(); ++i)
	{
		auto& packet = m_Packets[i];

		if (m_Packets[i].pMaterial->Get_Shader(0) != pCurShader) 
		{
			BindConstant(pContext, packet.pParticleSystem, packet.pMaterial, 0, 0, pRenderer);

			auto pRenderSystem = CGameInstance::GetInstance()->Get_RenderSystem();
			
			ID3D11ShaderResourceView* pStaticDepthSRV = pRenderSystem->Get_EngineTargetSRV("Target_Static_Depth");
			ID3D11ShaderResourceView* pSkinnedDepthSRV = pRenderSystem->Get_EngineTargetSRV("Target_Skinned_Depth");
			
			pCurShader->Bind_Value("StaticMeshDepthTexture", { pStaticDepthSRV,"Texture2D",0 });
			pCurShader->Bind_Value("SkinnedMeshDepthTexture", { pSkinnedDepthSRV,"Texture2D",0 });
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

	/*��Ŷ�� ��� ������ ����*/
	if (m_Packets.empty())
		return;

	/*��ο��� ����*/
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
		packet.pMaterial->ResetMaterial(packet.DrawIndex);
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
	pCurShader = {};

	if (m_Packets.empty()) return;

	pPipeLine->Begin_ObjectBuffer(pContext);
	for (auto& packet : m_Packets)
	{
		_uint TransformIndex  = pPipeLine->Write_ObjectData(*packet.pWorldMatrix);
		packet.TransformIndex = TransformIndex;
	}
	pPipeLine->End_ObjectBuffer(pContext);

	for (auto& packet : m_Packets)
	{
		if (packet.pSprite2D->Get_Shader() != pCurShader) 
			BindConstant(pContext, packet.pSprite2D, packet.pSprite2D->Get_PassConstant(), pRenderer);
	
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
	if (!packet.pSprite2D) return;
	m_Packets.push_back(packet);
}
#pragma endregion

#pragma region STATICSHADOW_PASS
void StaticShadowPass::Execute(ID3D11DeviceContext* pContext, CRenderer* pRenderer, _bool IsFinal)
{
	Execute_Opaque(pContext, pRenderer, IsFinal);
}

void StaticShadowPass::Submit(OPAQUE_PACKET packet)
{
	if (packet.pModel == nullptr) return;
	m_Packets.push_back(packet);
}

void StaticShadowPass::Clear()
{
	m_Packets.clear();
}

void StaticShadowPass::Execute_Opaque(ID3D11DeviceContext* pContext, CRenderer* pRenderer, _bool IsFinal)
{
	CPipeLine* pPipeLine = m_pRenderSystem->Get_Pipeline();
	pCurShader = { nullptr };

	sort(m_Packets.begin(), m_Packets.end(),
		[](const OPAQUE_PACKET& a, const OPAQUE_PACKET& b) {
			return a.GetKey() < b.GetKey();
		});

	if (m_Packets.empty())
		return;

	pPipeLine->Begin_ObjectBuffer(pContext);
	pPipeLine->Begin_SkinningBuffer(pContext);

	for (auto& packet : m_Packets)
	{
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

	for (auto& packet : m_VisiblePackets)
	{
		packet.pMaterial->Get_Shader(packet.MaterialIndex)->SetConstantBuffer("ShadowBuffer", pPipeLine->Get_ShadowBuffer());
		if (packet.pMaterial->Get_Shader(packet.MaterialIndex) != pCurShader) {
			CPipeLine* pPipeLine = m_pRenderSystem->Get_Pipeline();
			pCurShader = packet.pMaterial->Get_Shader(packet.MaterialIndex);
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
	if (IsFinal)
	{
		m_Packets.clear();
		m_VisiblePackets.clear();
	}
	pContext->OMSetDepthStencilState(nullptr, 0);
	pContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
	pContext->IASetInputLayout(nullptr);
}
#pragma endregion

#pragma region SKINNEDSHADOW_PASS
void SkinnedShadowPass::Execute(ID3D11DeviceContext* pContext, CRenderer* pRenderer, _bool IsFinal)
{
	Execute_Opaque(pContext, pRenderer, IsFinal);
}

void SkinnedShadowPass::Submit(OPAQUE_PACKET packet)
{
	if (packet.pModel == nullptr) return;
	m_Packets.push_back(packet);
}

void SkinnedShadowPass::Clear()
{
	m_Packets.clear();
}

void SkinnedShadowPass::Execute_Opaque(ID3D11DeviceContext* pContext, CRenderer* pRenderer, _bool IsFinal)
{
	CPipeLine* pPipeLine = m_pRenderSystem->Get_Pipeline();
	pCurShader = { nullptr };

	sort(m_Packets.begin(), m_Packets.end(),
		[](const OPAQUE_PACKET& a, const OPAQUE_PACKET& b) {
			return a.GetKey() < b.GetKey();
		});

	if (m_Packets.empty())
		return;

	pPipeLine->Begin_ObjectBuffer(pContext);
	pPipeLine->Begin_SkinningBuffer(pContext);

	for (auto& packet : m_Packets)
	{
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

	for (auto& packet : m_VisiblePackets)
	{
		packet.pMaterial->Get_Shader(packet.MaterialIndex)->SetConstantBuffer("ShadowBuffer", pPipeLine->Get_ShadowBuffer());
		if (packet.pMaterial->Get_Shader(packet.MaterialIndex) != pCurShader) {
			CPipeLine* pPipeLine = m_pRenderSystem->Get_Pipeline();
			pCurShader = packet.pMaterial->Get_Shader(packet.MaterialIndex);
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
	if (IsFinal)
	{
		m_Packets.clear();
		m_VisiblePackets.clear();
	}
	pContext->OMSetDepthStencilState(nullptr, 0);
	pContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
	pContext->IASetInputLayout(nullptr);

}
#pragma endregion

#pragma region NONLIGHT_PASS

void NonLightPass::Execute(ID3D11DeviceContext* pContext, CRenderer* pRenderer)
{
	/*�̰� ���������� ���̴��� �� �־��ִ� ����*/
	CPipeLine* pPipeLine = m_pRenderSystem->Get_Pipeline();
	pCurShader = { nullptr };

	/*���� ���̴�, ���� ��Ƽ����, ���� �𵨳��� ���� */
	sort(m_Packets.begin(), m_Packets.end(),
		[](const OPAQUE_PACKET& a, const OPAQUE_PACKET& b) {
			return a.GetKey() < b.GetKey();
		});

	/*��Ŷ�� ��� ������ ����*/
	if (m_Packets.empty())
		return;

	/*��� ���� �� SRV ����*/
	pPipeLine->Begin_ObjectBuffer(pContext);
	pPipeLine->Begin_SkinningBuffer(pContext);

	for (auto& packet : m_Packets)
	{
		if (!packet.bSkinning) {
			if (!pPipeLine->isVisible(packet.pModel->Get_MeshBoundingBox(packet.DrawIndex), XMLoadFloat4x4(packet.pWorldMatrix)))
				continue;
		}

		//���⼭ �ε��� �߰� ��������
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

	/*��ο��� ����*/
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
	/*�̰� ���������� ���̴��� �� �־��ִ� ����*/
	CPipeLine* pPipeLine = m_pRenderSystem->Get_Pipeline();
	pCurShader = { nullptr };

	/*���� ���̴�, ���� ��Ƽ����, ���� �𵨳��� ���� */
	sort(m_Packets.begin(), m_Packets.end(),
		[](const EFFECT_PACKET& a, const EFFECT_PACKET& b) {
			return a.GetKey() < b.GetKey();
		});

	/*��Ŷ�� ��� ������ ����*/
	if (m_Packets.empty())
		return;

	/*��� ���� �� SRV ����*/
	pPipeLine->Begin_ObjectBuffer(pContext);
	pPipeLine->Begin_SkinningBuffer(pContext);

	for (auto& packet : m_Packets)
	{
		if (!packet.bSkinning) {
			if (!pPipeLine->isVisible(packet.pModel->Get_MeshBoundingBox(packet.DrawIndex), XMLoadFloat4x4(packet.pWorldMatrix)))
				continue;
		}

		//���⼭ �ε��� �߰� ��������
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

	/*��ο��� ����*/
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
{/*�̰� ���������� ���̴��� �� �־��ִ� ����*/
	CPipeLine* pPipeLine = m_pRenderSystem->Get_Pipeline();
	pCurShader = { nullptr };

	/*���� ���̴�, ���� ��Ƽ����, ���� �𵨳��� ���� */
	sort(m_Packets.begin(), m_Packets.end(),
		[](const OPAQUE_PACKET& a, const OPAQUE_PACKET& b) {
			return a.GetKey() < b.GetKey();
		});

	/*��Ŷ�� ��� ������ ����*/
	if (m_Packets.empty())
		return;

	/*��� ���� �� SRV ����*/
	pPipeLine->Begin_ObjectBuffer(pContext);
	pPipeLine->Begin_SkinningBuffer(pContext);

	for (auto& packet : m_Packets)
	{
		if (!packet.bSkinning) {
			if (!pPipeLine->isVisible(packet.pModel->Get_MeshBoundingBox(packet.DrawIndex), XMLoadFloat4x4(packet.pWorldMatrix)))
				continue;
		}

		//���⼭ �ε��� �߰� ��������
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

	/*��ο��� ����*/
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
