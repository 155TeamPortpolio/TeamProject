#pragma once
#include "VIBuffer.h"
NS_BEGIN(Engine)

class CVI_Quad :
    public CVIBuffer
{
protected:
	CVI_Quad(const string& bufferID);
	CVI_Quad(const CVI_Quad& rhs);
	virtual ~CVI_Quad();

public:
	virtual HRESULT Initialize(ID3D11Device* pDevice)override;

private:
	virtual HRESULT Create_Vertex(ID3D11Device* pDevice) override;
	virtual HRESULT Create_Index(ID3D11Device* pDevice) override;

public:
	static CVI_Quad* Create(ID3D11Device* pDevice, const string& bufferID);
	virtual void Free() override;
};

NS_END
