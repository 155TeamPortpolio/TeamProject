#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CRenderer abstract :
    public CBase
{
protected:
    CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CRenderer();
    HRESULT Initialize(class CTarget_Manager* pTargetManager, class CPipeLine* pPipeLine);

public:
	ID3D11ShaderResourceView* Get_CustomTargetSRV(const string strTag);
	ID3D11ShaderResourceView* Get_EngineTargetSRV(const string strTag);

protected:
	virtual HRESULT Ready_Target() PURE;
	virtual HRESULT Ready_MRT() PURE;
	HRESULT LoadShader(string shaderName);
	HRESULT CreateVIBuffer();
	HRESULT Bind_WorldMatrix();

protected:
	HRESULT Change_Viewport(_uint iWidth, _uint iHeight);

public:
	virtual HRESULT Get_InputLayout(class CModel* pModel, class CShader* pShader, _uint DrawIndex,
		const string& passConstant, ID3D11InputLayout** ppInputLayout);
	virtual  HRESULT Get_BufferInputLayout(class CVIBuffer* pBuffer, class CShader* pShader,
		const string& passConstant, ID3D11InputLayout** ppInputLayout);

protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	unordered_map<string, ID3D11InputLayout*> m_InputLayouts;

	class CPipeLine*		m_pPipeLine = { nullptr };

	class CTarget_Manager*	m_pTargetManager = { nullptr };
	class CVIBuffer*		m_pVIBuffer = { nullptr };
	class CShader*			m_pShader = { nullptr };
	_float4x4				m_WorldMatrix;

protected:
	class CTexture* m_pRampTexture;

public:
	virtual void Free() override;
};

NS_END