#include "pch.h"
#include "BattleFXFlow.h"
#include "GameInstance.h"

CBattleFXFlow::CBattleFXFlow() {

}

void CBattleFXFlow::Initialize_Preset()
{
	m_BattleVFXData.resize(ENUM(BATTLE_VFX_TYPE::END), {});

	{
		auto& evade = m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::EVADE)];
		const _float duration = 1.f;
		evade.fVFXDuration = duration;
		evade.fBlurDuration = duration;
		evade.vStartColor = { 1.f,1.f,1.f };
		evade.vTargetColor = { 0.6f,0.6f,0.6f };
		evade.tPlayerTimeScale = TIME_SCALING({ duration, 0.8f, 0.f, 0.f , EaseType::OutExpo });
		evade.tMonsterTimeScale = TIME_SCALING({ duration, 0.8f, 0.f, 0.f , EaseType::OutExpo });
	}

	{
		auto& Parry = m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::PARRY)];
		const _float duration = 1.f;
		Parry.fVFXDuration = duration;
		Parry.fBlurDuration = duration;
		Parry.tPlayerTimeScale = TIME_SCALING({ duration, 0.f, 0.2f, 1.f , EaseType::InExpo });
		Parry.tMonsterTimeScale = TIME_SCALING({ duration, 0.f, 0.2f, 1.f , EaseType::InExpo });
	}
	{
		auto& Ultimate = m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::ULTIMATE)];
		const _float duration = 2.f;
		Ultimate.fVFXDuration = duration;
		Ultimate.fBlurDuration = duration;
		Ultimate.tMonsterTimeScale = TIME_SCALING({ duration, 0.f, 0.f, 0.f , EaseType::InOutSine });
	}
	{
		auto& Switch = m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::SWITCH)];
		const _float duration = 2.f;
		Switch.fVFXDuration = duration;
		Switch.fBlurDuration = duration;
		Switch.tMonsterTimeScale = TIME_SCALING({ duration, 0.f, 0.f, 0.f , EaseType::InOutSine });
	}
}

void CBattleFXFlow::Start(function<void()> onEnd)
{
	m_onEnd = move(onEnd);
	m_stepIndex = 0;
	m_timeInStep = 0.f;
	m_isRunning = !m_steps.empty() || !m_parallelTracks.empty();

	for (auto& track : m_parallelTracks)
		track.isRunning = !track.steps.empty();
}

void CBattleFXFlow::Update(_float dt)
{
	if (!m_isRunning)
		return;

	for (_int trackIndex = 0; trackIndex < m_parallelTracks.size(); )
	{
		auto& track = m_parallelTracks[trackIndex];
		track.elapsed += dt;

		while (track.isRunning && track.stepIndex < track.steps.size())
		{
			bool keep = track.steps[track.stepIndex](dt);
			if (keep) break;

			track.stepIndex++;
			if (track.stepIndex >= track.steps.size())
				track.isRunning = false;
		}

		const _bool timeOver = (track.elapsed >= track.duration);

		if (timeOver)
		{
			if (track.onEnd) track.onEnd();
			m_parallelTracks.erase(m_parallelTracks.begin() + trackIndex);
			continue;
		}

		++trackIndex;
	}

	// 메인 시퀀스 진행
	while (m_stepIndex < m_steps.size())
	{
		_bool keep = m_steps[m_stepIndex](dt);
		if (keep) break;

		m_stepIndex++;
		if (m_stepIndex >= m_steps.size())
			break;
	}

	const _bool mainDone = (m_stepIndex >= m_steps.size());
	const _bool hasParallel = !m_parallelTracks.empty();

	if (mainDone && !hasParallel)
	{
		m_isRunning = false;
		if (m_onEnd) m_onEnd();
		m_onEnd = {};
	}
	else
	{
		m_isRunning = true;
	}
}

void CBattleFXFlow::Clear(_bool callOnEnd)
{
	if (callOnEnd && m_isRunning && m_onEnd)
		m_onEnd();

	m_steps.clear();
	m_parallelTracks.clear();
	m_stepIndex = 0;
	m_isRunning = false;
	m_timeInStep = 0.f;
	m_onEnd = {};
}

void CBattleFXFlow::Cancel()
{
	if (m_onCancel)
		m_onCancel();

	m_isRunning = false;
	m_stepIndex = 0;
	m_steps.clear();
	m_parallelTracks.clear();

	m_onEnd = {};
}


void CBattleFXFlow::StartVfx(BATTLE_VFX_TYPE vfxType)
{
	if (m_BattleVFX.isRunning)
		return;

	 auto& preset = m_BattleVFXData[ENUM(vfxType)];

	m_BattleVFX.isRunning = true;
	m_BattleVFX.eVFXType = vfxType;
	m_BattleVFX.fDuration = preset.fVFXDuration;
	m_BattleVFX.fCurPos = 0.f;
	m_BattleVFX.vNowColor = preset.vStartColor;

	// 타입별 빌드
	switch (vfxType)
	{
	case BATTLE_VFX_TYPE::EVADE:
		StartVfx_Evade();
		break;
	case BATTLE_VFX_TYPE::PARRY:
		StartVfx_Parry();
		break;
	case BATTLE_VFX_TYPE::ULTIMATE:
		StartVfx_Ultimate();
		break;
	case BATTLE_VFX_TYPE::SWITCH:
		StartVfx_Ultimate();
		break;
	default:
		m_BattleVFX.isRunning = false;
		break;
	}

}

void CBattleFXFlow::StartVfx_Evade()
{
	Clear(false);

	auto& preset = m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::EVADE)]; 
	AddParallelTimeScale(BATTLE_OBJ_TYPE::PLAYER, preset.tPlayerTimeScale);
	AddParallelTimeScale(BATTLE_OBJ_TYPE::MONSTER, preset.tMonsterTimeScale);

	AddCall([this]() {RenderSystem()->Register_AddictiveColor(&m_BattleVFX.vNowColor);});
	AddCall([this, preset]() {RenderSystem()->Apply_RadialBlur(preset.fBlurDuration);});
	AddStep(
		[this, preset,elapsed = 0.f, duration = m_BattleVFX.fDuration](_float dt) mutable -> _bool
		{
			elapsed += dt;
			_float time01 = 1.f;
			if (duration > 0.f)
				time01 = clamp(elapsed / duration, 0.f, 1.f);

			_float normalizedT = 1.f - time01;
			_float pingpongT = (normalizedT < 0.5f) ? (normalizedT * 2.f) : (2.f - normalizedT * 2.f);
			_float easeT = Math::ApplyEase(EaseType::OutSine, pingpongT);

			const _float3 target = { 0.1f, 0.3f, 0.3f };
			_vector startColor = XMLoadFloat3(&preset.vStartColor);
			_vector targetColor = XMLoadFloat3(&preset.vTargetColor);
			XMStoreFloat3(&m_BattleVFX.vNowColor, XMVectorLerp(startColor, targetColor, easeT));

			m_BattleVFX.fCurPos = duration * time01;

			return elapsed < duration;
		}
	);

	AddCall([this, preset]() {
		RenderSystem()->UnRegister_AddictiveColor();
		m_BattleVFX.fCurPos = 0.f;
		m_BattleVFX.vNowColor = {};
		m_BattleVFX.isRunning = false;
		});

	Start(nullptr);
}

void CBattleFXFlow::StartVfx_Parry()
{
	Clear(false);

	auto& preset = m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::PARRY)];
	AddParallelTimeScale(BATTLE_OBJ_TYPE::PLAYER, preset.tPlayerTimeScale);
	AddParallelTimeScale(BATTLE_OBJ_TYPE::MONSTER, preset.tMonsterTimeScale);

	AddCall([this, preset]() {RenderSystem()->Apply_RadialBlur(preset.fBlurDuration); });
	AddStep(
		[this, preset, elapsed = 0.f, duration = m_BattleVFX.fDuration]
		(_float dt) mutable -> _bool
		{
			elapsed += dt;
			_float time01 = 1.f;
			if (duration > 0.f)
				time01 = clamp(elapsed / duration, 0.f, 1.f);

			_float normalizedT = 1.f - time01;
			_float pingpongT = (normalizedT < 0.5f) ? (normalizedT * 2.f) : (2.f - normalizedT * 2.f);
			_float easeT = Math::ApplyEase(EaseType::InOutSine, pingpongT);

			const _float3 target = { 0.1f, 0.3f, 0.3f };
			_vector startColor = XMLoadFloat3(&preset.vStartColor);
			_vector targetColor = XMLoadFloat3(&preset.vTargetColor);
			XMStoreFloat3(&m_BattleVFX.vNowColor, XMVectorLerp(startColor, targetColor, easeT));

			m_BattleVFX.fCurPos = duration * time01;

			return elapsed < duration;
		}
	);

	AddCall([this, preset]() {
		m_BattleVFX.fCurPos = 0.f;
		m_BattleVFX.vNowColor = {};
		m_BattleVFX.isRunning = false;
		});

	Start(nullptr);
}

void CBattleFXFlow::StartVfx_Ultimate()
{
	Clear(false);

	auto& preset = m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::ULTIMATE)];
	AddParallelTimeScale(BATTLE_OBJ_TYPE::MONSTER, preset.tMonsterTimeScale);
	AddWait(preset.fVFXDuration);
	AddCall([this, preset]() {
		m_BattleVFX.fCurPos = 0.f;
		m_BattleVFX.vNowColor = {};
		m_BattleVFX.isRunning = false;
		});
	Start(nullptr);
}

void CBattleFXFlow::StartVfx_Switch()
{
	Clear(false);

	auto& preset = m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::PARRY)];
	AddParallelTimeScale(BATTLE_OBJ_TYPE::PLAYER, preset.tPlayerTimeScale);
	AddParallelTimeScale(BATTLE_OBJ_TYPE::MONSTER, preset.tMonsterTimeScale);

	AddCall([this, preset]() {RenderSystem()->Apply_RadialBlur(preset.fBlurDuration); });
	AddStep(
		[this, preset, elapsed = 0.f, duration = m_BattleVFX.fDuration]
		(_float dt) mutable -> _bool
		{
			elapsed += dt;
			_float time01 = 1.f;
			if (duration > 0.f)
				time01 = clamp(elapsed / duration, 0.f, 1.f);

			_float normalizedT = 1.f - time01;
			_float pingpongT = (normalizedT < 0.5f) ? (normalizedT * 2.f) : (2.f - normalizedT * 2.f);
			_float easeT = Math::ApplyEase(EaseType::InOutSine, pingpongT);

			const _float3 target = { 0.1f, 0.3f, 0.3f };
			_vector startColor = XMLoadFloat3(&preset.vStartColor);
			_vector targetColor = XMLoadFloat3(&preset.vTargetColor);
			XMStoreFloat3(&m_BattleVFX.vNowColor, XMVectorLerp(startColor, targetColor, easeT));

			m_BattleVFX.fCurPos = duration * time01;

			return elapsed < duration;
		}
	);

	AddCall([this, preset]() {
		m_BattleVFX.fCurPos = 0.f;
		m_BattleVFX.vNowColor = {};
		m_BattleVFX.isRunning = false;
		});

	Start(nullptr);
}

void CBattleFXFlow::AddParallelTimeScale(BATTLE_OBJ_TYPE type, TIME_SCALING& timeScale)
{
	if (!IsValidTimeScale(timeScale.data))
		return;

	const _float duration = timeScale.data.fDuration;

	AddParallel(timeScale.data.fDuration,
		[this, type, timeScale](SubFlow& subFlow) mutable
		{
			subFlow.AddStep([this, type, timeScale](_float dt) mutable -> _bool
				{
					timeScale.elapsed += dt;

					const _float scale = timeScale.EvalScale();
					SetLayerTimeScale(type, scale);

					return timeScale.elapsed < timeScale.data.fDuration;
				});
		},
		[this, type]() { ResetLayerTimeScale(type); }
	);
}

void CBattleFXFlow::SetLayerTimeScale(BATTLE_OBJ_TYPE type, _float scale)
{
	const string nowLevelKey = LevelManager()->Get_NowLevelKey();
	ObjectManager()->Set_LayerTimeScale({ nowLevelKey, m_layerTag[ENUM(type)] }, scale);
}

void CBattleFXFlow::ResetLayerTimeScale(BATTLE_OBJ_TYPE type)
{
	const string nowLevelKey = LevelManager()->Get_NowLevelKey();
	ObjectManager()->Reset_LayerTimeScale({ nowLevelKey, m_layerTag[ENUM(type)] });
}

void CBattleFXFlow::AddStep(StepFunc step)
{
	m_steps.emplace_back(move(step));
}

/*N초 기다림*/
void CBattleFXFlow::AddWait(_float duration)
{
	_float elapsed = 0.f;
	m_steps.emplace_back([=](_float dt) mutable -> bool {
		elapsed += dt;
		return elapsed < duration;
		});
}

/*1회 호출 후 넘어감*/
void CBattleFXFlow::AddCall(function<void()> fn)
{
	m_steps.emplace_back([fn = std::move(fn)](float) mutable -> bool {
		fn();
		return false;
		});
}

void CBattleFXFlow::AddParallel(_float duration, function<void(SubFlow& subFlow)> build, VoidFunc onParallelEnd)
{
	ParallelTrack track;
	track.duration = duration;
	track.elapsed = 0.f;
	track.onEnd = std::move(onParallelEnd);

	SubFlow subFlow;
	if (build)
		build(subFlow);

	track.steps = std::move(subFlow.m_steps);
	track.stepIndex = 0;
	track.isRunning = !track.steps.empty();

	m_parallelTracks.emplace_back(move(track));
}

/*소수 목표치 향해 보간*/
void CBattleFXFlow::AddLerpFloatFromTo(_float* valuePtr, _float fromValue, _float toValue, _float duration, EaseType ease)
{
	if (!valuePtr || duration <= 0.f)
		return;

	*valuePtr = fromValue;

	_float elapsed = 0.f;
	AddStep([valuePtr, fromValue, toValue, duration, ease, elapsed](_float dt) mutable -> bool
		{
			elapsed += dt;

			_float t = Math::Clamp01(elapsed / duration);
			t = Math::ApplyEase(ease, t);

			*valuePtr = Math::Lerp(fromValue, toValue, t);
			return elapsed < duration;
		});
}

/*위에거 래퍼 0 >목표만 정해줌*/
void CBattleFXFlow::AddLerpFloatTo(_float* valuePtr, _float toValue, _float duration, EaseType ease)
{
	if (!valuePtr) return;
	AddLerpFloatFromTo(valuePtr, *valuePtr, toValue, duration, ease);
}

/*컬러 보간*/
void CBattleFXFlow::AddLerpColor3FromTo(_float3* colorPtr, const _float3& fromValue, const _float3& toValue, _float duration, EaseType ease)
{
	if (!colorPtr || duration <= 0.f)
		return;

	*colorPtr = fromValue;

	_float elapsed = 0.f;
	AddStep([colorPtr, fromValue, toValue, duration, ease, elapsed](_float dt) mutable -> bool
		{
			elapsed += dt;

			_float t = Math::Clamp01(elapsed / duration);
			t = Math::ApplyEase(ease, t);

			_vector fromV = XMLoadFloat3(&fromValue);
			_vector toV = XMLoadFloat3(&toValue);
			XMStoreFloat3(colorPtr, XMVectorLerp(fromV, toV, t));

			return elapsed < duration;
		});
}

/*컬러 보간 래퍼*/
void CBattleFXFlow::AddLerpColor3To(_float3* colorPtr, const _float3& toValue, _float duration, EaseType ease)
{
	if (!colorPtr) return;
	AddLerpColor3FromTo(colorPtr, *colorPtr, toValue, duration, ease);
}

/*왔다갔다 컬러 */
void CBattleFXFlow::AddPingPongColor3(_float3* colorPtr, const _float3& peakValue,
	_float duration, EaseType ease)
{
	if (!colorPtr || duration <= 0.f)
		return;

	const _float3 zero = {};
	*colorPtr = zero;

	_float elapsed = 0.f;
	AddStep([colorPtr, zero, peakValue, duration, ease, elapsed](_float dt) mutable -> bool
		{
			elapsed += dt;

			_float t = Math::Clamp01(elapsed / duration);
			_float ping = Math::PingPong01(t);          // 0->1->0
			ping = Math::ApplyEase(ease, ping);

			_vector fromV = XMLoadFloat3(&zero);
			_vector toV = XMLoadFloat3(&peakValue);
			XMStoreFloat3(colorPtr, XMVectorLerp(fromV, toV, ping));

			return elapsed < duration;
		});
}


_bool CBattleFXFlow::IsValidTimeScale(const TIME_SCALE_DATA& timeScale)
{
	const _float eps = 1e-4f;
	if (timeScale.fDuration <= 0.f) return false;
	if (fabsf(timeScale.fValue - 1.f) <= eps) return false;
	if (timeScale.fValue < 0.f) return false;
	return true;
}

CBattleFXFlow* CBattleFXFlow::Create()
{
	CBattleFXFlow* instance = new CBattleFXFlow();
	return instance;
}

void CBattleFXFlow::Free()
{
	__super::Free();
}
