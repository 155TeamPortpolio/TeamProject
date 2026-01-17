#pragma once
#include "Base.h"
NS_BEGIN(Engine)
class ENGINE_DLL CTexture final : public CBase
{
	struct CB_ALPHA_INSPECT
	{
		_uint2 textureSize;
		_uint  mipIndex;
		_float epsilon;
		_uint  padding0;
	};
	struct AlphaGlobalGPU
	{
		ID3D11Buffer* resultBuffer = nullptr;
		ID3D11UnorderedAccessView* resultUav = nullptr;
		ID3D11Buffer* stagingBuffer = nullptr;
		void Release() {
			Safe_Release(resultBuffer);
			Safe_Release(resultUav);
			Safe_Release(stagingBuffer);
		}
	};

public:
	enum TestLevel {None = 0,Fast = 1,Hint = 2,Precise = 3};
	struct AlphaDesc {
		TestLevel eLevel = { None };
		_bool hasNonOpaque = {false}; //알파 != 1존재
		_bool hasAverage = { false };
		_float AlphaAverage = { 1.0f };
	};

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
	_bool Get_TextureDesc(D3D11_TEXTURE2D_DESC& outDesc);
	_uint ChooseMipForAlpha(TestLevel level);
	void GetMipSize(_uint mipIndex, _uint& outWidth, _uint& outHeight);
	void CalcDispatchGroups(_uint mipWidth, _uint mipHeight, _uint& outGroupX, _uint& outGroupY);

private:
	_bool AlphaCheck(TestLevel level);
	HRESULT CreateAlphaGlobalBuffers(ID3D11Device* device, AlphaGlobalGPU& outGpu);
	void ClearAlphaGlobal(ID3D11DeviceContext* context, ID3D11UnorderedAccessView* uav);
private:
	string m_TextureKey = {};
	_uint2 m_TextureSize = {};
	ID3D11ShaderResourceView* m_pShaderResourceView = { nullptr };
	ID3D11Resource* m_pResource = { nullptr };
	D3D11_TEXTURE2D_DESC m_Desc = {};
	ID3D11Texture2D* m_pTexture = {};

private:
	AlphaDesc m_AlphaDesc = {};
	AlphaGlobalGPU m_AlphaGlobal = {};
	ID3D11Buffer* m_pAlphaInspectCB = {};
public:
	static CTexture* Create(ID3D11Device* pDevice, const wstring& filePath, const string& textureKey, _bool sRGBType);
	virtual void Free() override;
};
NS_END
