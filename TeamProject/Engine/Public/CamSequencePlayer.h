#pragma once

#include "Component.h"
#include "ICamEvaluator.h"
#include "CamEvaluator.h"
#include "CamSequencePlayerData.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCamSequencePlayer final : public CComponent
{
private:
	CCamSequencePlayer() DEFAULT;
	CCamSequencePlayer(const CCamSequencePlayer& rhs) : CComponent(rhs) {}
	~CCamSequencePlayer() DEFAULT;

public:
	HRESULT Initialize_Prototype()           override;
	HRESULT Initialize(COMPONENT_DESC* pArg) override;

public:
	void   SetSequence(const CamSequenceDesc* seq);
	const  CamSequenceDesc* GetSequence() const { return target.seq; }

	void   Invalidate() { eval.dirty = true; }

	void   Play();
	void   Pause() { playback.playing = false; }
	void   Stop(_bool resetTime = true);

	bool   IsPlaying() const { return playback.playing; }

	void   SetTime(_float t);
	_float GetTime() const { return playback.playTime; }

	void   SetTimeScale(_float scale) { playback.timeScale = scale; }
	_float GetTimeScale() const { return playback.timeScale; }

	void   SetApplyEnabled(_bool enabled);
	_bool  IsApplyEnabled() const { return apply.applyEnabled; }

	void   SetSpaceReference(OBJECT_HANDLE handle) { apply.spaceRefHandle = handle; }
	void   ClearSpaceReference() { apply.spaceRefHandle.Reset(); }

	void   Update(_float dt);

private:
	void   RebuildIfNeeded();
	void   ApplyPose(const CamPose& pose);

private:
	CamPlayerTarget        target{};
	CamPlayerPlaybackState playback{};
	CamPlayerApplyState    apply{};
	CamPlayerEvalState     eval{};

public:
	static CCamSequencePlayer* Create();
	virtual void Free() override;
	CComponent* Clone() override { return new CCamSequencePlayer(*this); }
};

NS_END