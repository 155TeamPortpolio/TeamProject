#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
	enum class KEY_STATE { NONE_KEY, TAP, HOLD, AWAY };
	enum class MOUSE_BTN { LB, RB, MB };
	enum class WINMODE { FULL, WIN, END };

	enum class LEVEL_STATE { INITIAL, REQUEST, LOADING, LOADED, STABLE };

	enum class STATE { RIGHT, UP, LOOK, POSITION, END };
	enum class BUFFER_TYPE { BASIC_RECT, BASIC_CUBE, BASIC_SPHERE, BASIC_PLANE, TERRAIN, MESH, BASIC_POINT };
	enum class MESH_TYPE { ANIM, NONANIM };
	enum class LIGHT_TYPE { DIRECTIONAL, POINT, SPOTLIGHT };

	enum class COLLIDER_TYPE { BOX, SPHERE, CAPSULE, END };
	enum class SOUND_GROUP { BGM, SFX, UI, TALK, ENV,END };
	enum class RENDER_PASS_TYPE { PRIORITY, RENDER_OPAQUE, NONLIGHT_OPAQUE};
	enum class RENDER_LAYER { Default, CustomOnly, Both };

	enum class ANCHOR : unsigned int {
		Center = 0,
		Left = 1,				// 0001
		Right = 2,			// 0010
		Top = 4,			// 0100
		Bottom = 8		// 1000
	};

	inline ANCHOR operator | (ANCHOR a, ANCHOR b) {
		return static_cast<ANCHOR>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
	}
	inline ANCHOR operator & (ANCHOR a, ANCHOR b) {
		return static_cast<ANCHOR>(static_cast<unsigned int>(a) & static_cast<unsigned int>(b));
	}

	enum class TEXTURE_TYPE {
        NONE = 0,
        DIFFUSE = 1,
        SPECULAR = 2,
        AMBIENT = 3,
        EMISSIVE = 4,
        HEIGHT = 5,
        NORMALS = 6,
        SHININESS = 7,
        OPACITY = 8,
        DISPLACEMENT = 9,
        LIGHTMAP = 10,
        REFLECTION = 11,
        BASE_COLOR = 12,
        NORMAL_CAMERA = 13,
        EMISSION_COLOR = 14,
        METALNESS = 15,
        DIFFUSE_ROUGHNESS = 16,
        AMBIENT_OCCLUSION = 17,
        UNKNOWN = 18,
        SHEEN = 19,
        CLEARCOAT = 20,
        TRANSMISSION = 21,
        MAYA_BASE = 22,
        MAYA_SPECULAR = 23,
        MAYA_SPECULAR_COLOR = 24,
        MAYA_SPECULAR_ROUGHNESS = 25,
        ANISOTROPY = 26,
        GLTF_METALLIC_ROUGHNESS = 27,
		END = 28
	};

	enum class EFFECT_TYPE
	{
		SPRITE,
		PARTICLE,
		MESH,
		END
	};

}
#endif // Engine_Enum_h__


