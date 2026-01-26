#pragma once
#include "Base.h"
class CBattleFXFlow :
	public CBase
{
	/*연출 단계 콜백 ->true: 계속, false: 다음 step */
using StepFunc = function<bool(float)>; 
using VoidFunc = function<void()>; 

private:
	struct ParallelTrack /*병렬 진행 구조체*/
	{
		_float duration = 0.f;
		_float elapsed = 0.f;
		VoidFunc onEnd;
		vector<StepFunc> steps;
		size_t stepIndex = 0;
		_bool isRunning = false;
	};
private:
	CBattleFXFlow();
	~CBattleFXFlow()DEFAULT;

public:
	void Start(function<void()> onEnd = {});
	void Clear(_bool callOnEnd = true);
	void Update(_float dt);
	void Cancel();

public:
	_bool IsRunning() const { return m_isRunning; }

public:/*시퀀스 추가 함수*/
	void AddStep(CBattleFXFlow::StepFunc step);
	void AddWait(_float duration);
	void AddCall(function<void()> fn);
	void AddParallel(_float duration, function<void(CBattleFXFlow& subFlow)> build, VoidFunc onParallelEnd = {});

	// float
	void AddLerpFloatTo(_float* valuePtr, _float toValue, _float duration, EaseType ease = EaseType::Linear);
	void AddLerpFloatFromTo(_float* valuePtr, _float fromValue, _float toValue, _float duration, EaseType ease = EaseType::Linear);

	// color3 (_float3)
	void AddLerpColor3To(_float3* colorPtr, const _float3& toValue, _float duration, EaseType ease = EaseType::Linear);
	void AddLerpColor3FromTo(_float3* colorPtr, const _float3& fromValue, const _float3& toValue, _float duration, EaseType ease = EaseType::Linear);

	// PingPong: 0 -> peak -> 0
	void AddPingPongColor3(_float3* colorPtr, const _float3& peakValue, _float duration, EaseType ease = EaseType::InOutSine);

	void SetOnCancel(VoidFunc onCancel) { m_onCancel = std::move(onCancel); }
private:
	_bool m_isRunning = false;

	size_t m_stepIndex = 0;
	_float m_timeInStep = 0.f;

	VoidFunc m_onEnd;
	VoidFunc m_onCancel;
	vector<StepFunc> m_steps;
	vector<ParallelTrack> m_parallelTracks;

public:
	static CBattleFXFlow* Create();
	void Free();
};

