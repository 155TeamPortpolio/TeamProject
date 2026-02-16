#include "pch.h"
#include "BattleFXFlow.h"
#include "GameInstance.h"

#include "PostRenderer.h"
#include "PostProcessCommand.h"
#include "CamDirector.h"
#include "UIDirector.h"

#include "Layer.h"
CBattleFXFlow::CBattleFXFlow() {

}

void CBattleFXFlow::Initialize_Preset()
{
	m_BattleVFXData.resize(ENUM(BATTLE_VFX_TYPE::END), {});

	{
		auto& evade = m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::EVADE)];
		const _float duration = .8f;
		evade.bCanIntersect = true;
		evade.fVFXDuration = duration;
		evade.fBlurDuration = .8f;
		evade.vStartColor = { 1.f,1.f,1.f };
		evade.vTargetColor = { 0.1f,0.1f,0.1f };
		evade.SetTimeData({ duration, 0.4f, 0.4f, 0.4f , EaseType::OutCubic });
	}

	{
		auto& Parry = m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::PARRY)];
		const _float duration = 1.f;
		Parry.bCanIntersect = true;
		Parry.fVFXDuration = duration;
		Parry.fBlurDuration = .5f;
		Parry.SetTimeData({ duration, 0.2f, 0.35f, 0.45f, EaseType::OutExpo });
		Parry.BattleTimeScale[ENUM(BATTLE_OBJ_TYPE::MONSTER)] = 
			TIME_SCALING({ duration, 0.05f, 0.1f, .2f , EaseType::InOutSine });
	} 
	{
		auto& Parry = m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::SWITCH)];
		const _float duration = 2.f;
		Parry.bCanIntersect = false;
		Parry.fVFXDuration = duration;
		Parry.fBlurDuration = .5f;
		Parry.SetTimeData({ duration, 0.2f, 0.15f, 0.15f, EaseType::OutExpo });
		Parry.BattleTimeScale[ENUM(BATTLE_OBJ_TYPE::MONSTER)] = 
			TIME_SCALING({ duration, 0.2f, 0.15f, 0.15f , EaseType::OutExpo });
	} 
	{
		auto& Ultimate = m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::ULTIMATE)];
		const _float duration = 2.f;
		Ultimate.bCanIntersect = false;
		Ultimate.fVFXDuration = duration;
		Ultimate.fBlurDuration = duration;
		Ultimate.BattleTimeScale[ENUM(BATTLE_OBJ_TYPE::MONSTER)] = 
			TIME_SCALING({ duration, 0.f, 1.f, 0.f , EaseType::InOutSine });
	}

	{
		auto& NormalHitLack = m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::HIT_NORMAL)];
		const _float duration = .08f;
		NormalHitLack.bCanIntersect = true;
		NormalHitLack.fVFXDuration = duration;
		NormalHitLack.fBlurDuration = duration;
		NormalHitLack.SetTimeData({ duration, 0.1f, 0.5f, .5f , EaseType::InOutSine });
	}

	{
		auto& HardHitLack = m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::HIT_HARD)];
		const _float duration = .1f;
		HardHitLack.bCanIntersect = true;
		HardHitLack.fVFXDuration = duration;
		HardHitLack.fBlurDuration = duration;
		HardHitLack.SetTimeData({ duration, 0.1f, 0.25f, .25f , EaseType::OutExpo });
	}

	{
		auto& WipeOut = m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::WIPEOUT)];
		const _float duration = 4.8f;
		WipeOut.bCanIntersect = false;
		WipeOut.fVFXDuration = duration;
		WipeOut.fBlurDuration = duration;
		WipeOut.SetTimeData({ duration, 0.0f, 0.3f, .0f , EaseType::OutQuint });
		WipeOut.BattleTimeScale[ENUM(BATTLE_OBJ_TYPE::CAMERA)] = TIME_SCALE_DATA{duration, 1.0f, 0.3f, .0f, EaseType::OutQuint};
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

	dt = TimeManager()->Get_RawDeltaTime(G_EngineTimerID);

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

	// ???? ?????? ????
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

	for (auto& track : m_parallelTracks)
	{
		if (track.onEnd) track.onEnd();
	}

	m_steps.clear();
	m_parallelTracks.clear();
	m_stepIndex = 0;
	m_isRunning = false;
	m_timeInStep = 0.f;
	m_onEnd = {};
}

void CBattleFXFlow::Cancel()
{
	if (m_onCancel) m_onCancel();

	for (auto& track : m_parallelTracks)
	{
		if (track.onEnd) track.onEnd();
	}

	m_isRunning = false;
	m_stepIndex = 0;
	m_steps.clear();
	m_parallelTracks.clear();
	m_onEnd = {};
}


void CBattleFXFlow::StartVfx(BATTLE_VFX_TYPE vfxType)
{
	if (m_BattleVFX.isRunning && !m_BattleVFX.bCanIntersect)
		return;

	auto& preset = m_BattleVFXData[ENUM(vfxType)];

	m_BattleVFX.isRunning = true;
	m_BattleVFX.eVFXType = vfxType;
	m_BattleVFX.fDuration = preset.fVFXDuration;
	m_BattleVFX.fCurPos = 0.f;
	m_BattleVFX.vNowColor = preset.vStartColor;
	m_BattleVFX.bCanIntersect = preset.bCanIntersect;

	switch (vfxType)
	{
	case BATTLE_VFX_TYPE::EVADE:
		StartVfx_Evade();
		break;
	case BATTLE_VFX_TYPE::PARRY:
		StartVfx_Parry();
		break;
	case BATTLE_VFX_TYPE::SWITCH:
		StartVfx_Switch();
		break;
	
	case BATTLE_VFX_TYPE::ULTIMATE:
		StartVfx_Ultimate();
		break;
	case BATTLE_VFX_TYPE::HIT_NORMAL:
		NormalHitLack();
		break;
	case BATTLE_VFX_TYPE::HIT_HARD:
		HardHitLack();
		break;
	case BATTLE_VFX_TYPE::WIPEOUT:
		StartVfx_WipeOut();
		break;
	case BATTLE_VFX_TYPE::CLEAR:
		StartVfx_Switch();
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
	AddParallelTimeScaleAll(preset);

	CPostRenderer* pPost = RenderSystem()->GetPostRenderer();

	AddCall([this, preset,pPost]() {
		pPost->GetCommand<CSaturationCommand>()
			->SetIntensity(.4f)
			->SetSaturationType(ENUM(SATURATIONTYPE::FULL))
			->SetDuration(preset.fVFXDuration)
			->SetEaseType(EaseType::OutBack)
			->SetEnable(true);
		});

	AddCall([this, preset, pPost]() {
		pPost->GetCommand<CRadialBlurCommand>()
			->SetDuration(preset.fBlurDuration)
			->SetEaseType(EaseType::OutSine)
			->SetIntensity(0.1)
			->SetEnable(true);
		});

	AddStep(
		[this, preset, elapsed = 0.f, duration = m_BattleVFX.fDuration](_float dt) mutable -> _bool
		{
			elapsed += dt;
			_float time01 = 1.f;
			if (duration > 0.f)
				time01 = clamp(elapsed / duration, 0.f, 1.f);

			_float normalizedT = 1.f - time01;
			_float pingpongT = (normalizedT < 0.5f) ? (normalizedT * 2.f) : (2.f - normalizedT * 2.f);
			_float easeT = Math::ApplyEase(EaseType::OutCubic, pingpongT);
			_vector startColor = XMLoadFloat3(&preset.vStartColor);
			_vector targetColor = XMLoadFloat3(&preset.vTargetColor);
			XMStoreFloat3(&m_BattleVFX.vNowColor, XMVectorLerp(startColor, targetColor, easeT));

			m_BattleVFX.fCurPos = duration * time01;

			return elapsed < duration;
		}
	);

	AddCall([this, preset, pPost]() {
		pPost->GetCommand<CAddictiveColorCommand>()
			->SetEnable(false);
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
	AddParallelTimeScaleAll(preset);

	//AddCall([this, preset]() {CamDirector()->StartParry(); });

	CPostRenderer* pPost = RenderSystem()->GetPostRenderer();
	AddCall([this, preset, pPost]() {
		pPost->GetCommand<CRadialBlurCommand>()
			->SetDuration(preset.fBlurDuration)
			->SetEaseType(EaseType::OutExpo)
			->SetIntensity(0.15)
			->SetEnable(true);
		});
	AddWait(preset.fVFXDuration);
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
	AddParallelTimeScale(BATTLE_OBJ_TYPE::MONSTER, preset.BattleTimeScale[ENUM(BATTLE_OBJ_TYPE::MONSTER)]);
	AddCall([this]() {
		string nowLevel = LevelManager()->Get_NowLevelKey();
		if(auto layer = ObjectManager()->Get_Layer({nowLevel, m_layerTag[ENUM(BATTLE_OBJ_TYPE::MONSTER)]}))
			layer->Set_RenderState(false);
		});
	AddWait(preset.fVFXDuration);
	AddCall([this, preset]() {
		m_BattleVFX.fCurPos = 0.f;
		m_BattleVFX.vNowColor = {}; 
		m_BattleVFX.isRunning = false;
		string nowLevel = LevelManager()->Get_NowLevelKey();
		if(auto layer = ObjectManager()->Get_Layer({ nowLevel ,m_layerTag[ENUM(BATTLE_OBJ_TYPE::MONSTER)] }))
			layer->Set_RenderState(true);
		});
	Start(nullptr);
}

void CBattleFXFlow::StartVfx_Switch()
{
	Clear(false);

	auto& preset = m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::SWITCH)];
	AddParallelTimeScaleAll(preset);

	CPostRenderer* pPost = RenderSystem()->GetPostRenderer();

	AddCall([this, preset, pPost]() {
		pPost->GetCommand<CSaturationCommand>()
			->SetIntensity(1.f)
			->SetSaturationType(ENUM(SATURATIONTYPE::STATIC))
			->SetDuration(preset.fVFXDuration)
			->SetEaseType(EaseType::OutBack)
			->SetEnable(true);
		});

	AddCall([this, preset, pPost]() {
		pPost->GetCommand<CRadialBlurCommand>()
			->SetDuration(preset.fBlurDuration)
			->SetEaseType(EaseType::OutSine)
			->SetIntensity(0.1)
			->SetEnable(true);
		});
	AddCall([this]() {
		UIDirector()->Show_Switch();
		});
	AddWait(preset.fVFXDuration);
	AddCall([this, preset, pPost]() {
		m_BattleVFX.fCurPos = 0.f;
		m_BattleVFX.vNowColor = {};
		m_BattleVFX.isRunning = false;
		});

	Start(nullptr);
}

void CBattleFXFlow::StartVfx_WipeOut()
{
	Clear(false);

	auto& preset = m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::WIPEOUT)];
	AddParallelTimeScaleAll(preset);
	CPostRenderer* pPost = RenderSystem()->GetPostRenderer();
	AddCall([this, preset, pPost]() {
		pPost->GetCommand<CSaturationCommand>()
			->SetIntensity(1.4f)
			->SetSaturationType(ENUM(SATURATIONTYPE::SKINNED))
			->SetDuration(preset.fVFXDuration)
			->SetEaseType(EaseType::OutBack)
			->SetEnable(true);
		});

	AddCall([this]() {
		CamDirector()->BeginWipeOut(); 
		UIDirector()->Show_Wipeout();
		UIDirector()->Hide_HUD(CUIDirector::BATTLE);
		});
	AddWait(preset.fVFXDuration);
	AddCall([this, preset]() {
		m_BattleVFX.fCurPos = 0.f;
		m_BattleVFX.vNowColor = {};
		m_BattleVFX.isRunning = false;
		CamDirector()->EndWipeOut();
		});
	Start(nullptr);
}

void CBattleFXFlow::StartVfx_Clear()
{
	Clear(false);
	auto& preset = m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::WIPEOUT)];
	AddParallelTimeScaleAll(preset);
	CPostRenderer* pPost = RenderSystem()->GetPostRenderer();
	AddCall([this, preset, pPost]() {
		pPost->GetCommand<CSaturationCommand>()
			->SetIntensity(1.4f)
			->SetSaturationType(ENUM(SATURATIONTYPE::SKINNED))
			->SetDuration(preset.fVFXDuration)
			->SetEaseType(EaseType::OutBack)
			->SetEnable(true);
		});

	AddCall([this]() {
		CamDirector()->BeginWipeOut();
		UIDirector()->Show_Clear();
		UIDirector()->Hide_HUD(CUIDirector::BATTLE);
		});
	AddWait(preset.fVFXDuration);
	AddCall([this, preset]() {
		m_BattleVFX.fCurPos = 0.f;
		m_BattleVFX.vNowColor = {};
		m_BattleVFX.isRunning = false;
		CamDirector()->EndWipeOut();
		});
	Start(nullptr);
}

void CBattleFXFlow::NormalHitLack()
{
	Clear(false);

	auto& preset = m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::HIT_NORMAL)];
	AddParallelTimeScaleAll(preset);
	AddWait(preset.fVFXDuration);
	AddCall([this, preset]() {
		m_BattleVFX.fCurPos = 0.f;
		m_BattleVFX.vNowColor = {};
		m_BattleVFX.isRunning = false;
		});
	Start(nullptr);
}

void CBattleFXFlow::HardHitLack()
{
	Clear(false);

	auto& preset = m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::HIT_HARD)];
	CPostRenderer* pPost = RenderSystem()->GetPostRenderer();
	AddParallelTimeScaleAll(preset);
	AddCall([this, preset, pPost]() {
		pPost->GetCommand<CRadialBlurCommand>()
			->SetDuration(preset.fBlurDuration)
			->SetEaseType(EaseType::OutExpo)
			->SetIntensity(0.03)
			->SetEnable(true);
		});
	AddWait(preset.fVFXDuration);
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

	const TIME_SCALE_DATA data = timeScale.data;

	AddParallel(data.fDuration,
		[this, type, data](SubFlow& subFlow) mutable
		{
			_float elapsed = 0.f;

			subFlow.AddStep([this, type, data, elapsed](_float dt) mutable -> _bool
				{
					elapsed += dt;

					_float t01 = 1.f;
					if (data.fDuration > 0.f)
						t01 = clamp(elapsed / data.fDuration, 0.f, 1.f);

					// TIME_SCALING�� EvalScale01 ������ �״�� �����ϰ� ������
					TIME_SCALING tmp(data);
					const _float scale = tmp.EvalScale01(t01);

					SetLayerTimeScale(type, scale);
					return elapsed < data.fDuration;
				});
		},
		[this, type]()
		{
			ResetLayerTimeScale(type);
		}
	);
}

void CBattleFXFlow::AddParallelTimeScaleAll(BATTLE_VFX_DATA& vfxData)
{
	for (size_t typeIndex = 0; typeIndex < ENUM(BATTLE_OBJ_TYPE::END); ++typeIndex)
		AddParallelTimeScale(BATTLE_OBJ_TYPE(typeIndex), vfxData.BattleTimeScale[typeIndex]);
}


void CBattleFXFlow::SetLayerTimeScale(BATTLE_OBJ_TYPE type, _float scale)
{
	const string nowLevelKey = LevelManager()->Get_NowLevelKey();
	if (type == BATTLE_OBJ_TYPE::CAMERA)
		ObjectManager()->Set_LayerTimeScale({ G_GlobalLevelKey, m_layerTag[ENUM(type)] }, scale);
	else
		ObjectManager()->Set_LayerTimeScale({ nowLevelKey, m_layerTag[ENUM(type)] }, scale);

}

void CBattleFXFlow::ResetLayerTimeScale(BATTLE_OBJ_TYPE type)
{
	const string nowLevelKey = LevelManager()->Get_NowLevelKey();
	if (type == BATTLE_OBJ_TYPE::CAMERA)
		ObjectManager()->Reset_LayerTimeScale({ G_GlobalLevelKey, m_layerTag[ENUM(type)] });
	else
		ObjectManager()->Reset_LayerTimeScale({ nowLevelKey, m_layerTag[ENUM(type)] });

}

void CBattleFXFlow::AddStep(StepFunc step)
{
	m_steps.emplace_back(move(step));
}

void CBattleFXFlow::AddWait(_float duration)
{
	_float elapsed = 0.f;
	m_steps.emplace_back([=](_float dt) mutable -> bool {
		elapsed += dt;
		return elapsed < duration;
		});
}

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

void CBattleFXFlow::AddLerpFloatTo(_float* valuePtr, _float toValue, _float duration, EaseType ease)
{
	if (!valuePtr) return;
	AddLerpFloatFromTo(valuePtr, *valuePtr, toValue, duration, ease);
}

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

void CBattleFXFlow::AddLerpColor3To(_float3* colorPtr, const _float3& toValue, _float duration, EaseType ease)
{
	if (!colorPtr) return;
	AddLerpColor3FromTo(colorPtr, *colorPtr, toValue, duration, ease);
}

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
