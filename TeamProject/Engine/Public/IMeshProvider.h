#pragma once
#include "Engine_Defines.h"
NS_BEGIN(Engine)
class IMeshProvider {
public:
	virtual ~IMeshProvider() DEFAULT;
public:
	virtual const	D3D11_INPUT_ELEMENT_DESC* Get_ElementDesc(_uint DrawIndex) PURE;
	virtual const	_uint Get_ElementCount(_uint DrawIndex) PURE;
	virtual const	string_view Get_ElementKey(_uint DrawIndex) PURE;
	virtual				HRESULT Draw(ID3D11DeviceContext* pContext, _uint Index) PURE;
	virtual				_uint Get_MeshCount() PURE;

};

NS_END