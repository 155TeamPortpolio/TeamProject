#ifndef Engine_RenderStruct_h__
#define Engine_RenderStruct_h__

namespace Engine
{
	typedef struct tagFogDesc {
		_float4 fogColor = _float4(0.9f, 0.7f, 0.75f, 1.0f);
		_float	fogDensity = 0.002f;
	}FOG_DESC;

	typedef struct tagGlitchDesc {
		_float	glitchSpeed = 15.f;
		_float  glitchStrength = 0.04f;
	}GLITCH_DESC;
}

#endif