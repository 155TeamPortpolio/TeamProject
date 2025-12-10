#pragma once
#include "VIBuffer.h"
NS_BEGIN(Engine)
class ENGINE_DLL CVI_Rect final :
    public CVIBuffer
{
protected:
	CVI_Rect(const string& bufferID);
	CVI_Rect(const CVI_Rect& rhs);
	virtual ~CVI_Rect();

public:
	virtual HRESULT Initialize(ID3D11Device* pDevice)override;

private:
	virtual HRESULT Create_Vertex(ID3D11Device* pDevice) override;
	virtual HRESULT Create_Index(ID3D11Device* pDevice) override;

public:
	static CVI_Rect* Create(ID3D11Device* pDevice, const string& bufferID) ;
	virtual void Free() override;

};
NS_END
