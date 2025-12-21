#pragma once
#include "Base.h"
NS_BEGIN(Engine)

class CResourceEntry :
	public CBase
{
	enum class ResourceType : uint8_t { Texture, Model, Shader, Material, Audio, Animation, Unknown };
	enum class LoadState : uint8_t { Unloaded, Loading, Ready, Failed };

	struct ResourceKey {
		ResourceType type{};
		uint64_t keyHash{};
		bool operator==(const ResourceKey& r) const { return type == r.type && keyHash == r.keyHash; }
	};

	struct ResourceKeyHash {
		size_t operator()(const ResourceKey& k) const noexcept {
			return size_t(k.keyHash) ^ (size_t(k.type) * 0x9e3779b97f4a7c15ull);
		}
	};

	using ResourceVariant = std::variant<
		monostate,
		class CTexture*,
		class CModelData*,
		class CShader*,
		class CMaterialData*,
		class CAnimationClip*
	>;

private:
	CResourceEntry();
	~CResourceEntry();

private:
	void Reset();
	
private:
	ResourceKey m_ID{};
	string m_SourcePath;

private:
	atomic<LoadState> state{ LoadState::Unloaded };

	// 아래 3개는 같이 갱신되므로 뭉텅이로 보호
	mutable mutex m_Mutex;
	ResourceVariant m_Resource;
	shared_future<ResourceVariant> m_LoadingTask;

private:
	string m_LastErrorMsg;
	string m_DebugName;

public:
	static CResourceEntry* Create();
	virtual void Free();
};

NS_END