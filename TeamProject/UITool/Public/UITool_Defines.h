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

typedef struct tagButtonEvent {
	wstring msg = L"";
}BTN_EVENT;

typedef struct tagUIScaleKeyframe {
	_float		fTrackPosition = {};
	_float2		value = { 1.f, 1.f };
	EaseType	easeType = {};
}UI_SCALE_KEYFRAME;

typedef struct tagUIRotationKeyframe {
	_float		fTrackPosition = {};
	_float		value = {};
	EaseType	easeType = {};
}UI_ROTATION_KEYFRAME;

typedef struct tagUIPositionKeyframe {
	_float		fTrackPosition = {};
	_float2		value = {};
	EaseType	easeType = {};
}UI_POS_KEYFRAME;

typedef struct tagUIColorKeyframe {
	_float		fTrackPosition = {};
	_float4		value = { 1.f, 1.f, 1.f, 1.f };
	EaseType	easeType = {};
}UI_COLOR_KEYFRAME;

typedef struct tagUIAnimationClip {
	string		strName;
	_bool		isLoop = {};
	_float		fDuration = { 1.f };

	vector<UI_SCALE_KEYFRAME>	m_ScaleKeyframes;
	vector<UI_ROTATION_KEYFRAME> m_RotationKeyframes;
	vector<UI_POS_KEYFRAME>	m_PosKeyframes;
	vector<UI_COLOR_KEYFRAME> m_ColorKeyframes;

	tagUIAnimationClip(string _strName) : strName(_strName) {}
}UI_ANIM_CLIP;