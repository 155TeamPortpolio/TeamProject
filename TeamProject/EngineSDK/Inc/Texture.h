#pragma once
#include "Base.h"
NS_BEGIN(Engine)
class ENGINE_DLL CTexture final: public CBase 
{
private:
	CTexture();
	virtual ~CTexture() DEFAULT;
private:
	HRESULT Initialize(ID3D11Device* pDevice, const _tchar* filePath);

public:
	const string& Get_Key() { return m_TextureKey; }
	void Set_Key(const string& imguiID) { m_TextureKey = imguiID; }
	ID3D11ShaderResourceView* Get_SRV(){ return m_pShaderResourceView; };
	void Render_GUI(_float Width);
private:
	string m_TextureKey = {};
	ID3D11ShaderResourceView* m_pShaderResourceView = { nullptr };

public:
	static CTexture* Create(ID3D11Device* pDevice, const wstring& filePath, const string& textureKey);
	virtual void Free() override;
};
NS_END
