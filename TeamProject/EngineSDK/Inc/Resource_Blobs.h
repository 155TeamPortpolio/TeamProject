#pragma once
#include "Engine_Defines.h"

namespace Engine {
	enum class ResourceType : uint8_t { Texture, Model, Shader, Material, Audio, Animation, Unknown };
	enum class LoadState : uint8_t { Unloaded, Loading, Committing, Ready, Failed };
	struct ResourceKey {
		uint64_t levelHash{};
		ResourceType type{};
		uint64_t keyHash{};
		bool operator==(const ResourceKey& rhs) const {
			return levelHash == rhs.levelHash && type == rhs.type && keyHash == rhs.keyHash;
		}
	};
	struct ResourceKeyHash {
		size_t operator()(const ResourceKey& resourceKey) const noexcept {
			size_t combined = size_t(resourceKey.levelHash);
			combined ^= size_t(resourceKey.keyHash) + 0x9e3779b97f4a7c15ull + (combined << 6) + (combined >> 2);
			combined ^= (size_t(resourceKey.type) * 0x9e3779b97f4a7c15ull);
			return combined;
		}
	};

	struct TEX_CPU_MIP
	{
		_uint rowPitch = 0;
		_uint slicePitch = 0;
		vector<unsigned char> bytes;
	};

	struct TEX_CPU_DATA
	{
		_uint width = 0;
		_uint height = 0;
		_uint mipCount = 1;
		DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM; // ±âº»
		vector<TEX_CPU_MIP> mips;
		_bool isSRGB = false;
	};


	using ResourceVariant = variant<
		monostate,
		class CTexture*,
		class CModelData*,
		class CShader*,
		class CMaterialData*,
		class CAnimationClip*,
		class CSoundData*,
		TEX_CPU_DATA
	>;
}