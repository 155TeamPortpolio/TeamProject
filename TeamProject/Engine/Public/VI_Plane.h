#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)
class ENGINE_DLL CVI_Plane final :
	public CVIBuffer
{
protected:
	CVI_Plane(const string& bufferID);
	CVI_Plane(const CVI_Plane& rhs);
	virtual ~CVI_Plane();

public:
	virtual HRESULT Initialize(ID3D11Device* pDevice)override;

private:
	virtual HRESULT Create_Vertex(ID3D11Device* pDevice) override;
	virtual HRESULT Create_Index(ID3D11Device* pDevice) override;

public:
	static CVI_Plane* Create(ID3D11Device* pDevice, const string& bufferID);
	virtual void Free() override;

};
NS_END
