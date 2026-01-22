#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
	enum class KEY_STATE { NONE_KEY, TAP, HOLD, AWAY };
	enum class MOUSE_BTN { LB, RB, MB };
	enum class WINMODE { FULL, WIN, END };

	enum class LEVEL_STATE { INITIAL, REQUEST, LOADING, LOADED, STABLE };

	enum class STATE { RIGHT, UP, LOOK, POSITION, END };
	enum class BUFFER_TYPE { BASIC_RECT, BASIC_CUBE, BASIC_SPHERE, BASIC_PLANE, TERRAIN, MESH, BASIC_POINT, BASIC_INSTANCE_POINT };
	enum class MESH_TYPE { ANIM, NONANIM };
	enum class LIGHT_TYPE { DIRECTIONAL, POINT, SPOTLIGHT };

	enum class COLLIDER_TYPE { BOX, SPHERE, CAPSULE, END };
	// 0 ~ 31¹üÀ§
	enum class COLLISION_GROUP { 
		COMMON = 1<<0,
		PLAYER = 1<<1,
		MONSTER = 1<<2,
		PLAYER_ATTACK = 1<<3,
		MONSTER_ATTACK = 1<<4,
		MONSTER_PARRY = 1<<5,
		CAMERA = 1<<6,
		INTERACTABLE = 1<<7,
		END = 1<<31 
	};
	enum class SOUND_GROUP { BGM, SFX, UI, TALK, ENV,END };
	enum class RENDER_PASS_TYPE { PRIORITY, RENDER_OPAQUE, NONLIGHT_OPAQUE, RENDER_EFFECT, RENDER_3DUI  };
	enum class RENDERER_TYPE {FORWARD, POST, EFFECT, UI};
	enum class RENDER_LAYER { Default, CustomOnly, Both, None };
	enum class POSTPROCESS { MRT_Bloom, MRT_Distortion, MRT_RimLight, END};
	enum class RIMLIGHT {OUTLINE, BACKLIGHT, RIMLIGHT, END};
	enum class CUSTOMTARGET { EFFECT, UI, ETC, END };
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

	enum class AXIS : unsigned int {
		NONE = 0,
		X = 1,
		Y = 2,
		Z = 4,
	};

	inline AXIS operator | (AXIS a, AXIS b) {
		return static_cast<AXIS>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
	}
	inline AXIS operator & (AXIS a, AXIS b) {
		return static_cast<AXIS>(static_cast<unsigned int>(a) & static_cast<unsigned int>(b));
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
		NOISE = 28,
		DISSOLVE = 29,
		ALPHA_MASK = 30,
		DISTORTION = 31,
		END = 32
	};

	/* Effect */
	enum class EFFECT_TYPE
	{
		SPRITE,
		PARTICLE,
		MESH,
		TRAIL,
		END
	};

	enum class IK_TYPE
	{
		TWO_BONE,
		FOOT_IK,
	};

	enum class EaseType
	{
		None, Linear, InOutSine, OutCubic, InOutCubic, OutSine, InOutQuad, InSine, InCubic, InQuad, InCirc, InOutCirc, OutCirc, OutQuad,
		InQuart, InQuint, InOutQuart, OutQuart, InOutQuint, OutQuint, InOutExpo, OutExpo, InExpo, OutBack, InOutBack, InBack,
		OutElastic, InOutElastic, InElastic, OutBounce, InOutBounce, InBounce,
	};

	enum class AlphaCheckLevel { None = 0, Fast = 1, Hint = 2, Precise = 3 };

}
#endif // Engine_Enum_h__


