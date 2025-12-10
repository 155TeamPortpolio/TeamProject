#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
	enum class KEY_STATE { NONE_KEY, TAP, HOLD, AWAY };

	enum class MOUSE_BTN { LB, RB, MB };

	enum class WINMODE { FULL, WIN, END };

	enum class LEVEL_STATE { INITIAL, REQUEST, LOADING, LOADED, STABLE };

	enum class STATE { RIGHT, UP, LOOK, POSITION, END };

	enum class BUFFER_TYPE {BASIC_RECT, BASIC_CUBE, BASIC_SPHERE, BASIC_PLANE, TERRAIN, MESH};
	
	enum class MESH_TYPE { ANIM, NONANIM };
	
	enum class LIGHT_TYPE { DIRECTIONAL, POINT, SPOTLIGHT };


	enum class COLLIDER_TYPE { AABB, OBB, SPHERE };
	
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

	enum class NEIGHBOR_INDEX : unsigned int {
		UPLEFT = 1 << 0,
		UP = 1 << 1,
		UPRIGHT = 1 << 2,
		LEFT = 1 << 3,
		CENTER = 1 << 4,
		RIGHT = 1 << 5,
		DOWNLEFT = 1 << 6,
		DOWN = 1 << 7,
		DOWNRIGHT = 1 << 8,
		END = 0
	};

	inline NEIGHBOR_INDEX operator | (NEIGHBOR_INDEX a, NEIGHBOR_INDEX b) {
		return static_cast<NEIGHBOR_INDEX>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
	}
	inline NEIGHBOR_INDEX operator & (NEIGHBOR_INDEX a, NEIGHBOR_INDEX b) {
		return static_cast<NEIGHBOR_INDEX>(static_cast<unsigned int>(a) & static_cast<unsigned int>(b));
	}

	inline unsigned int operator & (unsigned int a, NEIGHBOR_INDEX b) {
		return static_cast<unsigned int>(a) & static_cast<unsigned int>(b);
	}

	inline unsigned int operator | (unsigned int a, NEIGHBOR_INDEX b) {
		return static_cast<unsigned int>(a) | static_cast<unsigned int>(b);
	}

	inline unsigned int Get_Index(NEIGHBOR_INDEX dir)
	{
		switch (dir)
		{
		case NEIGHBOR_INDEX::UPLEFT:   return 0;
		case NEIGHBOR_INDEX::UP:       return 1;
		case NEIGHBOR_INDEX::UPRIGHT:  return 2;
		case NEIGHBOR_INDEX::LEFT:     return 3;
		case NEIGHBOR_INDEX::CENTER:   return 4;
		case NEIGHBOR_INDEX::RIGHT:    return 5;
		case NEIGHBOR_INDEX::DOWNLEFT: return 6;
		case NEIGHBOR_INDEX::DOWN:     return 7;
		case NEIGHBOR_INDEX::DOWNRIGHT:return 8;
		default: return 4;
		}
	};
	inline NEIGHBOR_INDEX Get_Negihbor(unsigned int index) {
		switch (index)
		{
		case 0:return Engine::NEIGHBOR_INDEX::UPLEFT;
		case 1:return Engine::NEIGHBOR_INDEX::UP;
		case 2:return Engine::NEIGHBOR_INDEX::UPRIGHT;
		case 3:return Engine::NEIGHBOR_INDEX::LEFT;
		case 4:return Engine::NEIGHBOR_INDEX::CENTER;
		case 5:return Engine::NEIGHBOR_INDEX::RIGHT;
		case 6:return Engine::NEIGHBOR_INDEX::DOWNLEFT;
		case 7:return Engine::NEIGHBOR_INDEX::DOWN;
		case 8:return Engine::NEIGHBOR_INDEX::DOWNRIGHT;
		default:return Engine::NEIGHBOR_INDEX::END;
		}
	};
	inline NEIGHBOR_INDEX Rotate45_CCW(NEIGHBOR_INDEX dir, unsigned int count)
	{
		if (dir == NEIGHBOR_INDEX::CENTER)
			return dir;

		NEIGHBOR_INDEX curDir = dir;
		for (size_t i = 0; i < count; ++i)
		{
			switch (curDir)
			{
			case Engine::NEIGHBOR_INDEX::UPLEFT:
				curDir = NEIGHBOR_INDEX::LEFT;
				break;
			case Engine::NEIGHBOR_INDEX::UP:
				curDir = NEIGHBOR_INDEX::UPLEFT;
				break;
			case Engine::NEIGHBOR_INDEX::UPRIGHT:
				curDir = NEIGHBOR_INDEX::UP;
				break;
			case Engine::NEIGHBOR_INDEX::LEFT:
				curDir = NEIGHBOR_INDEX::DOWNLEFT;
				break;
			case Engine::NEIGHBOR_INDEX::CENTER:
				curDir = NEIGHBOR_INDEX::CENTER;
				break;
			case Engine::NEIGHBOR_INDEX::RIGHT:
				curDir = NEIGHBOR_INDEX::UPRIGHT;
				break;
			case Engine::NEIGHBOR_INDEX::DOWNLEFT:
				curDir = NEIGHBOR_INDEX::DOWN;
				break;
			case Engine::NEIGHBOR_INDEX::DOWN:
				curDir = NEIGHBOR_INDEX::DOWNRIGHT;
				break;
			case Engine::NEIGHBOR_INDEX::DOWNRIGHT:
				curDir = NEIGHBOR_INDEX::RIGHT;
				break;
			default:
				break;
			}
		}

		return curDir;
	};



	enum class TEXTURE_TYPE {
		NONE = 0,
		ALBEDO = 1,
		NORMAL = 2,
		OPACITY = 3,
		EMMISION = 4,

		ALBEDO_GRAY = 5,
		NORMAL_ORY = 6,
		ALBEDO_ORY = 7,
		EMMISION_ORY = 8,
		INDEXMAP = 9,
		SCALEX = 10,
		SCALEY = 11,
		SCALEXY = 12,

		GRADATION = 13,
		GRADATION_EDGE = 14,
		MIX = 15,
		SPECULAR = 16,
		ALPHA = 17,
		END = 18
	};

}
#endif // Engine_Enum_h__


