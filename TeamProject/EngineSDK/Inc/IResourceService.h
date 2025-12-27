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
    virtual vector<class CAnimationClip*> Load_MetaClip(const string& levelTag, const string& MetaClipKey) PURE;
    virtual EFFECT_ASSET Load_EffectAsset(const string& levelTag, const string& effectKey) PURE;
    virtual class CComputeShader* Load_ComputeShader(const string& levelTag, const string& shaderKey) PURE;
    virtual void Load_InitialResource() PURE;


    virtual string Get_ResourcePath(const string& resourceKey) PURE;
    virtual HRESULT Add_ResourcePath(const string& resourceKey, const string& resourcePath) PURE;
    virtual void Clear_Resource(const string& levelTag)PURE;
    virtual HRESULT Sync_To_Level()PURE;


public:
    virtual class CResourceEntry* Request_TextureEntry(const string& levelTag, const string& textureKey, _bool sRGBType = false) { return nullptr; };
    virtual class CResourceEntry* Request_ModelEntry(const string& levelTag, const string& modelKey) { return nullptr; };
    virtual void Pump_AllEntries_MainThread() {};

};

NS_END
