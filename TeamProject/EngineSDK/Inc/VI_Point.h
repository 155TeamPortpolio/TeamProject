#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)
class ENGINE_DLL CVI_Point final :
	public CVIBuffer
{
protected:
	CVI_Point(const string& bufferID);
	CVI_Point(const CVI_Point& rhs);
	virtual ~CVI_Point();

public:
	virtual HRESULT Initialize(ID3D11Device* pDevice)override;
	virtual HRESULT Bind_Buffer(ID3D11DeviceContext* pContext) override;
	virtual HRESULT Render(ID3D11DeviceContext* pContext) override;

private:
	virtual HRESULT Create_Vertex(ID3D11Device* pDevice) override;

public:
	static CVI_Point* Create(ID3D11Device* pDevice, const string& bufferID);
	virtual void Free() override;
};
NS_END
