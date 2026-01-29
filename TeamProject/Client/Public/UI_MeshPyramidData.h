#pragma once

#include "GameObject.h"

NS_BEGIN(Client)

struct UI_MeshPyramidConfig
{
    Vector4 baseColorAlpha = {0.25f, 0.25f, 0.25f, 1.f};

    float fadeInDur = 0.5f;
    float fadeOutDur = 0.5f;

    float onScreenMarginPx = 20.f;

    float blinkSec = 0.15f;

    Vector3 gray = {0.2f, 0.2f, 0.2f};
    Vector3 red = {1.00f, 0.05f, 0.05f};

    float posSmoothSpeed = 22.f;
    float yawSmoothSpeed = 26.f;

    float ringRadius = 1.0f;
    float yOffset = 0.03f;

    float basePitchRad = XM_PI * 0.5f;

    float minDirLen = 0.02f;

    float maxFootStepPerFrame = 2.0f;
    float maxTargetStepPerFrame = 2.0f;

    float maxPosStepPerFrame = 0.35f;
};

struct UI_MeshPyramidRuntime
{
    float fadeT = 0.f;
    float alertBlinkT = 0.f;
    bool  isAlert = false;

    Vector2 lastFootXZ{};
    bool    hasLastFootXZ = false;

    Vector2 lastTargetXZ{};
    bool    hasLastTargetXZ = false;

    Vector3 smoothPos{};
    bool    hasSmoothPos = false;

    float lastYawRad = 0.f;
    bool  hasLastYaw = false;

    Vector3 lastDirXZ = {0.f, 0.f, 1.f};
    bool    hasLastDir = false;
};

NS_END