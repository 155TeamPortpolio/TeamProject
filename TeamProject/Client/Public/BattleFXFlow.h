#pragma once
#include "Base.h"
#include "Engine_Math.h"

NS_BEGIN(Client)
class CBattleFXFlow :
	public CBase
{
	/*연출 단계 콜백 ->true: 계속, false: 다음 step */
using StepFunc = function<bool(float)>; 
using VoidFunc = function<void()>; 

class SubFlow
{
public:
	void AddStep(StepFunc step) { m_steps.emplace_back(move(step)); }
	void AddWait(_float duration)
	{
		_float elapsed = 0.f;
		m_steps.emplace_back([=](_float dt) mutable -> _bool
			{
				elapsed += dt;
				return elapsed < duration;
			});
	}
	void AddCall(function<void()> fn)
	{
		m_steps.emplace_back([fn = move(fn)](_float) mutable -> _bool {
			fn();
			return false;
			});
	}

private:
	friend class CBattleFXFlow;
	vector<StepFunc> m_steps;
};

public:
	struct ParallelTrack
	{
		_float duration = 0.f;
		_float elapsed = 0.f;

		vector<StepFunc> steps;
		size_t stepIndex = 0;
		_bool isRunning = false;
		VoidFunc onEnd = {};
	};
	typedef struct tagTimeScaleData
	{
		_float	fDuration = {};
		_float	fValue = { 1.f };			// dt에 스케일 할 값 (dt * fValue)
		_float	fStartLerpTime = { 0.f };	// 스케일 시작 보간 비율 (0~1)
		_float	fEndLerpTime = { 0.2f };	// 스케일 종료 보간 비율 (0~1)
		EaseType eEaseType = EaseType::None;
	}TIME_SCALE_DATA; /* fDuration, fValue, fStartLerpTime */
	typedef struct tagTimeScale
	{
		_bool isRunning = false;
		TIME_SCALE_DATA data = {};
		_float elapsed = 0.f;
		tagTimeScale() {};
		tagTimeScale(const TIME_SCALE_DATA& src) :data(src) {};
		void Start(const TIME_SCALE_DATA& src){
			isRunning = true;
			data = src;
			elapsed = 0.f;
		}

		void Stop()
		{
			isRunning = false;
			elapsed = 0.f;
		}

		_float EvalScale01(_float t01) const
		{
			const _float start = clamp(data.fStartLerpTime, 0.f, 1.f);
			const _float end = clamp(data.fEndLerpTime, 0.f, 1.f);
			const _float target = data.fValue;

			const EaseType ease = data.eEaseType;

			if (start > 0.f && t01 < start)
			{
				_float a = clamp(t01 / start, 0.f, 1.f);
				a = Math::ApplyEase(ease, a);
				return Math::Lerp(1.f, target, a);
			}

			if (end > 0.f && t01 > (1.f - end))
			{
				_float a = clamp((t01 - (1.f - end)) / end, 0.f, 1.f);
				a = Math::ApplyEase(ease, a);
				return Math::Lerp(target, 1.f, a);
			}

			return target;
		}
		_float EvalScale() const
		{
			_float t01 = 1.f;
			if (data.fDuration > 0.f)
				t01 = clamp(elapsed / data.fDuration, 0.f, 1.f);
			return EvalScale01(t01);
		}
	}TIME_SCALING;
	typedef struct tagBattleVFX
	{
		BATTLE_VFX_TYPE eVFXType = BATTLE_VFX_TYPE::END;
		_bool	isRunning = {};
		_float	fDuration = {};
		_float	fCurPos = {};
		_float3 vNowColor = {};
		_float	GetTimeRatio()
		{
			if (0 > fCurPos)
				return 0.f;		// 0 나누기 방지
			return clamp(fCurPos / fDuration, 0.f, 1.f);
		}
	}BATTLE_VFX;
	typedef struct tagBattleVFXData
	{
		_float			fVFXDuration = {};			/* 화면 색 먹이는 시간 */
		_float			fBlurDuration = {};			/* 블러 먹이는 시간 */
		_float3 vStartColor = {};
		_float3 vTargetColor = {};
		TIME_SCALING tPlayerTimeScale = {};		/* Duration, ScaleValue(0 < value < 1), StartLerpTime, EndLerpTime */
		TIME_SCALING tMonsterTimeScale = {};		/* Duration, ScaleValue(0 < value < 1), StartLerpTime, EndLerpTime */
		TIME_SCALING tEffectTimeScale = {};		/* Duration, ScaleValue(0 < value < 1), StartLerpTime, EndLerpTime */
	}BATTLE_VFX_DATA;


private:
	CBattleFXFlow();
	~CBattleFXFlow()DEFAULT;

public:
	void Initialize_Preset();

public:
	void Start(function<void()> onEnd = {});
	void Clear(_bool callOnEnd = true);
	void Update(_float dt);
	void Cancel();

public:
	_bool IsRunning() const { return m_BattleVFX.isRunning; }
	const _float3& GetAddictiveColor() const { return m_BattleVFX.vNowColor; }
	void SetLayerTag(BATTLE_OBJ_TYPE type, const string& tag){m_layerTag[ENUM(type)] = tag;}
	void StartVfx(BATTLE_VFX_TYPE vfxType);

public:
	void StartVfx_Evade();
	void StartVfx_Parry();
	void StartVfx_Ultimate();
	void StartVfx_Switch();
	void SetLayerTimeScale(BATTLE_OBJ_TYPE type, _float scale);
	void ResetLayerTimeScale(BATTLE_OBJ_TYPE type);

private:/*시퀀스 추가 함수*/
	void AddStep(StepFunc step);
	void AddWait(_float duration);
	void AddCall(function<void()> fn);
	void AddParallel(_float duration,function<void(SubFlow& subFlow)> build,VoidFunc onParallelEnd = {});
	// float
	void AddLerpFloatTo(_float* valuePtr, _float toValue, _float duration, EaseType ease = EaseType::Linear);
	void AddLerpFloatFromTo(_float* valuePtr, _float fromValue, _float toValue, _float duration, EaseType ease = EaseType::Linear);

	// color3 
	void AddLerpColor3To(_float3* colorPtr, const _float3& toValue, _float duration, EaseType ease = EaseType::Linear);
	void AddLerpColor3FromTo(_float3* colorPtr, const _float3& fromValue, const _float3& toValue, _float duration, EaseType ease = EaseType::Linear);

	// PingPong: 0 -> peak -> 0
	void AddPingPongColor3(_float3* colorPtr, const _float3& peakValue, _float duration, EaseType ease = EaseType::InOutSine);
	void SetOnCancel(VoidFunc onCancel) { m_onCancel = move(onCancel); }
	void AddParallelTimeScale(BATTLE_OBJ_TYPE type, TIME_SCALING& timeScale);

private:
	const BATTLE_VFX_DATA& GetPreset(BATTLE_VFX_TYPE type) const {return m_BattleVFXData[ENUM(type)];};
	_bool IsValidTimeScale(const TIME_SCALE_DATA& timeScale);
	
private:
	_bool m_isRunning = false;

	size_t m_stepIndex = 0;
	_float m_timeInStep = 0.f;

	VoidFunc m_onEnd;
	VoidFunc m_onCancel;
	vector<StepFunc> m_steps;
	vector<ParallelTrack>	m_parallelTracks;
	BATTLE_VFX					m_BattleVFX;

	vector<BATTLE_VFX_DATA>		m_BattleVFXData;
	array<string, ENUM(BATTLE_OBJ_TYPE::END)> m_layerTag = {};
	array<TIME_SCALING, ENUM(BATTLE_OBJ_TYPE::END)> m_LayerTimeScale = {};

public:
	static CBattleFXFlow* Create();
	void Free();

};

NS_END