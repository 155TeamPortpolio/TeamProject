#pragma once
#include "Base.h"
NS_BEGIN(Engine)
class CTarget_Manager :
    public CBase
{
	struct SavedState
	{
		ID3D11RenderTargetView* pPrevRTV = nullptr;
		ID3D11DepthStencilView* pPrevDSV = nullptr;
		D3D11_VIEWPORT PrevViewPort;
		UINT NumViewports = 1;
	};

private:
	CTarget_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTarget_Manager() DEFAULT;

public:
	HRESULT Create_Target(const RenderTargetDesc& targetDesc, _bool Engine = true);

public:
	HRESULT Add_MRT(const string& strMRTTag, const string& strTargetTag);
	HRESULT Begin_MRT(const string& strMRTTag, _bool Clear = true);
	HRESULT End_MRT();
	HRESULT Get_TargetParam(const string& strTargetTag, SHADER_PARAM& param);
	vector<class CRenderTarget*>& Find_MRT(const string& strMRTTag);
	ID3D11DepthStencilView* Get_MTR_DSV(const string& strMRTTag);

public:
	const string PostProcessToTargetName(POSTPROCESS type);

#ifdef _USING_GUI
	void Render_GUI();
#endif // _USING_GUI

public:
	class CRenderTarget* Get_CustomTarget(const string& strTargetTag);
	class CRenderTarget* Get_EngineTarget(const string& strTargetTag);
	void Push_Target(const string& key);
	void Pop_Target();

private:
	class CRenderTarget* Find_RenderTarget(const string& strTargetTag);
private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	unordered_map<string, class CRenderTarget*>							m_EngineRenderTargets;
	unordered_map<string, vector<class CRenderTarget*>>			m_MRTs;
	stack<SavedState> m_SaveEngineStates;

	ID3D11RenderTargetView* m_pBackBufferRTV = { nullptr };
	ID3D11DepthStencilView* m_pDSV = { nullptr };

#ifdef _USING_GUI
	_float width, height;
#endif // _USING_GUI

	/*Client Target*/
	unordered_map<string, class CRenderTarget*> m_CustomTargets;
	stack<SavedState> m_SaveCustomStates;

public:
	static CTarget_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END

