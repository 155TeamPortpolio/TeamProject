#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVI_Tessellation :
    public CVIBuffer
{
protected:
	CVI_Tessellation(const string& bufferID);
	CVI_Tessellation(const CVI_Tessellation& rhs);
	virtual ~CVI_Tessellation();


public:
	HRESULT Initialize(ID3D11Device* pDevice, _uint iGridSize, _float fTotalSize);

private:
	HRESULT Create_Vertex(ID3D11Device* pDevice, _float fTotalSize);
	virtual HRESULT Create_Index(ID3D11Device* pDevice) override;

protected:
	_uint	m_iGridSize = 0;

public:
	static CVI_Tessellation* Create(ID3D11Device* pDevice, const string& bufferID, _uint iGridSize, _float fTotalSize);
	virtual void Free() override;
};

NS_END