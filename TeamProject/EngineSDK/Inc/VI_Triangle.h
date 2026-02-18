#pragma once
#include "VIBuffer.h"
NS_BEGIN(Engine)

class CVI_Triangle :
    public CVIBuffer
{
protected:
	CVI_Triangle(const string& bufferID);
	CVI_Triangle(const CVI_Triangle& rhs);
	virtual ~CVI_Triangle();

public:
	virtual HRESULT Initialize(ID3D11Device* pDevice)override;

private:
	virtual HRESULT Create_Vertex(ID3D11Device* pDevice) override;
	virtual HRESULT Create_Index(ID3D11Device* pDevice) override;

public:
	static CVI_Triangle* Create(ID3D11Device* pDevice, const string& bufferID);
	virtual void Free() override;
};

NS_END
