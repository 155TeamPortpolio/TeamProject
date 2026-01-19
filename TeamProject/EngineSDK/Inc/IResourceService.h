#pragma once
#include "IService.h"

NS_BEGIN(Engine)

class ENGINE_DLL IResourceService : public IService
{
protected:
	~IResourceService() DEFAULT;
public:
    virtual HRESULT Initiallize()PURE;

    virtual class CSoundData* Load_Sound(const string& levelTag, const string& SoundKey) PURE;
    virtual class CVIBuffer* Load_VIBuffer(const string& levelTag, const string& bufferKey, BUFFER_TYPE eType) PURE;
    virtual vector<class CMaterialInstance*> Load_MaterialFromFile(const string& levelTag, const string& fileKey) PURE;
    virtual class CTexture* Load_Texture(const string& levelTag, const string& textureKey, _bool sRGBType = false) PURE;
    virtual class CShader* Load_Shader(const string& levelTag, const string& shaderKey) PURE;	
    virtual class CModelData* Load_ModelData(const string& levelTag, const string& ModelKey) PURE;
    virtual ANIMATION_META Load_MetaClip(const string& levelTag, const string& MetaKey) PURE;
    virtual EFFECT_ASSET Load_EffectAsset(const string& levelTag, const string& effectKey) PURE;
    virtual class CComputeShader* Load_ComputeShader(const string& levelTag, const string& shaderKey) PURE;
    virtual void Load_InitialResource() PURE;


    virtual string Get_ResourcePath(const string& resourceKey) PURE;
    virtual HRESULT Add_ResourcePath(const string& resourceKey, const string& resourcePath) PURE;
    virtual void Clear_Resource(const string& levelTag)PURE;
    virtual HRESULT Sync_To_Level()PURE;


    virtual _bool RequestPreload(const PreloadKey& key) PURE;
    virtual void PumpPreloads(vector<PreloadCompleted>& outCompleted) PURE;
    virtual void GetPreloadProgress(_uint& outDone, _uint& outTotal) const PURE;
    virtual _bool isLoadComplete() const PURE;
};

NS_END
