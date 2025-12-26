#include "Engine_Defines.h"
#include "ResourceThread.h"
#include "GameInstance.h"
#include "Helper_Func.h"

#include "VIBuffer.h"
#include "VI_Rect.h"
#include "VI_Cube.h"
#include "VI_Terrain.h"
#include "VI_Plane.h"
#include "VI_Point.h"
#include "VI_InstancePoint.h"

#include "Shader.h"
#include "Material.h"
#include "Texture.h"
#include "SoundData.h"
#include "MaterialData.h"
#include "ModelData.h"
#include "MaterialInstance.h"
#include "AnimationClip.h"
#include "AnimationLayout.h"
#include "ComputeShader.h"

CResourceThread::CResourceThread(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }, m_pContext{ pContext }, m_pInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pInstance);
}

HRESULT CResourceThread::Initiallize()
{
	return S_OK;
}


void CResourceThread::Clear_Resource(const string& levelTag)
{
	
}

HRESULT CResourceThread::Sync_To_Level()
{
	
	return S_OK;
}

CSoundData* CResourceThread::Load_Sound(const string& levelTag, const string& soundKey)
{
	return nullptr;
}

CVIBuffer* CResourceThread::Load_VIBuffer(const string& levelTag, const string& bufferKey, BUFFER_TYPE eType)
{
	return nullptr;
}

vector<CMaterialInstance*> CResourceThread::Load_MaterialFromFile(
	const string& levelTag,
	const string& fileKey)
{
	return {};

}

CShader* CResourceThread::Load_Shader(const string& levelTag, const string& shaderKey)
{
	return nullptr;
}

CTexture* CResourceThread::Load_Texture(const string& levelTag, const string& textureKey, _bool sRGBType)
{
    const string entryKey = MakeTextureEntryKey(textureKey, sRGBType);
  
    auto entryIter = m_TextureEntryByKey.find(entryKey);
    if (entryIter == m_TextureEntryByKey.end())
    {
        TEX_ENTRY newEntry;
        newEntry.pEntry = CResourceEntry::Create();
        newEntry.pEntry->SetSourcePath(newEntry.sourcePath);
        newEntry.levelTag = levelTag;
        newEntry.isSRGB = sRGBType;
  
        auto pathIter = m_PathByKey.find(textureKey);
  
        if (pathIter != m_PathByKey.end())
            newEntry.sourcePath = pathIter->second;
  
        newEntry.pTexture = CTexture::Create(m_pDevice,Helper::ConvertToWideString(newEntry.sourcePath),textureKey,sRGBType);
        newEntry.pEntry = CResourceEntry::Create();
  
        m_TextureEntryByKey.emplace(entryKey, std::move(newEntry));
        entryIter = m_TextureEntryByKey.find(entryKey);
    }
  
    TEX_ENTRY& textureEntry = entryIter->second;
  
    if (textureEntry.sourcePath.empty())
        return m_DefaultTexture;
  
    textureEntry.pEntry->Begin_LoadAsync(
        [sRGBType](const string& sourcePath, string& errorMsg) -> ResourceVariant
        {
            // TODO: 확장자 보고
            //  - DDS면 TEX_DDS_BYTES(파일 바이트) 반환
            //  - WIC면 TEX_CPU_DATA(디코드 결과) 반환
            return monostate{};
        }
    );
  
    // 조회
    if (textureEntry.pEntry->GetState() == LoadState::Ready)
        return textureEntry.pTexture;
  
    return m_DefaultTexture;
}

vector<CAnimationClip*> CResourceThread::Load_MetaClip(const string& levelTag, const string& MetaClipKey)
{
	return {};
}

EFFECT_ASSET CResourceThread::Load_EffectAsset(const string& levelTag, const string& effectTag)
{
	return {};
}

CComputeShader* CResourceThread::Load_ComputeShader(const string& levelTag, const string& shaderKey)
{
	return nullptr;
}

CModelData* CResourceThread::Load_ModelData(const string& levelTag, const string& ModelKey)
{
	return nullptr;
}

string CResourceThread::Get_ResourcePath(const string& resourceKey)
{
	return string();
}

HRESULT CResourceThread::Add_ResourcePath(const string& resourceKey, const string& resourcePath)
{
	return S_OK;
}

_int CResourceThread::ValidLevel(const string& levelKey)
{;
return 0;
}

string CResourceThread::MakeTextureEntryKey(const string& textureKey, _bool isSRGB)
{
	return textureKey + (isSRGB ? "#SRGB" : "#LINEAR");
}

void CResourceThread::Load_InitialResource()
{
}


string CResourceThread::MakePath(const string& pathKey)
{
	return "";
}

void CResourceThread::Pump_Textures(_uint maxCommitsPerFrame)
{
}


CResourceThread* CResourceThread::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CResourceThread* instance = new CResourceThread(pDevice, pContext);
	if (FAILED(instance->Initiallize())) {
		Safe_Release(instance);
	}
	return instance;
}

void CResourceThread::Free()
{
	__super::Free();
}
