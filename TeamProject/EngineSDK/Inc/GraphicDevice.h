#pragma once
#include "IGraphicService.h"
NS_BEGIN(Engine)
class CGraphicDevice :
	public IGraphicService
{
private:
	CGraphicDevice();
	virtual ~CGraphicDevice() override;

public: 
	HRESULT Initialize(HWND hWnd, WINMODE isWindowed, _uint iWinSizeX, _uint iWinSizeY,
		 ID3D11Device** ppDevice,  ID3D11DeviceContext** ppContext);

	virtual HRESULT Clear_BackBuffer_View(const _float4* pClearColor) override;
	virtual HRESULT Clear_DepthStencil_View() override;
	virtual HRESULT Present() override;

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
	IDXGISwapChain* m_pSwapChain = { nullptr };
	ID3D11RenderTargetView* m_pBackBufferRTV = { nullptr };
	ID3D11DepthStencilView* m_pDepthStencilView = { nullptr };

	vector< ID3D11DepthStencilView*> m_RequestedDepthStencil;

private:
	HRESULT Ready_SwapChain(HWND hWnd, WINMODE isWindowed, _uint iWinCX, _uint iWinCY);
	HRESULT Ready_BackBufferRenderTargetView();
	HRESULT Ready_DepthStencilView(_uint iWinCX, _uint iWinCY);

public:
	static  CGraphicDevice* Create(ENGINE_DESC engine, ID3D11Device** ppDevice, ID3D11DeviceContext** ppDeviceContextOut);
	virtual void Free() override;
};
NS_END
