#pragma once
#include "IService.h"
NS_BEGIN(Engine)
class ENGINE_DLL IFontService abstract :
public IService
{
protected:
	virtual  ~IFontService() DEFAULT;
public:
	virtual HRESULT Add_Font(string FontName, const wstring& FontFilePath) PURE;
	virtual HRESULT Push_Text(TEXT_INFO info) PURE;
	virtual HRESULT Render_Font() PURE;
	virtual class CCustomFont* Get_Font(string FontName) PURE;
};
NS_END
