#pragma once
#include "IResourceService.h"
#include "ResourceEntry.h"
#include "ThreadPool.h"

NS_BEGIN(Engine)
using Level_Resource = vector<unordered_map<string, CResourceEntry*>>;

class ENGINE_DLL CResourceThread final :
	public IResourceService
{
	enum RESOURCE { TEXTURE, MODELDATA, VI_BUFFER,MATERIALS, SHADER, RESOURCE_TYPE_COUNT };

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
	virtual class CShader* Load_Shader(const string& levelTag, const string& shaderKey) override;
	virtual class CTexture* Load_Texture(const string& levelTag, const string& textureKey, _bool sRGBType = false) override;
	virtual vector<class CAnimationClip*> Load_MetaClip(const string& levelTag, const string& MetaClipKey) override;
	virtual EFFECT_ASSET Load_EffectAsset(const string& levelTag, const string& effectTag) override;
	virtual class CComputeShader* Load_ComputeShader(const string& levelTag, const string& shaderKey) override;
	virtual void Load_InitialResource() override;

public:
	virtual CResourceEntry* Request_TextureEntry(const string& levelTag, const string& textureKey, _bool sRGBType = false);
	virtual CResourceEntry* Request_ModelEntry(const string& levelTag, const string& modelKey);
	virtual class CVIBuffer* Load_WaitVIBuffer(const string& levelTag, const string& bufferKey, BUFFER_TYPE eType);

public:
	virtual vector<class CMaterialInstance*> Load_MaterialFromFile(const string& levelTag, const string& fileKey) override;
	virtual _bool IsMaterialReady(const string& levelTag, const string& fileKey) override;

public:
	bool Begin_LoadAsync(const LoaderFunc& loaderFunc, const ScheduleFunc& scheduleFunc);
	void Pump_AllEntries_MainThread() override;

public:
	virtual HRESULT Add_ResourcePath(const string& resourceKey, const string& resourcePath) override;
	virtual string Get_ResourcePath(const string& resourceKey) override;
	
private:
	vector<CMaterialInstance*> Make_MaterialHandles(const vector<CMaterialData*>& dataList);
	vector<CMaterialData*>* GetOrLoad_MaterialData(const string& levelTag, const string& fileKey,_bool wait = false);
	shared_future<ResourceVariant> Schedule(JobFunc jobFunc);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CGameInstance* m_pInstance = { nullptr };

	unordered_map<string, string> m_PathByKey;
	unordered_map<string, Level_Resource> m_LevelResources;
	vector<CResourceEntry*> m_PendingDestroyEntries;

private:
	CThreadPool* m_pThreadPool = { nullptr };

private:

	CTexture* m_DefaultTexture = { nullptr };
	CModelData* m_DefaultModel = { nullptr };
public:
	static CResourceThread* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};


NS_END

