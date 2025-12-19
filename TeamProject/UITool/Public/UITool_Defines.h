#pragma once

#include <windows.h>
#include <d3d11.h>

namespace UITool {
	static constexpr unsigned int g_iWinSizeX = 1280;
	static constexpr unsigned int g_iWinSizeY = 720;
	static constexpr unsigned int g_iMainFrame = 144;
}

extern HWND g_hWnd;
extern HINSTANCE g_hInstance;

using namespace UITool;

#include "Engine_Defines.h"
#include "Engine_Math.h"

// 엔진에 넣어야
typedef struct tagButtonEvent {
	wstring msg = L"";
}BTN_EVENT;

typedef struct tagUIKeyframe {
	_float		fTime = {};
	_float2		vScale = { 1.f, 1.f };
	_float		fAngle = {};
	_float2		vPosition = {};
	_float4		vColor = { 1.f, 1.f, 1.f, 1.f };
	EaseType	easeType = {};

	tagUIKeyframe(_float _fTime = 0.f) : fTime(_fTime) {}
}UI_KEYFRAME;

typedef struct tagUIAnimationClip {
	string		strName;
	_bool		isLoop = {};
	_float		fDuration = { 1.f };

	vector<UI_KEYFRAME>	keyframes;

	tagUIAnimationClip(string _strName) : strName(_strName) {}
}UI_ANIM_CLIP;