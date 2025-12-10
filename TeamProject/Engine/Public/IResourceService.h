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
    virtual class CTexture* Load_Texture(const string& levelTag, const string& textureKey) PURE;
    virtual class CShader* Load_Shader(const string& levelTag, const string& shaderKey) PURE;	
    virtual class CModelData* Load_ModelData(const string& levelTag, const string& ModelKey) PURE;
    virtual class CAnimationClip* Load_AnimClip(const string& levelTag, const string& AnimClipKey, const string& Subject) PURE;


    virtual string Get_ResourcePath(const string& resourceKey) PURE;
    virtual HRESULT Add_ResourcePath(const string& resourceKey, const string& resourcePath) PURE;
    virtual void Clear_Resource(const string& levelTag)PURE;
    virtual HRESULT Sync_To_Level()PURE;
};

NS_END
