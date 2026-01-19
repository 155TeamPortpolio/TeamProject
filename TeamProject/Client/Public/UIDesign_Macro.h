#pragma once

namespace Client {

#define UI_WHITE			_float4(1.f, 1.f, 1.f, 1.f)
#define UI_TRANSPARENT		_float4()

#define UI_GRAY_DARKEST		_float4(0.0275f, 0.0275f, 0.0275f, 1.f) // #070707
#define UI_GRAY_DARK		_float4(0.1412f, 0.1412f, 0.1490f, 1.f) // #242426
#define UI_GRAY_MEDIUM		_float4(0.2078f, 0.2157f, 0.2157f, 1.f) // #353737
#define UI_GRAY_LIGHT		_float4(0.2588f, 0.2667f, 0.2824f, 1.f) // #424448
#define UI_GRAY_LIGHTEST	_float4(0.5882f, 0.5882f, 0.5882f, 1.f) // #969696

#define UI_SWITCH_YELLOW	_float4(0.9686f, 0.8157f, 0.0863f, 1.f) // #F7D016

#define UI_HPBACK_LIGHT		_float4(0.902f, 0.424f, 0.f, 1.f)		// #E66C00
#define UI_HPBACK_DARK		_float4(0.592f, 0.157f, 0.125f, 1.f)	// #972820
}