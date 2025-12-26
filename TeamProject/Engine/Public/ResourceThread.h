#pragma once
#include "IResourceService.h"
#include "ResourceEntry.h"

NS_BEGIN(Engine)

class CResourceThread final :
	public IResourceService
{

private:
	CResourceThread(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CResourceThread() DEFAULT;

public:
	HRESULT Initiallize();
	virtual void Clear_Resource(const string& levelTag)override;
	virtual HRESULT Sync_To_Level()override;

public:
	virtual class CSoundData* Load_Sound(const string& levelTag, const string& soundKey) override;

	virtual class CModelData* Load_ModelData(const string& levelTag, const string& ModelKey) override;
	virtual class CVIBuffer* Load_VIBuffer(const string& levelTag, const string& bufferKey, BUFFER_TYPE eType) override;
	virtual vector<class CMaterialInstance*> Load_MaterialFromFile(const string& levelTag, const string& fileKey) override;
	virtual class CShader* Load_Shader(const string& levelTag, const string& shaderKey) override;
	virtual class CTexture* Load_Texture(const string& levelTag, const string& textureKey, _bool sRGBType = false) override;
	virtual vector<class CAnimationClip*> Load_MetaClip(const string& levelTag, const string& MetaClipKey) override;
	virtual EFFECT_ASSET Load_EffectAsset(const string& levelTag, const string& effectTag) override;
	virtual class CComputeShader* Load_ComputeShader(const string& levelTag, const string& shaderKey) override;

	// 경로 등록 (키 -> 파일 경로)
	//HRESULT Add_ResourcePath(const std::string& resourceKey, const std::string& resourcePath);

	virtual string Get_ResourcePath(const string& resourceKey) override;
	virtual HRESULT Add_ResourcePath(const string& resourceKey, const string& resourcePath) override;

	virtual void Load_InitialResource() override;
private:
	_int ValidLevel(const string& levelKey);
	string MakeTextureEntryKey(const string& textureKey, _bool isSRGB);
private:
	string MakePath(const string& pathKey);
	void Pump_Textures(_uint maxCommitsPerFrame);
private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CGameInstance* m_pInstance = { nullptr };

	unordered_map<string, string> m_PathByKey;
	struct TEX_ENTRY
	{
		string levelTag;
		string sourcePath;
		_bool isSRGB = false;
		CTexture* pTexture = { nullptr };
		CResourceEntry* pEntry = { nullptr };      
	};

	unordered_map<string, TEX_ENTRY> m_TextureEntryByKey;

private:
	CTexture* m_DefaultTexture = { nullptr };

public:
	static CResourceThread* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};


NS_END

