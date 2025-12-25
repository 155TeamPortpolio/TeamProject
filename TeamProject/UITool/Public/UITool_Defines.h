#pragma once

#include <windows.h>
#include <d3d11.h>

namespace UITool {
	static constexpr unsigned int g_iWinSizeX = 1920;
	static constexpr unsigned int g_iWinSizeY = 1080;
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

typedef struct tagUITransformData {
	_uint iAnchor = {};
	array<_float, 2> vAnchorOffset = {};
	array<_float, 2> vSize = {};
	array<_float, 2> vScale = {};
	array<_float, 2> vPivot = {};
	_float fRadian = {};
}UI_TRANSFORM_DATA;
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(UI_TRANSFORM_DATA, iAnchor, vAnchorOffset, vSize, vScale, vPivot, fRadian);

typedef struct tagUIKeyframeData {
    _float fTime = {};
    array<_float, 2> vScale = {};
    _float fAngle = {};
    array<_float, 2> vPosition = {};
    array<_float, 4> vColor = {};
    _uint uEaseType = {};
}UI_KEYFRAME_DATA;
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(UI_KEYFRAME_DATA, fTime, vScale, fAngle, vPosition, vColor, uEaseType);

typedef struct tagUIClipData {
    string strName = {};
    _bool isLoop = {};
    _float fDuration = {};

    vector<UI_KEYFRAME_DATA> keyframes;
}UI_CLIP_DATA;
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(UI_CLIP_DATA, strName, isLoop, fDuration, keyframes);

typedef struct tagUIElementData {
    string strTypeTag = {}; 
    UI_TRANSFORM_DATA transform = {};
    array<_float, 4> vColor = {};

    // 타입별 선택적 속성들
    string strTextureTag = {};

    // Text
    string strText = {};
    string strFontTag = {};
    _float fFontScale = {};
    _bool isOutlined = {};
    _float fOutlineThickness = {};
    array<_float, 4> vOutlineColor = {};

    // Button
    string strEventMsg = {};

    // SpriteAnimation
    _int iFrameCountTotal = {};
    _int iFrameCountX = {};
    _int iFrameCountY = {};
    _float fFrameSpeed = {};
    _bool isLoop = {};
    
    // UVAnimation
    array<_float, 2> vUVOffsetSpeed = {}; 

    // Gauge
    _float fDirection = {};
    _float fFillAmount = {};
    _bool isRadial = {};

    vector<UI_CLIP_DATA> animClips;
    vector<tagUIElementData> children;
}UI_ELEMENT_DATA;
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(UI_ELEMENT_DATA, strTypeTag, vColor, transform, strTextureTag, strText, strFontTag, fFontScale,
    strEventMsg, iFrameCountTotal, iFrameCountX, iFrameCountY, fFrameSpeed, isLoop, vUVOffsetSpeed, fDirection, fFillAmount, isRadial,
    animClips, children);