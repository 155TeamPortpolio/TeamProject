#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)
class ENGINE_DLL CVI_Cube final :
	public CVIBuffer
{
protected:
	CVI_Cube(const string& bufferID);
	CVI_Cube(const CVI_Cube& rhs);
	virtual ~CVI_Cube();

public:
	virtual HRESULT Initialize(ID3D11Device* pDevice)override;

private:
	virtual HRESULT Create_Vertex(ID3D11Device* pDevice) override;
	virtual HRESULT Create_Index(ID3D11Device* pDevice) override;

public:
	static CVI_Cube* Create(ID3D11Device* pDevice, const string& bufferID);
	virtual void Free() override;
};
NS_END
