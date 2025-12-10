#pragma once
#include "Base.h"
NS_BEGIN(Engine)
class ENGINE_DLL CVIBuffer abstract:public CBase
{
protected:
	CVIBuffer(const string& ModelKey);
	CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer();

public:
	virtual HRESULT Initialize(ID3D11Device* pDevice);
	virtual HRESULT Bind_Buffer(ID3D11DeviceContext* pContext);
	virtual HRESULT Render(ID3D11DeviceContext* pContext);

	const string& Get_Key() { return m_VIKey; }
	virtual void Render_GUI();
	virtual const D3D11_INPUT_ELEMENT_DESC* Get_ElementDesc()	{ return m_ElementDesc; };
	virtual const _uint Get_ElementCount()															{ return m_ElementCount; };
	virtual const string_view Get_ElementKey()												{ return m_ElementKey; };

public:
	virtual _uint Get_VertexStride()																	{ return m_iVertexStride; };
	virtual _uint Get_VertexCount()																	{ return m_iVerticesCount; };
	virtual _uint Get_IndexCount()																	{ return m_iIndicesCount; };
	virtual ID3D11Buffer* Get_VertexBuffer()												{ return m_pVB; };
	virtual ID3D11Buffer* Get_IndexBuffer()													{ return m_pIB; };

protected:
	virtual HRESULT Create_Vertex(ID3D11Device* pDevice);
	virtual HRESULT Create_Index(ID3D11Device* pDevice);

protected:
	ID3D11Buffer* m_pVB = {nullptr};
	ID3D11Buffer* m_pIB = { nullptr };

	/*버텍스 버퍼의 개수(버퍼 개수)*/
	_uint m_iVertexBufferCount = {};

	/*버텍스의 개수*/
	_uint m_iVerticesCount = {};
	_uint m_iVertexStride = {};

	_uint m_iIndicesCount = {};
	_uint m_iIndexStride = {};

	DXGI_FORMAT m_eIndexFormat = {};
	D3D_PRIMITIVE_TOPOLOGY m_ePrimitive = {};

	string m_VIKey;

	/**/
	const D3D11_INPUT_ELEMENT_DESC* m_ElementDesc = { nullptr };
	_uint m_ElementCount = {};
	string_view m_ElementKey = {};

public:
	virtual void Free() override;
};
NS_END
