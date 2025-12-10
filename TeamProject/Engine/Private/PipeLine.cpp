#include "PipeLine.h"
#include "GameInstance.h"
#include "ICameraService.h"
#include "IResourceService.h"
#include "ILightService.h"
#include "RenderSystem.h"
#include "Light.h"
#include	"VIBuffer.h"
#include "Shader.h"
#include "Model.h"
#include "Texture.h"

CPipeLine::CPipeLine()
{
}

HRESULT CPipeLine::Initialize(ID3D11Device* pDevice, class CRenderSystem* pSystem)
{
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

HRESULT CPipeLine::Update_ShadowBuffer(ID3D11DeviceContext* pContext)
{
	ShadowBuffer shadowBuffer{};

	shadowBuffer.matShadowProjection = *CGameInstance::GetInstance()->Get_CameraMgr()->Get_ShadowProjMatrix();
	shadowBuffer.matShadowView = *CGameInstance::GetInstance()->Get_CameraMgr()->Get_ShadowViewMatrix();
	shadowBuffer.matShadowViewInverse = *CGameInstance::GetInstance()->Get_CameraMgr()->Get_InversedShadowViewMatrix();
	shadowBuffer.matShadowProjectionInverse = *CGameInstance::GetInstance()->Get_CameraMgr()->Get_InversedShadowProjMatrix();
	shadowBuffer.vShadowPosition = CGameInstance::GetInstance()->Get_CameraMgr()->Get_ShadowCameraPos();
	shadowBuffer.zShadowFar = CGameInstance::GetInstance()->Get_CameraMgr()->Get_ShadowFar();

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

void CPipeLine::Update_Frustum()
{
	_matrix view = XMLoadFloat4x4(CGameInstance::GetInstance()->Get_CameraMgr()->Get_ViewMatrix());
	_matrix proj = XMLoadFloat4x4(CGameInstance::GetInstance()->Get_CameraMgr()->Get_ProjMatrix());

	BoundingFrustum frustum;
	BoundingFrustum::CreateFromMatrix(frustum, proj);

	frustum.Transform(m_Frustum, XMMatrixInverse(nullptr, view));
}

_bool CPipeLine::isVisible(MINMAX_BOX minMax, _fmatrix worldTransform)
{

	_float4x4 matViewInverse = *CGameInstance::GetInstance()->Get_CameraMgr()->Get_InversedViewMatrix();
	_float4 vCamPosition = CGameInstance::GetInstance()->Get_CameraMgr()->Get_CameraPos();
	_float3 CameraForward = { matViewInverse._31,matViewInverse._32,matViewInverse._33 };

	XMFLOAT3 center{
		(minMax.vMin.x + minMax.vMax.x) * 0.5f,
		(minMax.vMin.y + minMax.vMax.y) * 0.5f,
		(minMax.vMin.z + minMax.vMax.z) * 0.5f
	};

	XMFLOAT3 extents{
		(minMax.vMax.x - minMax.vMin.x) * 0.5f,
		(minMax.vMax.y - minMax.vMin.y) * 0.5f,
		(minMax.vMax.z - minMax.vMin.z) * 0.5f
	};

	float radius = sqrtf(extents.x * extents.x +
		extents.y * extents.y +
		extents.z * extents.z);

	BoundingSphere localSphere(center, radius);

	BoundingSphere worldSphere;

	localSphere.Transform(worldSphere, worldTransform);
	_vector camPos = XMLoadFloat4(&vCamPosition);     
	_vector camForward = XMLoadFloat3(&CameraForward); 
	_vector sphereCenter = XMLoadFloat3(&worldSphere.Center);

	_vector toObj = XMVectorSubtract(sphereCenter, camPos);
	_float dist = XMVectorGetX(XMVector3Dot(toObj, camForward));

	_float curve = (dist * dist) / 900 * 0.85;
	//_float curve = (dist * dist) / 1000 * 0;

	worldSphere.Center.y -= curve;
	_float maxExtent = max(extents.x, max(extents.y, extents.z));
	_float scaleFactor = 0.1f; // 여유 비율 (10%)
	worldSphere.Radius += maxExtent * scaleFactor;

	return m_Frustum.Intersects(worldSphere);
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

HRESULT CPipeLine::Begin_ObjectBuffer(ID3D11DeviceContext* pContext)
{
	HRESULT hr = pContext->Map(
		m_pDeviceObjectBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&m_mappedObjectBuffer
	);

	if (FAILED(hr))
		return hr;

	m_pObjectArray = reinterpret_cast<_float4x4*>(m_mappedObjectBuffer.pData);
	m_ObjectOffset = 0;
	return S_OK;
}

HRESULT CPipeLine::End_ObjectBuffer(ID3D11DeviceContext* pContext)
{
	pContext->Unmap(m_pDeviceObjectBuffer, 0);
	m_pObjectArray = nullptr;
	m_ObjectOffset = 0;
	return S_OK;
}

_uint CPipeLine::Write_SkinningBuffer(const vector<_float4x4>& bones)
{
	if (!m_pSkinningArray) return UINT_MAX;

	const _uint SkinningCount = static_cast<_uint>(bones.size());

	if (m_SkinningOffset + SkinningCount > g_iMaxNumBones) {
		MSG_BOX("To Many Skinning");
		return UINT_MAX; // 초과
	}

	memcpy(&m_pSkinningArray[m_SkinningOffset], bones.data(), sizeof(_float4x4) * SkinningCount);
	_uint LastOffset = m_SkinningOffset;
	m_SkinningOffset += SkinningCount;
	return LastOffset;
}

HRESULT CPipeLine::Begin_SkinningBuffer(ID3D11DeviceContext* pContext)
{
	HRESULT hr = pContext->Map(
		m_pDeviceSkinningBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&m_mappedSkinningBuffer
	);

	if (FAILED(hr))
		return hr;

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

HRESULT CPipeLine::Bind_Light(CShader* pShader, class CVIBuffer* pBuffer, ID3D11DeviceContext* pContext)
{

	auto& vector = CGameInstance::GetInstance()->Get_LightMgr()->Get_VisibleLight();
	if (vector.empty()) return E_FAIL;

	for (size_t i = 0; i < vector.size(); i++)
	{
		if (vector[i] == nullptr)
			continue;
		LIGHT_DESC desc = *vector[i]->Get_Desc(); // 값 복사

		SHADER_PARAM LightParam;
		LightParam.iSize = sizeof(_float4);
		LightParam.typeName = "float4";
		LightParam.pData = &desc.vLightDiffuse;
		pShader->Bind_Value("g_vLightDiffuse", LightParam);

		LightParam.pData = &desc.vLightAmbient;
		pShader->Bind_Value("g_vLightAmbient", LightParam);

		LightParam.pData = &desc.vLightDirection;
		pShader->Bind_Value("g_vLightDir", LightParam);

		LightParam.pData = &desc.vLightPosition;
		pShader->Bind_Value("g_vLightPos", LightParam);

		LightParam.pData = &desc.vLightSpecular;
		pShader->Bind_Value("g_vLightSpecular", LightParam);

		LightParam.iSize = sizeof(_float);
		LightParam.typeName = "float";
		LightParam.pData = &desc.fLightRange;
		pShader->Bind_Value("g_fLightRange", LightParam);

		ID3D11InputLayout* pLayout;

		switch (vector[i]->Get_Type())
		{
		case Engine::LIGHT_TYPE::DIRECTIONAL:
			m_pSystem->Get_BufferInputLayout(pBuffer, pShader, "Directional", &pLayout);
			pContext->IASetInputLayout(pLayout);
			pShader->Apply("Directional", pContext);
			pBuffer->Bind_Buffer(pContext);
			pBuffer->Render(pContext);
			break;
		case Engine::LIGHT_TYPE::POINT:
			m_pSystem->Get_BufferInputLayout(pBuffer, pShader, "Point", &pLayout);
			pContext->IASetInputLayout(pLayout);
			pShader->Apply("Point", pContext);
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

}
