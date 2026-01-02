#pragma once
#include "Base.h"
NS_BEGIN(Engine)
class ENGINE_DLL CPipeLine :
	public CBase
{
	struct alignas(16)  FrameBuffer
	{
		_float4x4 matView;
		_float4x4 matProjection;
		_float4x4 matViewInverse;
		_float4x4 matProjectionInverse;
		_float4x4 matOrthograph;
		_float4 vCamPosition;
		_float3 CameraForward;
		_float zFar;
	};


	struct alignas(16)  ShadowBuffer
	{
		_float4x4 matShadowView;
		_float4x4 matShadowProjection;
		_float4x4 matShadowViewInverse;
		_float4x4 matShadowProjectionInverse;
		_float4x4 matLightViewProj[4];      // 각 Cascade의 Light VP
		_vector vCascadeSplits;          // (split1, split2, split3, split4)
		_float4 vShadowPosition;
		_float zShadowFar;
		_int iCurrentCascade;

		_float2 ShadowPadding;
	};

	struct alignas(16)  LightBuffer
	{
		_float4 vLightDir;
		_float4 vLightPos;
		_float4 vLightDiffuse;
		_float4 vLightAmbient;
		_float4 vLightSpecular;
		_float fLightRange;
		_float fLightIntensity;
		_int iLightSize;
		_float LightPadding;
	};

	struct alignas(16)  SSAOBuffer
	{
		_float Radius;
		_float Bias;
		_float ScreenWidth;
		_float ScreenHeight;
	};

	struct SSAOKernel
	{
		_float4 SSAOKernel[64];     //고민중 따로 뺄지 같이할지
	};

	struct SkinningBuffer
	{
		_float4x4 SkinningBuffer[g_iMaxNumBones];
	};


private:
	CPipeLine();
	virtual ~CPipeLine() DEFAULT;

public:
	HRESULT Initialize(ID3D11Device* pDevice, class CRenderSystem* pSystem);
public:
	_bool isVisible(MINMAX_BOX minMax, _fmatrix worldTransform);
public:
	HRESULT Update_FrameBuffer(ID3D11DeviceContext* pContext);
	HRESULT Update_ShadowBuffer(ID3D11DeviceContext* pContext, _int cascadeIndex);
	HRESULT Update_LightBuffer(ID3D11DeviceContext* pContext, const LIGHT_DESC& Desc, _int lightSize);
	HRESULT Update_SSAOBuffer(ID3D11DeviceContext* pContext);
	HRESULT Write_SSAOKernelBuffer(ID3D11Device* pDevice);
	void Update_Frustum();
	void Update_HiZ(ID3D11DeviceContext* pContext);

	_uint Write_ObjectData(const _float4x4& worldMatrix);
	HRESULT Begin_ObjectBuffer(ID3D11DeviceContext* pContext);
	HRESULT End_ObjectBuffer(ID3D11DeviceContext* pContext);

	_uint Write_SkinningBuffer(vector<_float4x4> BoneMatrices);
	HRESULT Begin_SkinningBuffer(ID3D11DeviceContext* pContext);
	HRESULT End_SkinningBuffer(ID3D11DeviceContext* pContext);

	void Begin_ShadowRender(_uint cascadeIndex);
	void End_ShadowRender();

#ifdef _USING_GUI
	void Render_GUI();
#endif // _USING_GUI

public:
	ID3D11Buffer* Get_FrameBuffer() { return m_pDeviceFrameBuffer; };
	ID3D11Buffer* Get_ShadowBuffer() { return m_pDeviceShadowBuffer; };
	ID3D11Buffer* Get_SSAOBuffer() { return m_pDeviceSSAOBuffer; };
	ID3D11Buffer* Get_SSAOKernelBuffer() { return m_pDeviceSSAOKernelBuffer; };
	ID3D11Buffer* Get_LightBuffer() { return m_pDeviceLightBuffer; };

	ID3D11ShaderResourceView* Get_ObjectResource() { return m_pObjectResource; };
	ID3D11ShaderResourceView* Get_SkinningResource() { return m_pSkinningResource; };

	HRESULT Bind_Light(class CShader* pShader, class CVIBuffer* pBuffer, ID3D11DeviceContext* pContext, class CRenderer* pRenderer);
	vector<OPAQUE_PACKET> OcculsionCulling(const vector<OPAQUE_PACKET>& frustums);

private:
	ID3D11Buffer* m_pDeviceFrameBuffer = { nullptr };
	ID3D11Buffer* m_pDeviceShadowBuffer = { nullptr };
	ID3D11Buffer* m_pDeviceSSAOBuffer = { nullptr };
	ID3D11Buffer* m_pDeviceSSAOKernelBuffer = { nullptr };
	ID3D11Buffer* m_pDeviceLightBuffer = { nullptr };

	/*Transform*/
	_uint m_ObjectOffset = {};
	_float4x4* m_pObjectArray = nullptr;

	ID3D11Buffer* m_pDeviceObjectBuffer = { nullptr };
	ID3D11ShaderResourceView* m_pObjectResource = { nullptr };
	D3D11_MAPPED_SUBRESOURCE m_mappedObjectBuffer = {};

	/*Matrix*/
	_uint m_SkinningOffset = {};
	_float4x4* m_pSkinningArray = nullptr;
	ID3D11Buffer* m_pDeviceSkinningBuffer = { nullptr };
	ID3D11ShaderResourceView* m_pSkinningResource = { nullptr };
	D3D11_MAPPED_SUBRESOURCE m_mappedSkinningBuffer = {};

	class CRenderSystem* m_pSystem = { nullptr };
	BoundingFrustum m_Frustum;

private:
	class CHiZ_Culling* m_pHiZ = { nullptr };
	class CCSMShadow* m_pCSM = { nullptr };

public:
	static CPipeLine* Create(ID3D11Device* pDevice, class CRenderSystem* pSystem);
	virtual void Free() override;
};
NS_END