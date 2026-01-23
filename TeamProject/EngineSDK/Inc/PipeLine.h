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
		_float4x4 matStaticLightViewProj[4];     
		_float4x4 matSkinnedLightViewProj[4];     
		_vector vCascadeSplits;          
		_float4 vShadowPosition;
		_int iCurrentCascade;
		_float zShadowFar;

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
	HRESULT Update_ShadowBuffer(ID3D11DeviceContext* pContext, _bool IsSkinningMesh, _int cascadeIndex);
	HRESULT Update_LightBuffer(ID3D11DeviceContext* pContext, const LIGHT_DESC& Desc, _int lightSize);
	HRESULT Update_SSAOBuffer(ID3D11DeviceContext* pContext);
	HRESULT Write_SSAOKernelBuffer(ID3D11Device* pDevice);
	void Update_Frustum();
	void Update_StaticCSM();
	void Update_SkinnedCSM();
	void Update_HiZ(ID3D11DeviceContext* pContext);

	HRESULT Begin_ObjectBuffer(ID3D11DeviceContext* pContext);
	_uint Write_ObjectData(const _float4x4& worldMatrix);
	_uint GetOrWriteTransform(_uint objId, const _float4x4& world);
	HRESULT End_ObjectBuffer(ID3D11DeviceContext* pContext);


	HRESULT Begin_SkinningBuffer(ID3D11DeviceContext* pContext);
	_uint GetOrWriteSkinning(_uint objId, _uint drawIndex, const vector<_float4x4>& bones);
	_uint Write_SkinningBuffer(const vector<_float4x4>& bones);
	HRESULT End_SkinningBuffer(ID3D11DeviceContext* pContext);

	void Begin_ShadowRender(_bool IsSkinningMesh, _uint cascadeIndex);
	void End_ShadowRender(_bool IsSkinningMesh);

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
	ID3D11DepthStencilView* GetCSMDSV(_bool IsSkinningMesh, _uint index) const;

	HRESULT Bind_ShadowMap(class CShader* pShader);
	void BindSampler(ID3D11DeviceContext* pContext, _uint slot);
	HRESULT Bind_Light(class CShader* pShader, class CVIBuffer* pBuffer, ID3D11DeviceContext* pContext, class CRenderer* pRenderer);
	vector<OPAQUE_PACKET> OcculsionCulling(const vector<OPAQUE_PACKET>& frustums);


private:

private:
	ID3D11Buffer* m_pDeviceFrameBuffer = { nullptr };
	ID3D11Buffer* m_pDeviceShadowBuffer = { nullptr };
	ID3D11Buffer* m_pDeviceSSAOBuffer = { nullptr };
	ID3D11Buffer* m_pDeviceSSAOKernelBuffer = { nullptr };
	ID3D11Buffer* m_pDeviceLightBuffer = { nullptr };
	ShadowBuffer PreShadowBuffer;

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
	_float4x4 identity;

private:
	class CHiZ_Culling* m_pHiZ = { nullptr };
	class CCSMShadow* m_pStaticCSM = { nullptr };
	class CCSMShadow* m_pSkinnedCSM = { nullptr };

	/*Frame Cache*/
private:
	unordered_map<uint64_t, _uint> m_skinningCache;
	unordered_map<_uint, _uint> m_transformIndexCache;

public:
	static CPipeLine* Create(ID3D11Device* pDevice, class CRenderSystem* pSystem);
	virtual void Free() override;
};
NS_END