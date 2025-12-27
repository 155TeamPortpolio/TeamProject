#pragma once
#include "Engine_Defines.h"

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

	using ResourceVariant = std::variant<
		monostate,
		class CTexture*,
		class CModelData*,
		class CShader*,
		class CMaterialData*,
		class CAnimationClip*,
		class CVIBuffer*,
		class CSoundData*
	>;

	using JobFunc = function<ResourceVariant()>;
	using LoaderFunc = function<ResourceVariant(const string& sourcePath, string& outError)>;
	using CommitFunc = function<_bool(const ResourceVariant& stagingVariant, ResourceVariant& finalVariant, string& errorMsg)>;
	using ScheduleFunc = function<shared_future<ResourceVariant>(JobFunc)>;

}