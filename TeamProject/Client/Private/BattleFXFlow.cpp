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
		Parry.BattleTimeScale[ENUM(BATTLE_OBJ_TYPE::CAMERA)] = TIME_SCALE_DATA{ duration, 1.0f, 0.3f, .0f, EaseType::OutQuint };
	}

	{
		auto& Switch = m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::SWITCH)];
		const _float duration = 2.f;
		Switch.bCanIntersect = false;
		Switch.fVFXDuration = duration;
		Switch.fBlurDuration = .5f;
		Switch.SetTimeData({ duration, 0.2f, 0.15f, 0.15f, EaseType::OutExpo });
		Switch.BattleTimeScale[ENUM(BATTLE_OBJ_TYPE::MONSTER)] = 
			TIME_SCALING({ duration, 0.2f, 0.15f, 0.15f , EaseType::OutExpo });
		Switch.BattleTimeScale[ENUM(BATTLE_OBJ_TYPE::CAMERA)] = TIME_SCALE_DATA{ duration, 1.0f, 0.3f, .0f, EaseType::OutQuint };
	}
	{
		auto& Ultimate = m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::ULTIMATE)];
		const _float duration = 2.f;
		Ultimate.bCanIntersect = false;
		Ultimate.fVFXDuration = duration;
		Ultimate.fBlurDuration = duration;
		Ultimate.BattleTimeScale[ENUM(BATTLE_OBJ_TYPE::MONSTER)] = 
			TIME_SCALING({ duration, 0.f, 1.f, 0.f , EaseType::InOutSine });
		Ultimate.BattleTimeScale[ENUM(BATTLE_OBJ_TYPE::CAMERA)] = TIME_SCALE_DATA{ duration, 1.0f, 0.3f, .0f, EaseType::OutQuint };
	}

	{
		auto& NormalHitLack = m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::HIT_NORMAL)];
		const _float duration = .08f;
		NormalHitLack.bCanIntersect = true;
		NormalHitLack.fVFXDuration = duration;
		NormalHitLack.fBlurDuration = duration;
		NormalHitLack.SetTimeData({ duration, 0.1f, 0.5f, .5f , EaseType::InOutSine });
		NormalHitLack.BattleTimeScale[ENUM(BATTLE_OBJ_TYPE::CAMERA)] = TIME_SCALE_DATA{ duration, 1.0f, 0.3f, .0f, EaseType::OutQuint };
	}

	{
		auto& HardHitLack = m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::HIT_HARD)];
		const _float duration = .1f;
		HardHitLack.bCanIntersect = true;
		HardHitLack.fVFXDuration = duration;
		HardHitLack.fBlurDuration = duration;
		HardHitLack.SetTimeData({ duration, 0.1f, 0.25f, .25f , EaseType::OutExpo });
		HardHitLack.BattleTimeScale[ENUM(BATTLE_OBJ_TYPE::CAMERA)] = TIME_SCALE_DATA{ duration, 1.0f, 0.3f, .0f, EaseType::OutQuint };
	}

	{
		auto& WipeOut = m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::WIPEOUT)];
		const _float duration = 4.7f;
		WipeOut.bCanIntersect = false;
		WipeOut.fVFXDuration = duration;
		WipeOut.fBlurDuration = duration;
		WipeOut.SetTimeData({ duration, 0.0f, 0.3f, .02f , EaseType::OutQuint });
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
	CPostRenderer* postRenderer = RenderSystem()->GetPostRenderer();
	auto noiseTexture = ResourceManager()->Load_Texture(G_GlobalLevelKey, "Eff_Noise_194.png");

	const _float totalDuration = max(preset.fVFXDuration, 0.01f);
	const _float blurDuration = min(preset.fBlurDuration, totalDuration);

	auto RequestGlitch = [postRenderer, noiseTexture](_float glitchIntensity, _float glitchDuration)
		{
			if (!postRenderer) return;

			postRenderer->GetCommand<CGlitchCommand>()
				->SetDuration(glitchDuration)
				->SetIntensity(glitchIntensity)
				->SetNoiseTexture(noiseTexture)
				->SetEaseType(EaseType::OutQuart)
				->SetEnable(true);
		};

	AddParallelTimeScaleAll(preset);

	AddCall([postRenderer, totalDuration]()
		{
			if (!postRenderer) return;

			postRenderer->GetCommand<CSaturationCommand>()
				->SetIntensity(1.f)
				->SetSaturationType(ENUM(SATURATIONTYPE::STATIC))
				->SetDuration(totalDuration)
				->SetEaseType(EaseType::OutBack)
				->SetEnable(true);
		});

	AddCall([postRenderer, blurDuration]()
		{
			if (!postRenderer) return;

			postRenderer->GetCommand<CRadialBlurCommand>()
				->SetDuration(blurDuration)
				->SetEaseType(EaseType::OutSine)
				->SetIntensity(0.1f)
				->SetEnable(true);
		});

	AddCall([this]()
		{
			UIDirector()->Show_Switch();
		});

	struct GlitchKey
	{
		_float time01;
		_float intensity;
		_float dur01;
	};

	const GlitchKey glitchKeys[] =
	{
		{ 0.15f, 1.2f, 0.04f },
		{ 0.30f, 0.3f, 0.04f },
		{ 0.45f, 0.7f, 0.04f },
	};

	_float accumulatedTime = 0.f;
	const _uint glitchKeyCount = (_uint)(sizeof(glitchKeys) / sizeof(glitchKeys[0]));

	for (_uint keyIndex = 0; keyIndex < glitchKeyCount; ++keyIndex)
	{
		const _float targetTime = clamp(glitchKeys[keyIndex].time01, 0.f, 1.f) * totalDuration;
		const _float waitTime = max(0.f, targetTime - accumulatedTime);

		AddWait(waitTime);
		accumulatedTime += waitTime;

		const _float glitchIntensity = glitchKeys[keyIndex].intensity;
		const _float glitchDuration = max(0.01f, clamp(glitchKeys[keyIndex].dur01, 0.f, 1.f) * totalDuration);

		AddCall([RequestGlitch, glitchIntensity, glitchDuration]() mutable
			{
				RequestGlitch(glitchIntensity, glitchDuration);
			});
	}

	if (accumulatedTime < totalDuration)
		AddWait(totalDuration - accumulatedTime);

	AddCall([this]()
		{
			m_BattleVFX.fCurPos = 0.f;
			m_BattleVFX.vNowColor = {};
			m_BattleVFX.isRunning = false;
		});

	Start(nullptr);
}

void CBattleFXFlow::StartVfx_WipeOut()
{
	Clear(false);
	ObjectManager()->Get_Layer({ "Test_Level","PlacedObject_Layer" })->Set_RenderState(false);

	auto& preset = m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::WIPEOUT)];
	CPostRenderer* postRenderer = RenderSystem()->GetPostRenderer();

	const _float totalDuration = max(preset.fVFXDuration, 0.01f);

	auto defaultNoiseTexture = ResourceManager()->Load_Texture(G_GlobalLevelKey, "Eff_Noise_052.png");
	using NoiseTextureType = decltype(defaultNoiseTexture);

	// -------------------------
	// Glitch 호출 람다 (기존 그대로)
	// -------------------------
	auto RequestGlitch = [postRenderer](_float glitchIntensity, _float glitchDuration, NoiseTextureType noiseTexture)
		{
			if (!postRenderer) return;
			if (!noiseTexture) return;

			postRenderer->GetCommand<CGlitchCommand>()
				->SetDuration(glitchDuration)
				->SetIntensity(glitchIntensity)
				->SetNoiseTexture(noiseTexture)
				->SetEaseType(EaseType::OutQuart)
				->SetEnable(true);
		};

	// -------------------------
	// Saturation 호출 람다 (추가)
	// -------------------------
	auto RequestSaturation = [postRenderer](_float saturationIntensity, _float saturationDuration)
		{
			if (!postRenderer) return;

			postRenderer->GetCommand<CSaturationCommand>()
				->SetIntensity(saturationIntensity)
				->SetSaturationType(ENUM(SATURATIONTYPE::SKINNED))
				->SetDuration(saturationDuration)
				->SetEaseType(EaseType::OutBack)
				->SetEnable(true);
		};

	AddParallelTimeScaleAll(preset);

	AddCall([this]() {
		CamDirector()->BeginWipeOut();
		UIDirector()->Show_Wipeout();
		UIDirector()->Hide_HUD(CUIDirector::BATTLE);
		});

	// (옵션) 시작 시 기본 새튜레이트를 한번 걸고 싶으면 여기서 호출
	// 단, CSaturationCommand가 "매 호출이 덮어쓰기"라면 이후 키에서 값이 바뀐다.
	AddCall([RequestSaturation, preset]() mutable {
		RequestSaturation(1.f, preset.fVFXDuration);
		});

	// -------------------------
	// 글리치 키 (그대로 유지)
	// -------------------------
	struct GlitchKeySec
	{
		_float timeSec;
		_float intensity;
		_float durSec;
		const char* noiseKey;
	};

	const GlitchKeySec glitchKeys[] =
	{
		{ 0.00f, 4.2f, 0.06f, "Eff_Noise_119.png" },
		{ 0.04f, 2.2f, 0.09f, "Eff_Noise_045_YC_01.png" },
		{ 0.32f, 2.2f, 0.26f, "Eff_Noise_086_LKJ_01.png" },
		{ 1.32f, 4.2f, 0.26f, "Eff_Noise_086_LKJ_01.png" },
		{ 2.02f, 2.2f, 0.06f, "Eff_Noise_003 (1).png" },
		{ 2.22f, 2.2f, 0.06f, "Eff_Noise_003 (1).png" },
		{ 2.35f, 3.2f, 0.26f, "Eff_Noise_119.png" },
		{ 3.10f, 2.2f, 0.06f, "Eff_Noise_003 (1).png" },
		{ 3.50f, 4.2f, 0.26f, "Eff_Noise_045_YC_01.png" },
		{ 4.00f, 6.2f, 0.06f, "Eff_Noise_003 (1).png" },
		{ 4.33f, 3.2f, 0.06f, "Eff_Noise_052.png" },
		{ 4.35f, 6.2f, 0.26f, "Eff_Noise_086_LKJ_01.png" },
	};

	// -------------------------
	// 새튜레이트 키(추가) : 글리치 사이사이에 펄스 넣기
	// -------------------------
	struct SaturationKeySec
	{
		_float timeSec;
		_float intensity;
		_float durSec;
	};

	const SaturationKeySec saturationKeys[] =
	{
		{ 0.00f, 1.35f, 0.10f },
		{ 0.32f, 1.15f, 0.18f },
		{ 1.32f, 1.40f, 0.22f },
		{ 2.35f, 1.20f, 0.16f },
		{ 3.50f, 1.30f, 0.20f },
		{ 4.35f, 1.55f, 0.25f },
	};

	const _uint glitchKeyCount = (_uint)(sizeof(glitchKeys) / sizeof(glitchKeys[0]));
	const _uint saturationKeyCount = (_uint)(sizeof(saturationKeys) / sizeof(saturationKeys[0]));

	// -------------------------
	// 핵심: 두 키를 "같은 타임라인"에서 merge
	// -------------------------
	_uint glitchIndex = 0;
	_uint saturationIndex = 0;

	_float accumulatedTimeSec = 0.f;

	while (glitchIndex < glitchKeyCount || saturationIndex < saturationKeyCount)
	{
		const _float nextGlitchTime =
			(glitchIndex < glitchKeyCount)
			? clamp(glitchKeys[glitchIndex].timeSec, 0.f, totalDuration)
			: 1e9f;

		const _float nextSaturationTime =
			(saturationIndex < saturationKeyCount)
			? clamp(saturationKeys[saturationIndex].timeSec, 0.f, totalDuration)
			: 1e9f;

		const _float targetTimeSec = (nextGlitchTime < nextSaturationTime) ? nextGlitchTime : nextSaturationTime;

		const _float waitTimeSec = max(0.f, targetTimeSec - accumulatedTimeSec);
		AddWait(waitTimeSec);
		accumulatedTimeSec += waitTimeSec;

		// 같은 시점이면 둘 다 발동되게(부동소수 오차 대비)
		const _float timeEpsilon = 1e-4f;

		if (glitchIndex < glitchKeyCount && fabsf(nextGlitchTime - targetTimeSec) <= timeEpsilon)
		{
			const _float glitchIntensity = glitchKeys[glitchIndex].intensity;
			const _float glitchDurationSec = max(0.01f, min(glitchKeys[glitchIndex].durSec, totalDuration));

			const char* noiseKeyForThis = glitchKeys[glitchIndex].noiseKey;
			NoiseTextureType noiseTextureForThis =
				(noiseKeyForThis && noiseKeyForThis[0] != '\0')
				? ResourceManager()->Load_Texture(G_GlobalLevelKey, noiseKeyForThis)
				: defaultNoiseTexture;

			AddCall([RequestGlitch, glitchIntensity, glitchDurationSec, noiseTextureForThis]() mutable
				{
					RequestGlitch(glitchIntensity, glitchDurationSec, noiseTextureForThis);
				});

			++glitchIndex;
		}

		if (saturationIndex < saturationKeyCount && fabsf(nextSaturationTime - targetTimeSec) <= timeEpsilon)
		{
			const _float saturationIntensity = saturationKeys[saturationIndex].intensity;
			const _float saturationDurationSec = max(0.01f, min(saturationKeys[saturationIndex].durSec, totalDuration));

			AddCall([RequestSaturation, saturationIntensity, saturationDurationSec]() mutable
				{
					RequestSaturation(saturationIntensity, saturationDurationSec);
				});

			++saturationIndex;
		}
	}

	// 타임라인 끝까지 채우기
	if (accumulatedTimeSec < totalDuration)
		AddWait(totalDuration - accumulatedTimeSec);

	// 정리
	AddWait(preset.fVFXDuration);
	AddCall([this, preset]() {
		m_BattleVFX.fCurPos = 0.f;
		m_BattleVFX.vNowColor = {};
		m_BattleVFX.isRunning = false;
		CamDirector()->EndWipeOut();
		ObjectManager()->Get_Layer({ "Test_Level","PlacedObject_Layer" })->Set_RenderState(true);
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
