#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)
class CVI_InstancePoint :
    public CVIBuffer
{
public:
	typedef struct tagCBPacked
	{
		_uint iSystemID{};
		_uint iMaxCount{};
		_uint iVertexCountPerInstance{};
		_uint iCapacity{};
	}CB_PACKED;

	typedef struct tagDrawInstArgs
	{
		_uint iVertexCounterPerInstance{};
		_uint iInstanceCount{};
		_uint iStartVertexLocation{};
		_uint iStartInstanceLocation{};
	}DRAW_INST_ARGS;
protected:
	CVI_InstancePoint(const string& bufferID);
	CVI_InstancePoint(const CVI_InstancePoint& rhs);
	virtual ~CVI_InstancePoint();

public:
	virtual HRESULT Initialize(ID3D11Device* pDevice)override;
	virtual HRESULT Bind_Buffer(ID3D11DeviceContext* pContext) override;
	virtual HRESULT Render(ID3D11DeviceContext* pContext) override;

private:
	virtual HRESULT Create_Vertex(ID3D11Device* pDevice) override;
	HRESULT Create_InstanceBuffer(ID3D11Device * pDevice);

public:
	static CVI_InstancePoint* Create(ID3D11Device* pDevice, const string& bufferID);
	virtual void Free() override;

public:
	void Update_InstanceBuffer(ID3D11DeviceContext* pContext, const VTX_INSTANCE_POINT* instanceData, _uint numInstance);

private:
	ID3D11Buffer* m_pInstanceBuffer = { nullptr };		//인스턴스 데이터
	_uint m_iMaxInstancesCount{};
	_uint m_iInstanceStride{};
	_uint m_iNumUsedInstances{};

};
NS_END
