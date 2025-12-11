#pragma once
#include "Base.h"
NS_BEGIN(Engine)
class CRenderTarget :
    public CBase
{
private:
	CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderTarget() DEFAULT;

public:
	ID3D11RenderTargetView* Get_RTV() const {
		return m_pRTV;
	}
	ID3D11ShaderResourceView* Get_SRV() const {
		return m_pSRV;
	}
	ID3D11DepthStencilView* Get_DSV() const {
		return m_pDSV;
	}
	D3D11_VIEWPORT* Get_ViewPort() {
		return &m_viewPort;
	}
public:
	HRESULT Initialize(const RenderTargetDesc& targetDesc);
	void Clear();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	ID3D11Texture2D* m_pTexture2D = { nullptr };
	ID3D11RenderTargetView* m_pRTV = { nullptr };
	ID3D11ShaderResourceView* m_pSRV = { nullptr };
	ID3D11DepthStencilView* m_pDSV = { nullptr };
	_float4						m_vClearColor = {};
	D3D11_VIEWPORT m_viewPort = {};

public:
	static CRenderTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const RenderTargetDesc& targetDesc);
	virtual void Free() override;
};

NS_END