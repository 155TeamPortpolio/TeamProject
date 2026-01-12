#pragma once
#include "Base.h"
NS_BEGIN(Engine)
class ENGINE_DLL CTexture final: public CBase 
{
private:
	CTexture();
	virtual ~CTexture() DEFAULT;
private:
	HRESULT Initialize(ID3D11Device* pDevice, const _tchar* filePath, _bool sRGBType);

public:
	const string& Get_Key() { return m_TextureKey; }
	void Set_Key(const string& imguiID) { m_TextureKey = imguiID; }
	ID3D11ShaderResourceView* Get_SRV(){ return m_pShaderResourceView; };
	void Render_GUI(_float Width);

	_uint2 Get_Size() { return m_TextureSize; };

private:
	void Extract_Size();
private:
	string m_TextureKey = {};
	_uint2 m_TextureSize = {};
	ID3D11ShaderResourceView* m_pShaderResourceView = { nullptr };
	ID3D11Resource* m_pResource = { nullptr };
public:
	static CTexture* Create(ID3D11Device* pDevice, const wstring& filePath, const string& textureKey, _bool sRGBType);
	virtual void Free() override;
};
NS_END
