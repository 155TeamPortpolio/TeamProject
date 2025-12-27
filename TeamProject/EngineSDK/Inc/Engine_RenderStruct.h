#ifndef Engine_RenderStruct_h__
#define Engine_RenderStruct_h__

namespace Engine
{
	typedef struct tagFogDesc {
		_float4 fogColor = _float4(0.9f, 0.7f, 0.75f, 1.0f);
		_float	fogStart = 0.f;
		_float	fogEnd = 0.f;
		_float	fogDensity = 0.002f;
		_bool	IsUse = false;
	}FOG_DESC;
}

#endif