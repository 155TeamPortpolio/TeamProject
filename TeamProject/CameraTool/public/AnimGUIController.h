#pragma once

NS_BEGIN(Engine)
class CAnimator3D;
NS_END

NS_BEGIN(CameraTool)

class CAnimGUIController
{
public:
    void DrawInline(OBJECT_HANDLE spaceRefHandle);

public:
    bool  HasAnimator(OBJECT_HANDLE spaceRefHandle) const;
    bool  IsPlaying(OBJECT_HANDLE spaceRefHandle) const;
    void  SetPlaying(OBJECT_HANDLE spaceRefHandle, bool play);
    void  SetTimeSec(OBJECT_HANDLE spaceRefHandle, float timeSec);
    float GetClipEndSec(OBJECT_HANDLE spaceRefHandle) const;

private:
    CAnimator3D* ResolveAnimator(OBJECT_HANDLE spaceRefHandle) const;
    void DrawScrubBar(CAnimator3D* anim, float width = 180.f);

private:
    CAnimator3D* m_scrubAnim = nullptr;
    bool         m_resumeAfterScrub = false;
};

NS_END