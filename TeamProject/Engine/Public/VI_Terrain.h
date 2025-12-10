#pragma once
#include "VIBuffer.h"
NS_BEGIN(Engine)

class ENGINE_DLL CVI_Terrain :
    public CVIBuffer
{
protected:
	CVI_Terrain(const string& imguiID);
	CVI_Terrain(const CVI_Terrain& rhs);
	virtual ~CVI_Terrain();

public:
	virtual HRESULT Initialize(ID3D11Device* pDevice, const string& HeightfilePath);

private:
	virtual HRESULT Create_Vertex(ID3D11Device* pDevice) override;
	virtual HRESULT Create_Index(ID3D11Device* pDevice) override;

private:
	void ComputeNormal();

private:
	_uint* pPixels = {nullptr};
	VTXNORMTEX* m_VBContainer = { nullptr };
	_uint* m_IBContainer = { nullptr };

	_uint iVerticesCountX = {};
	_uint iVerticesCountZ = {};

public:
	static CVI_Terrain* Create(ID3D11Device* pDevice, const string& imguiID, const string& HeightfilePath );
	virtual void Free() override;

};
NS_END
