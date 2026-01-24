#include "Engine_Defines.h"
#include "PipeLine.h"
#include "GameInstance.h"
#include "ICameraService.h"
#include "IResourceService.h"
#include "ILightService.h"
#include "RenderSystem.h"
#include "Light.h"
#include "VIBuffer.h"
#include "Shader.h"
#include "Model.h"
#include "Texture.h"
#include "Renderer.h"
#include "Helper_Func.h"
#include "HiZ_Culling.h"
#include "CSMShadow.h"

CPipeLine::CPipeLine()
{
}

HRESULT CPipeLine::Initialize(ID3D11Device* pDevice, class CRenderSystem* pSystem)
{
	XMStoreFloat4x4(&identity, XMMatrixIdentity());
	/*---------------------------------------------------------------------------------------------------- - */
	/*상수 버퍼*/
	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = sizeof(FrameBuffer);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pDevice->CreateBuffer(&desc, nullptr, &m_pDeviceFrameBuffer);

	desc.ByteWidth = sizeof(ShadowBuffer);
	pDevice->CreateBuffer(&desc, nullptr, &m_pDeviceShadowBuffer);

	desc.ByteWidth = sizeof(LightBuffer);
	pDevice->CreateBuffer(&desc, nullptr, &m_pDeviceLightBuffer);

	desc.ByteWidth = sizeof(SSAOBuffer);
	pDevice->CreateBuffer(&desc, nullptr, &m_pDeviceSSAOBuffer);
	/*---------------------------------------------------------------------------------------------------- - */
	/*스키닝 본 버퍼 - > 이건 셰이더 리소스 뷰도 같이 만들어버림*/
	vector<_float4x4> BoneMatrices;
	BoneMatrices.resize(g_iMaxNumBones);

	D3D11_BUFFER_DESC SkinningBufferDesc = {};
	SkinningBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	SkinningBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	SkinningBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	SkinningBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	SkinningBufferDesc.StructureByteStride = sizeof(_float4x4);
	SkinningBufferDesc.ByteWidth = sizeof(_float4x4) * g_iMaxNumBones;

	pDevice->CreateBuffer(&SkinningBufferDesc, nullptr, &m_pDeviceSkinningBuffer);

	D3D11_SHADER_RESOURCE_VIEW_DESC SkinningResourceDesc = {};
	SkinningResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX; /*텍스처가 아니다! */
	SkinningResourceDesc.BufferEx.NumElements = g_iMaxNumBones;
	SkinningResourceDesc.Format = DXGI_FORMAT_UNKNOWN;										/*픽셀 아님*/

	pDevice->CreateShaderResourceView(m_pDeviceSkinningBuffer, &SkinningResourceDesc, &m_pSkinningResource);

	/*트랜스폼 버퍼 - > 이건 셰이더 리소스 뷰도 같이 만들어버림*/
	vector<_float4x4> TransformMatrix;
	TransformMatrix.resize(g_iMaxTransform);

	D3D11_BUFFER_DESC TransformBufferDesc = {};
	TransformBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	TransformBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TransformBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	TransformBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	TransformBufferDesc.StructureByteStride = sizeof(_float4x4);
	TransformBufferDesc.ByteWidth = sizeof(_float4x4) * g_iMaxTransform;
	pDevice->CreateBuffer(&TransformBufferDesc, nullptr, &m_pDeviceObjectBuffer);

	D3D11_SHADER_RESOURCE_VIEW_DESC TransformResourceDesc = {};
	TransformResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX; /*텍스처가 아니다! */
	TransformResourceDesc.BufferEx.NumElements = g_iMaxTransform;
	TransformResourceDesc.Format = DXGI_FORMAT_UNKNOWN;										/*픽셀 아님*/
	pDevice->CreateShaderResourceView(m_pDeviceObjectBuffer, &TransformResourceDesc, &m_pObjectResource);

	m_pSystem = pSystem;

	ID3D11DeviceContext* pContext = CGameInstance::GetInstance()->Get_Context();
	m_pHiZ = CHiZ_Culling::Create();

	m_pSkinnedCSM = CCSMShadow::Create(pDevice, pContext, 8192);
	m_pStaticCSM = CCSMShadow::Create(pDevice, pContext, 8192);

	ZeroMemory(&PreShadowBuffer, sizeof(ShadowBuffer));

	return S_OK;
}

/*프레임 단위의 버퍼*/
HRESULT CPipeLine::Update_FrameBuffer(ID3D11DeviceContext* pContext)
{
	FrameBuffer frameBuffer{};

	frameBuffer.matProjection = *CGameInstance::GetInstance()->Get_CameraMgr()->Get_ProjMatrix();
	frameBuffer.matView = *CGameInstance::GetInstance()->Get_CameraMgr()->Get_ViewMatrix();

	_float2 clientSize = CGameInstance::GetInstance()->Get_ClientSize();
	if (clientSize.x == 0 || clientSize.y == 0)
		return E_FAIL;

	_float4x4 OrthoProject;
	XMStoreFloat4x4(&OrthoProject, XMMatrixOrthographicLH(clientSize.x, clientSize.y, 0.f, 1.f));
	frameBuffer.matOrthograph = OrthoProject;
	frameBuffer.matViewInverse = *CGameInstance::GetInstance()->Get_CameraMgr()->Get_InversedViewMatrix();
	frameBuffer.matProjectionInverse = *CGameInstance::GetInstance()->Get_CameraMgr()->Get_InversedProjMatrix();
	frameBuffer.vCamPosition = CGameInstance::GetInstance()->Get_CameraMgr()->Get_CameraPos();
	frameBuffer.CameraForward = { frameBuffer.matViewInverse._31,frameBuffer.matViewInverse._32,frameBuffer.matViewInverse._33 };
	frameBuffer.zFar = CGameInstance::GetInstance()->Get_CameraMgr()->Get_Far();
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	HRESULT hr = pContext->Map(
		m_pDeviceFrameBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedResource
	);
	if (FAILED(hr))
		return hr;

	memcpy(mappedResource.pData, &frameBuffer, sizeof(FrameBuffer));
	pContext->Unmap(m_pDeviceFrameBuffer, 0);

	return S_OK;
}

HRESULT CPipeLine::Update_ShadowBuffer(ID3D11DeviceContext* pContext, _bool IsSkinningMesh, _int cascadeIndex)
{
	ShadowBuffer shadowBuffer{};
	ZeroMemory(&shadowBuffer, sizeof(ShadowBuffer));

	shadowBuffer.matShadowProjection = *CGameInstance::GetInstance()->Get_CameraMgr()->Get_ShadowProjMatrix();
	shadowBuffer.matShadowView = *CGameInstance::GetInstance()->Get_CameraMgr()->Get_ShadowViewMatrix();
	shadowBuffer.matShadowViewInverse = *CGameInstance::GetInstance()->Get_CameraMgr()->Get_InversedShadowViewMatrix();
	shadowBuffer.matShadowProjectionInverse = *CGameInstance::GetInstance()->Get_CameraMgr()->Get_InversedShadowProjMatrix();
	shadowBuffer.vShadowPosition = CGameInstance::GetInstance()->Get_CameraMgr()->Get_ShadowCameraPos();
	shadowBuffer.zShadowFar = CGameInstance::GetInstance()->Get_CameraMgr()->Get_ShadowFar();

	if (IsSkinningMesh)
	{
		for (_uint i = 0; i < 4; ++i)
		{
			Matrix lightVP = m_pSkinnedCSM->GetLightViewProj(i);
			shadowBuffer.matSkinnedLightViewProj[i] = lightVP;
			shadowBuffer.matStaticLightViewProj[i] = PreShadowBuffer.matStaticLightViewProj[i];
			PreShadowBuffer.matSkinnedLightViewProj[i] = shadowBuffer.matSkinnedLightViewProj[i];
		}

		const float* splits = m_pSkinnedCSM->GetCascadeSplits();
		shadowBuffer.vCascadeSplits = Vector4(splits[1], splits[2], splits[3], splits[4]);

		shadowBuffer.iCurrentCascade = cascadeIndex;
	}
	else
	{
		for (_uint i = 0; i < 4; ++i)
		{
			Matrix lightVP = m_pStaticCSM->GetLightViewProj(i);
			shadowBuffer.matStaticLightViewProj[i] = lightVP;
			shadowBuffer.matSkinnedLightViewProj[i] = PreShadowBuffer.matSkinnedLightViewProj[i];
			PreShadowBuffer.matStaticLightViewProj[i] = shadowBuffer.matStaticLightViewProj[i];
		}

		const float* splits = m_pStaticCSM->GetCascadeSplits();
		shadowBuffer.vCascadeSplits = Vector4(splits[1], splits[2], splits[3], splits[4]);

		shadowBuffer.iCurrentCascade = cascadeIndex;
	}

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	HRESULT hr = pContext->Map(
		m_pDeviceShadowBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedResource
	);
	if (FAILED(hr))
		return hr;

	memcpy(mappedResource.pData, &shadowBuffer, sizeof(ShadowBuffer));
	pContext->Unmap(m_pDeviceShadowBuffer, 0);
	
	return S_OK;
}

HRESULT CPipeLine::Update_LightBuffer(ID3D11DeviceContext* pContext, const LIGHT_DESC& Desc, _int lightSize)
{
	LightBuffer lightBuffer{};
	lightBuffer.vLightDir = Desc.vLightDirection;
	lightBuffer.vLightPos = Desc.vLightPosition;
	lightBuffer.vLightDiffuse = Desc.vLightDiffuse;
	lightBuffer.vLightAmbient = Desc.vLightAmbient;
	lightBuffer.vLightSpecular = Desc.vLightSpecular;
	lightBuffer.fLightRange = Desc.fLightRange;
	lightBuffer.fLightIntensity = Desc.fLightIntensity;
	lightBuffer.iLightSize = lightSize;

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	HRESULT hr = pContext->Map(
		m_pDeviceLightBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedResource
	);
	if (FAILED(hr))
		return hr;

	memcpy(mappedResource.pData, &lightBuffer, sizeof(LightBuffer));
	pContext->Unmap(m_pDeviceLightBuffer, 0);

	return S_OK;
}

HRESULT CPipeLine::Update_SSAOBuffer(ID3D11DeviceContext* pContext)
{
	SSAOBuffer ssaoBuffer{};

	_uint				iNumViewports = { 1 };
	D3D11_VIEWPORT		ViewportDesc{};
	pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	ssaoBuffer.Radius = 0.5f;
	ssaoBuffer.Bias = 0.025f;
	ssaoBuffer.ScreenWidth = ViewportDesc.Width;
	ssaoBuffer.ScreenHeight = ViewportDesc.Height;

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	HRESULT hr = pContext->Map(
		m_pDeviceSSAOBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedResource
	);
	if (FAILED(hr))
		return hr;

	memcpy(mappedResource.pData, &ssaoBuffer, sizeof(SSAOBuffer));
	pContext->Unmap(m_pDeviceSSAOBuffer, 0);

	return S_OK;
}

HRESULT CPipeLine::Write_SSAOKernelBuffer(ID3D11Device* pDevice)
{
	if (m_pDeviceSSAOKernelBuffer)
	{
		Safe_Release(m_pDeviceSSAOKernelBuffer);
		m_pDeviceSSAOKernelBuffer = nullptr;
	}

	SSAOKernel kernelBuffer;

	for (unsigned int i = 0; i < 64; ++i)
	{
		_vector3 Sample(Helper::Get_Random_Float(-1.f, 1.f), Helper::Get_Random_Float(-1.f, 1.f), Helper::Get_Random_Float(0.f, 1.f));
		Sample.Normalize();

		float scale = (float)i / 64.0f;
		scale = 0.1f + (scale * scale) * 0.9f;
		Sample *= scale;

		kernelBuffer.SSAOKernel[i] = _float4(Sample.x, Sample.y, Sample.z, 0.0f);
	}

	D3D11_BUFFER_DESC Desc = {};
	Desc.ByteWidth = sizeof(SSAOKernel);
	Desc.Usage = D3D11_USAGE_IMMUTABLE;
	Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Desc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA Data = {};
	Data.pSysMem = &kernelBuffer;

	if (FAILED(pDevice->CreateBuffer(&Desc, &Data, &m_pDeviceSSAOKernelBuffer)))
		return E_FAIL;

	return S_OK;
}

void CPipeLine::Update_Frustum()
{
	_matrix proj = XMLoadFloat4x4(CGameInstance::GetInstance()->Get_CameraMgr()->Get_ProjMatrix());
	_smatrix invView = *CGameInstance::GetInstance()->Get_CameraMgr()->Get_InversedViewMatrix();

	BoundingFrustum frustumView;
	BoundingFrustum::CreateFromMatrix(frustumView, proj);

	frustumView.Transform(m_Frustum, invView);
}

void CPipeLine::Update_StaticCSM()
{
	m_pStaticCSM->Update();
}

void CPipeLine::Update_SkinnedCSM()
{
	m_pSkinnedCSM->Update();
}

void CPipeLine::Update_HiZ(ID3D11DeviceContext* pContext)
{
	m_pHiZ->Update_HiZ(pContext);
}

_bool CPipeLine::isVisible(MINMAX_BOX minMax, _fmatrix worldTransform)
{
	// 월드 AABB 계산
	MINMAX_BOX worldBox = minMax.TransformBox_8Corner(Matrix(worldTransform));

	XMFLOAT3 center{
		(worldBox.vMin.x + worldBox.vMax.x) * 0.5f,
		(worldBox.vMin.y + worldBox.vMax.y) * 0.5f,
		(worldBox.vMin.z + worldBox.vMax.z) * 0.5f
	};
	XMFLOAT3 extents{
		(worldBox.vMax.x - worldBox.vMin.x) * 0.5f,
		(worldBox.vMax.y - worldBox.vMin.y) * 0.5f,
		(worldBox.vMax.z - worldBox.vMin.z) * 0.5f
	};

	if (!(extents.x > 0.f && extents.y > 0.f && extents.z > 0.f))
		return false;

	extents.y = max(extents.y, 0.2f);

	BoundingBox worldAabb(center, extents);

	return m_Frustum.Intersects(worldAabb);
}



_uint CPipeLine::Write_ObjectData(const _float4x4& worldMatrix)
{
	if (!m_pObjectArray) return UINT_MAX;

	const _uint offsetCount = 1;

	if (m_ObjectOffset + offsetCount > g_iMaxTransform) {
		MSG_BOX("To Many Transform");
		return UINT_MAX; // 초과
	}

	memcpy(&m_pObjectArray[m_ObjectOffset], &worldMatrix, sizeof(_float4x4));
	_uint LastOffset = m_ObjectOffset;
	m_ObjectOffset += offsetCount;
	return LastOffset;
}

_uint CPipeLine::GetOrWriteTransform(_uint objId, const _float4x4& world)
{
	auto found = m_transformIndexCache.find(objId);
	if (found != m_transformIndexCache.end())
		return found->second;

	_uint index = Write_ObjectData(world);
	if (index != UINT_MAX)
		m_transformIndexCache.emplace(objId, index);
	return index;
}

HRESULT CPipeLine::Begin_ObjectBuffer(ID3D11DeviceContext* pContext)
{
	m_transformIndexCache.clear(); // 추가

	HRESULT hr = pContext->Map(m_pDeviceObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &m_mappedObjectBuffer);
	if (FAILED(hr)) return hr;

	m_pObjectArray = reinterpret_cast<_float4x4*>(m_mappedObjectBuffer.pData);
	m_pObjectArray[0] = identity;
	m_ObjectOffset = 1;
	return S_OK;
}

HRESULT CPipeLine::End_ObjectBuffer(ID3D11DeviceContext* pContext)
{
	pContext->Unmap(m_pDeviceObjectBuffer, 0);
	m_pObjectArray = nullptr;
	m_ObjectOffset = 1;
	return S_OK;
}

static uint64_t MakeSkinKey(uint32_t objId, uint32_t drawIndex)
{
	return (uint64_t(objId) << 32) | uint64_t(drawIndex);
}

_uint CPipeLine::GetOrWriteSkinning(_uint objId, _uint drawIndex, const vector<_float4x4>& bones)
{
	uint64_t key = MakeSkinKey((uint32_t)objId, (uint32_t)drawIndex);

	auto found = m_skinningCache.find(key);
	if (found != m_skinningCache.end())
		return found->second;

	uint32_t offset = Write_SkinningBuffer(bones);
	if (offset != UINT_MAX)
		m_skinningCache.emplace(key, offset);
	return offset;
}

_uint CPipeLine::Write_SkinningBuffer(const vector<_float4x4>& bones)
{
	if (!m_pSkinningArray) return UINT_MAX;

	const _uint count = (_uint)bones.size();
	if (count == 0) return UINT_MAX;

	if (m_SkinningOffset + count > g_iMaxNumBones) {
		MSG_BOX("To Many Skinning");
		return UINT_MAX;
	}

	memcpy(&m_pSkinningArray[m_SkinningOffset], bones.data(), sizeof(_float4x4) * count);
	_uint last = m_SkinningOffset;
	m_SkinningOffset += count;
	return last;
}

HRESULT CPipeLine::Begin_SkinningBuffer(ID3D11DeviceContext* pContext)
{
	m_skinningCache.clear(); // 추가

	HRESULT hr = pContext->Map(m_pDeviceSkinningBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &m_mappedSkinningBuffer);
	if (FAILED(hr)) return hr;

	m_pSkinningArray = reinterpret_cast<_float4x4*>(m_mappedSkinningBuffer.pData);
	m_SkinningOffset = 0;
	return S_OK;
}

HRESULT CPipeLine::End_SkinningBuffer(ID3D11DeviceContext* pContext)
{
	pContext->Unmap(m_pDeviceSkinningBuffer, 0);

	m_pSkinningArray = nullptr;
	m_SkinningOffset = 0;
	return S_OK;
}

void CPipeLine::Begin_ShadowRender(_bool IsSkinningMesh, _uint cascadeIndex)
{
	if (IsSkinningMesh) m_pSkinnedCSM->Begin_ShadowRender(cascadeIndex);
	else m_pStaticCSM->Begin_ShadowRender(cascadeIndex);
}

void CPipeLine::End_ShadowRender(_bool IsSkinningMesh)
{
	if (IsSkinningMesh) m_pSkinnedCSM->End_ShadowRender();
	else m_pStaticCSM->End_ShadowRender();
}

ID3D11DepthStencilView* CPipeLine::GetCSMDSV(_bool IsSkinningMesh, _uint index) const
{
	ID3D11DepthStencilView* DSV = nullptr;

	if (IsSkinningMesh) DSV = m_pSkinnedCSM->GetDSV(index);
	else DSV = m_pStaticCSM->GetDSV(index);

	return DSV;
}

HRESULT CPipeLine::Bind_ShadowMap(CShader* pShader)
{
	pShader->Bind_Value("SkinnedShadowMapArray", { m_pSkinnedCSM->GetShadowMapSRV(), "Texture2DArray", 0 });
	pShader->Bind_Value("StaticShadowMapArray", { m_pStaticCSM->GetShadowMapSRV(), "Texture2DArray", 0 });
	return S_OK;
}

void CPipeLine::BindSampler(ID3D11DeviceContext* pContext, _uint slot)
{
	m_pSkinnedCSM->BindSampler(pContext, slot);
}

HRESULT CPipeLine::Bind_Light(CShader* pShader, class CVIBuffer* pBuffer, ID3D11DeviceContext* pContext, CRenderer* pRenderer)
{

	auto LightSnapShots = CGameInstance::GetInstance()->Get_LightMgr()->Visible_Lights();

	if (LightSnapShots.empty()) return E_FAIL;

	for (size_t i = 0; i < LightSnapShots.size(); i++)
	{
		Update_LightBuffer(pContext, LightSnapShots[i], LightSnapShots.size());
	
		ID3D11InputLayout* pLayout;

		switch (LightSnapShots[i].eType)
		{
		case Engine::LIGHT_TYPE::DIRECTIONAL:
			pRenderer->Get_BufferInputLayout(pBuffer, pShader, "DIRECTIONAL", &pLayout);
			pContext->IASetInputLayout(pLayout);
			pShader->Apply("DIRECTIONAL", pContext);
			pShader->SetConstantBuffer("LightBuffer", Get_LightBuffer());
			BindSampler(pContext, 10);
			pBuffer->Bind_Buffer(pContext);
			pBuffer->Render(pContext);
			break;
		case Engine::LIGHT_TYPE::POINT:
			pRenderer->Get_BufferInputLayout(pBuffer, pShader, "POINT", &pLayout);
			pContext->IASetInputLayout(pLayout);
			pShader->Apply("POINT", pContext);
			pShader->SetConstantBuffer("LightBuffer", Get_LightBuffer());
			pBuffer->Bind_Buffer(pContext);
			pBuffer->Render(pContext);
			break;
		case Engine::LIGHT_TYPE::SPOTLIGHT:
			break;
		default:
			break;
		}
	}

	return S_OK;
}

vector<OPAQUE_PACKET> CPipeLine::OcculsionCulling(const vector<OPAQUE_PACKET>& frustums)
{
	return m_pHiZ->OcculsionCulling(frustums);
}


#ifdef _USING_GUI
void CPipeLine::Render_GUI()
{
	m_pHiZ->Render_GUI();
}
#endif // !_USING_GUI

CPipeLine* CPipeLine::Create(ID3D11Device* pDevice, class CRenderSystem* pSystem)
{
	CPipeLine* instance = new CPipeLine();
	if (FAILED(instance->Initialize(pDevice, pSystem)))
	{
		Safe_Release(instance);
	}

	return instance;
}

void CPipeLine::Free()
{
	__super::Free();

	Safe_Release(m_pDeviceFrameBuffer);
	Safe_Release(m_pDeviceObjectBuffer);
	Safe_Release(m_pDeviceSkinningBuffer);
	Safe_Release(m_pSkinningResource);
	Safe_Release(m_pObjectResource);
	Safe_Release(m_pDeviceShadowBuffer);
	Safe_Release(m_pDeviceSSAOBuffer);
	Safe_Release(m_pDeviceLightBuffer);
	Safe_Release(m_pDeviceSSAOKernelBuffer);
	Safe_Release(m_pHiZ);
	Safe_Release(m_pSkinnedCSM);
	Safe_Release(m_pStaticCSM);
}
