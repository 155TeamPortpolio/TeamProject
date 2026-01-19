#pragma once
#include "IResourceService.h"

NS_BEGIN(Engine)

class CResourceMgr final :
	public IResourceService
{
	struct RS_Pool {
		unordered_map<string, class CShader*>					m_Shaders;
		unordered_map<string, class CVIBuffer*>					m_Buffers;
		unordered_map<string, class CModelData*>				m_ModelDatas;
		unordered_map<string, vector<class CMaterialData*>>		m_MaterialInstances;
		unordered_map<string, class CTexture*>					m_Textures;
		unordered_map<string, class CSoundData*>				m_Sounds;
		unordered_map<string, ANIMATION_META>					m_AnimationMetas;
		unordered_map<string, EFFECT_ASSET>						m_EffectAssets;
		unordered_map <string, class CComputeShader*>			m_ComputeShaders;

		mutable mutex shaderMutex;
		mutable mutex textureMutex;
		mutable mutex soundMutex;
		mutable mutex modelMutex;
		mutable mutex materialMutex;
		mutable mutex bufferMutex;
		mutable mutex animMutex;
		mutable mutex effectMutex;
		mutable mutex computeMutex;
	};

private:
	CResourceMgr(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CResourceMgr() DEFAULT;

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
	virtual ANIMATION_META Load_MetaClip(const string& levelTag, const string& MetaKey) override;
	virtual EFFECT_ASSET Load_EffectAsset(const string& levelTag, const string& effectTag) override;
	virtual class CComputeShader* Load_ComputeShader(const string& levelTag, const string& shaderKey) override;

	virtual string Get_ResourcePath(const string& resourceKey) override;
	virtual HRESULT Add_ResourcePath(const string& resourceKey, const string& resourcePath) override;
	virtual void Load_InitialResource() override;

public:
	_bool RequestPreload(const PreloadKey& key);
	void PumpPreloads(vector<PreloadCompleted>& outCompleted);
	void GetPreloadProgress(_uint& outDone, _uint& outTotal) const;
	virtual _bool isLoadComplete() const override;

private:
	_int ValidLevel(const string& levelKey);
	string MakePath(const string& pathKey);
	vector<CMaterialData*>* GetOrLoad_MaterialData(const string& levelTag, const string& fileKey);
	vector<CMaterialInstance*> Make_MaterialHandles(const vector<CMaterialData*>& dataList);

private:
	void Init_PreLoader();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CGameInstance* m_pInstance = { nullptr };

	///vector<RS_Pool >m_Resources;
	deque<RS_Pool> m_Resources; 
	unordered_map<string, _uint> m_LevelIndex;

	unordered_map<string, string>m_KeyPath;
	mutable mutex m_keyPathMutex;

	unordered_map<string, vector<string>>m_MateriaFileKeys;

private:
	class CPreloadScheduler* m_pPreloader = { nullptr };
public:
	static CResourceMgr* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};


NS_END

