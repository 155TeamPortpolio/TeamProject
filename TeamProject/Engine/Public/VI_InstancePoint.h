#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)
class CVI_InstancePoint :
    public CVIBuffer
{
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
	HRESULT Create_InstanceBuffer(ID3D11Debug* pDevice);

public:
	static CVI_InstancePoint* Create(ID3D11Device* pDevice, const string& bufferID);
	virtual void Free() override;

private:
	ID3D11Buffer* m_pInstanceBuffer = { nullptr };

};
NS_END
