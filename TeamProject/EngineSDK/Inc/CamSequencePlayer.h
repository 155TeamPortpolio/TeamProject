#pragma once

#include "Component.h"
#include "ICamEvaluator.h"
#include "CamEvaluator.h"
#include "Transform.h"
#include "Camera.h"

NS_BEGIN(Engine)
class CCamPosPerSegmentEvaluator; class CCamRotPerSegmentEvaluator; class CCamFovPerSegmentEvaluator;

class ENGINE_DLL CCamSequencePlayer final : public CComponent
{
	struct CamPlayerTarget
	{
		const CamSequenceDesc* seq{};
	};
	struct CamPlayerPlaybackState
	{
		_bool  playing = false;
		_float playTime = 0.f;
		_float timeScale = 1.f;
	};
	struct CamPlayerApplyState
	{
		_bool         applyEnabled = true;
		CTransform*   transform{};
		CCamera*      cam{};
		OBJECT_HANDLE spaceRefHandle{};
	};
	struct CamPlayerEvalState
	{
		_bool          dirty = true;
		CCamEvaluator* evaluator{};

		CCamPosPerSegmentEvaluator* pos{};
		CCamRotPerSegmentEvaluator* rot{};
		CCamFovPerSegmentEvaluator* fov{};
	};

private:
	CamPlayerTarget        target{};
	CamPlayerPlaybackState playback{};
	CamPlayerApplyState    apply{};
	CamPlayerEvalState     eval{};

private:
	CCamSequencePlayer() DEFAULT;
	CCamSequencePlayer(const CCamSequencePlayer& rhs) : CComponent(rhs) {}
	~CCamSequencePlayer() DEFAULT;

public:
	HRESULT Initialize_Prototype()           override { return S_OK; }
	HRESULT Initialize(COMPONENT_DESC* pArg) override;

public:
	void   SetSequence(const CamSequenceDesc* seq);
	const  CamSequenceDesc* GetSequence() const { return target.seq; }

	void   Invalidate() { eval.dirty = true; }

	void   SyncSpaceRefAnimatorTime(_float sampleTime);

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
	_float GetPlaybackDuration() const;
	void   ApplyAtSampleTime(_float sampleTime);

public:
	static CCamSequencePlayer* Create();
	virtual void Free() override { Safe_Release(eval.evaluator); __super::Free();}
	CComponent* Clone() override { return new CCamSequencePlayer(*this); }
};

NS_END