#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)
class ENGINE_DLL CVI_Trail : public CVIBuffer
{
private:
	CVI_Trail(const string& bufferID);
	CVI_Trail(const CVI_Trail& rhs);
	virtual ~CVI_Trail();
public:
	virtual HRESULT Initialize(ID3D11Device* pDevice)override;
	virtual HRESULT Bind_Buffer(ID3D11DeviceContext* pContext) override;
	virtual HRESULT Render(ID3D11DeviceContext* pContext) override;

public:
	void Update_Vertices(VTXTRAIL* pVertices, _uint iCount);

private:
	virtual HRESULT Create_Vertex(ID3D11Device* pDevice) override;
	virtual HRESULT Create_Index(ID3D11Device* pDevice) override;

public:
	static CVI_Trail* Create(ID3D11Device* pDevice, const string& bufferID);
	virtual void Free() override;

private:
	_uint m_iCurrPointCount{};

};
NS_END