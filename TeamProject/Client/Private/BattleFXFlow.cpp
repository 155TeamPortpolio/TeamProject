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
		const _float duration = 1.f;
		evade.bCanIntersect = true;
		evade.fVFXDuration = duration;
		evade.vTargetColor = { 0.96f,0.98f,0.96f };
		evade.vStartColor = { 1.f,1.f,1.f };
		evade.fBlurDuration = .4f;
		evade.SetTimeData({ duration, 0.4f, 0.8f, 0.2f , EaseType::OutCubic });
	}
	{
		auto& Parry = m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::PARRY)];
		const _float duration = 1.f;
		Parry.bCanIntersect = true;
		Parry.fVFXDuration = duration;
		Parry.fBlurDuration = .3f;
		Parry.SetTimeData({ duration, 0.1f, 0.05f, 0.55f, EaseType::OutCubic });
		Parry.BattleTimeScale[ENUM(BATTLE_OBJ_TYPE::CAMERA)] =	TIME_SCALE_DATA{ duration, 1.0f, 0.3f, .0f, EaseType::OutQuint };
	}

	{
		auto& Switch = m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::SWITCH)];
		const _float duration = 2.5f;
		Switch.bCanIntersect = false;
		Switch.fVFXDuration = duration;
		Switch.fBlurDuration = 2.3f;
		Switch.SetTimeData({ duration, 0.02f, 0.15f, 0.05f, EaseType::InOutSine });
		Switch.BattleTimeScale[ENUM(BATTLE_OBJ_TYPE::CAMERA)] = TIME_SCALE_DATA{ duration, 1.0f, 0.3f, .0f, EaseType::OutQuint };
	}
	{
		auto& SwitchCancle = m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::CANCLESWITCH)];
		const _float duration = 1.f;
		SwitchCancle.bCanIntersect = false;
		SwitchCancle.fVFXDuration = duration;
		SwitchCancle.fBlurDuration = .2f;
		SwitchCancle.SetTimeData({ duration, 0.4f, 0.35f, 0.35f, EaseType::InOutBack });
		SwitchCancle.BattleTimeScale[ENUM(BATTLE_OBJ_TYPE::CAMERA)] = TIME_SCALE_DATA{ duration, 1.0f, 0.3f, .0f, EaseType::OutQuint };
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
		const _float duration = .09f;
		NormalHitLack.bCanIntersect = true;
		NormalHitLack.fVFXDuration = duration;
		NormalHitLack.fBlurDuration = duration;
		NormalHitLack.SetTimeData({ duration, 0.1f, 0.05f, .2f , EaseType::InOutSine });
		NormalHitLack.BattleTimeScale[ENUM(BATTLE_OBJ_TYPE::CAMERA)] = TIME_SCALE_DATA{ duration, 1.0f, 0.3f, .0f, EaseType::OutQuint };
	}

	{
		auto& HardHitLack = m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::HIT_HARD)];
		const _float duration = .1f;
		HardHitLack.bCanIntersect = true;
		HardHitLack.fVFXDuration = duration;
		HardHitLack.fBlurDuration = duration;
		HardHitLack.SetTimeData({ duration, 0.1f, 0.05f, .05f , EaseType::OutExpo });
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
	{
		auto& Clear = m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::CLEAR)];
		const _float duration = 1.2f;
		Clear.bCanIntersect = false;
		Clear.fVFXDuration = duration;
		Clear.fBlurDuration = 0.1;
		Clear.SetTimeData({ duration, 0.0f, 0.1f, .7f , EaseType::OutQuint });
		Clear.BattleTimeScale[ENUM(BATTLE_OBJ_TYPE::CAMERA)] =
			TIME_SCALE_DATA{duration, 1.f, 0.8f, 0.1f, EaseType::OutQuint};
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
		if (track.onEnd) track.onEnd();

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
		if (track.onEnd) track.onEnd();

	m_isRunning = false;
	m_stepIndex = 0;
	m_steps.clear();
	m_parallelTracks.clear();
	m_onEnd = {};

	CPostRenderer* postRenderer = RenderSystem()->GetPostRenderer();
	if (postRenderer)
	{
		postRenderer->GetCommand<CAddictiveColorCommand>()->SetEnable(false);
		postRenderer->GetCommand<CRadialBlurCommand>()->SetEnable(false);
		postRenderer->GetCommand<CSaturationCommand>()->SetEnable(false);
	}

	ResetBattleVfxState(); 
}


_bool CBattleFXFlow::IsRunning(BATTLE_VFX_TYPE vfxType) const
{
	return (m_isRunning&&m_BattleVFX.isRunning && m_BattleVFX.eVFXType == vfxType);
}

void CBattleFXFlow::StartVfx(BATTLE_VFX_TYPE vfxType)
{
	if (m_BattleVFX.isRunning && !m_BattleVFX.bCanIntersect)
	{
		if (m_isRunning)
			return;
		ResetBattleVfxState();
	}

	if (m_BattleVFX.isRunning)
	{
		Cancel(); 
	}

	auto& preset = m_BattleVFXData[ENUM(vfxType)];

	m_BattleVFX.isRunning = true;
	m_BattleVFX.eVFXType = vfxType;
	m_BattleVFX.fDuration = preset.fVFXDuration;
	m_BattleVFX.fCurPos = 0.f;
	m_BattleVFX.vNowColor = preset.vStartColor;
	m_BattleVFX.bCanIntersect = preset.bCanIntersect;

	switch (vfxType)
	{
	case BATTLE_VFX_TYPE::EVADE:      StartVfx_Evade(); break;
	case BATTLE_VFX_TYPE::PARRY:      StartVfx_Parry(); break;
	case BATTLE_VFX_TYPE::SWITCH:     StartVfx_Switch(CHARACTER::JaneDoe, CHARACTER::Miyabi); break;
	case BATTLE_VFX_TYPE::ULTIMATE:   StartVfx_Ultimate(); break;
	case BATTLE_VFX_TYPE::HIT_NORMAL: NormalHitLack(); break;
	case BATTLE_VFX_TYPE::HIT_HARD:   HardHitLack(); break;
	case BATTLE_VFX_TYPE::WIPEOUT:    StartVfx_WipeOut(); break;
	case BATTLE_VFX_TYPE::CLEAR:      StartVfx_Clear(); break;
	default:
		ResetBattleVfxState();
		return;
	}

	if (m_BattleVFX.isRunning && !m_isRunning && m_steps.empty() && m_parallelTracks.empty())
	{
		ResetBattleVfxState();
	}
}

void CBattleFXFlow::StartVfx_Evade()
{
	Clear(false);

	auto& preset = m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::EVADE)];
	AddParallelTimeScaleAll(preset);

	m_BattleVFX.isRunning = true;
	m_BattleVFX.fDuration = max(preset.fVFXDuration, 0.01f);
	m_BattleVFX.fCurPos = 0.f;
	m_BattleVFX.vNowColor = preset.vStartColor;

	CPostRenderer* postRenderer = RenderSystem()->GetPostRenderer();

	const _float totalDurationSeconds = m_BattleVFX.fDuration;
	const _float halfDurationSeconds = totalDurationSeconds * 0.3f;
	const _float blurDurationSeconds = max(totalDurationSeconds - halfDurationSeconds, 0.01f);

	// 애디티브/새츄레이션: 시작에 켜고, 끝에만 끈다
	AddCall([this, postRenderer, totalDurationSeconds]()
		{
			if (!postRenderer) return;

			postRenderer->GetCommand<CAddictiveColorCommand>()
				->SetAddictiveColor(&m_BattleVFX.vNowColor)
				->SetEnable(true);

			postRenderer->GetCommand<CSaturationCommand>()
				->SetIntensity(1.f)
				->SetSaturationType(ENUM(SATURATIONTYPE::FULL))
				->SetDuration(totalDurationSeconds)
				->SetEaseType(EaseType::InOutCubic)
				->SetEnable(true);
		});

	// 0 ~ 0.5 : start -> target
	AddStep([this, preset, elapsedSeconds = 0.f, halfDurationSeconds](_float deltaTime) mutable -> _bool
		{
			elapsedSeconds += deltaTime;

			const _float time01 = clamp(elapsedSeconds / max(halfDurationSeconds, 0.01f), 0.f, 1.f);
			const _float eased01 = Math::ApplyEase(EaseType::OutCubic, time01);

			_vector startColorVec = XMLoadFloat3(&preset.vStartColor);
			_vector targetColorVec = XMLoadFloat3(&preset.vTargetColor);
			XMStoreFloat3(&m_BattleVFX.vNowColor, XMVectorLerp(startColorVec, targetColorVec, eased01));

			m_BattleVFX.fCurPos = halfDurationSeconds * time01;
			return elapsedSeconds < halfDurationSeconds;
		});

	// 0.5 시점에 블러 시작 (애디티브는 끄지 않음)
	AddCall([postRenderer, blurDurationSeconds]()
		{
			if (!postRenderer) return;

			postRenderer->GetCommand<CRadialBlurCommand>()
				->SetDuration(blurDurationSeconds)
				->SetEaseType(EaseType::OutSine)
				->SetIntensity(0.1f)
				->SetEnable(true);
		});

	// 0.5 ~ 1.0 : 현재 색 -> (1,1,1) 로 복귀
	AddStep([this, elapsedSeconds = 0.f, blurDurationSeconds, startFadeColor = _float3{}](_float deltaTime) mutable -> _bool
		{
			if (elapsedSeconds == 0.f)
				startFadeColor = m_BattleVFX.vNowColor;

			elapsedSeconds += deltaTime;

			const _float time01 = clamp(elapsedSeconds / max(blurDurationSeconds, 0.01f), 0.f, 1.f);
			const _float eased01 = Math::ApplyEase(EaseType::OutSine, time01);

			_float3 neutralColor = { 1.f, 1.f, 1.f }; 
			_vector fromColorVec = XMLoadFloat3(&startFadeColor);
			_vector toColorVec = XMLoadFloat3(&neutralColor);

			XMStoreFloat3(&m_BattleVFX.vNowColor, XMVectorLerp(fromColorVec, toColorVec, eased01));
			return elapsedSeconds < blurDurationSeconds;
		});

	AddCall([this, postRenderer]()
		{
			m_BattleVFX.vNowColor = { 1.f, 1.f, 1.f }; 
			if (postRenderer)
			{
				postRenderer->GetCommand<CAddictiveColorCommand>()->SetEnable(false);
				postRenderer->GetCommand<CRadialBlurCommand>()->SetEnable(false);
				postRenderer->GetCommand<CSaturationCommand>()->SetEnable(false);
			}

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
			->SetIntensity(0.25)
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

void CBattleFXFlow::StartVfx_Switch(CHARACTER eLeft, CHARACTER eRight)
{
	Clear(false);
	auto& preset = m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::SWITCH)];
	m_BattleVFX.isRunning = true;
	m_BattleVFX.eVFXType = BATTLE_VFX_TYPE::SWITCH;
	m_BattleVFX.fDuration = preset.fVFXDuration;
	m_BattleVFX.fCurPos = 0.f;
	m_BattleVFX.vNowColor = preset.vStartColor;
	m_BattleVFX.bCanIntersect = preset.bCanIntersect;

	//2.5f;
	CPostRenderer* postRenderer = RenderSystem()->GetPostRenderer();
	const _float totalDuration = max(preset.fVFXDuration, 0.01f);
	const _float blurDuration = min(preset.fBlurDuration, totalDuration);
	if (!postRenderer) return;
	AddParallelTimeScaleAll(preset);
	AddCall([postRenderer, totalDuration]()
	{
		postRenderer->GetCommand<CSaturationCommand>()
			->SetIntensity(1.f)
			->SetSaturationType(ENUM(SATURATIONTYPE::STATIC))
			->SetDuration(totalDuration)
			->SetEaseType(EaseType::InOutCubic)
			->SetEnable(true);
	});

	AddCall([this]() { CamDirector()->EnterSwitch(); });

	AddCall([this, eLeft, eRight](){
		UIDirector()->Show_Switch(eLeft,eRight);
		UIDirector()->Hide_HUD(CUIDirector::BATTLE);
		});
	AddWait(0.3f);
	AddCall([postRenderer, blurDuration]()
		{
			postRenderer->GetCommand<CRadialBlurCommand>()
				->SetDuration(blurDuration)
				->SetEaseType(EaseType::InOutSine)
				->SetIntensity(0.15f)
				->SetEnable(true);
		});

	AddWait(totalDuration - 0.2f);

	AddCall([this]()
		{
			m_BattleVFX.fCurPos = 0.f;
			m_BattleVFX.vNowColor = {};
			m_BattleVFX.isRunning = false;
		});

	Start([]() {
		UIDirector()->Hide_Switch();
		UIDirector()->Show_HUD(CUIDirector::BATTLE);
		});
}

void CBattleFXFlow::Cancle_Switch()
{
	if (m_BattleVFX.isRunning && m_BattleVFX.eVFXType == BATTLE_VFX_TYPE::SWITCH) {
		Cancel();
		UIDirector()->Hide_Switch();
		UIDirector()->Show_HUD(CUIDirector::BATTLE);
		CamDirector()->EndSwitch();
	}
	Clear(false);
	auto& preset = m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::CANCLESWITCH)];
	m_BattleVFX.isRunning = true;
	m_BattleVFX.eVFXType = BATTLE_VFX_TYPE::CANCLESWITCH;
	m_BattleVFX.fDuration = preset.fVFXDuration;
	m_BattleVFX.fCurPos = 0.f;
	m_BattleVFX.vNowColor = preset.vStartColor;
	m_BattleVFX.bCanIntersect = preset.bCanIntersect;
	const _float totalDuration = max(preset.fVFXDuration, 0.01f);
	AddParallelTimeScaleAll(preset);
	AddWait(totalDuration);
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

	auto& preset = m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::WIPEOUT)];
	CPostRenderer* postRenderer = RenderSystem()->GetPostRenderer();

	const _float totalDuration = max(preset.fVFXDuration, 0.01f);
	const _float blurDuration = min(preset.fBlurDuration, totalDuration);
	(void)blurDuration;

	auto defaultNoiseTexture = ResourceManager()->Load_Texture(G_GlobalLevelKey, "Eff_Noise_052.png");
	using NoiseTextureType = decltype(defaultNoiseTexture);
	auto ChooseGlitchEase = [](_float glitchDuration)
		{
			if (glitchDuration <= 0.08f) return EaseType::OutQuad;
			if (glitchDuration <= 0.18f) return EaseType::OutCubic;
			return  EaseType::OutExpo;
		};

	auto RequestGlitch = [postRenderer, ChooseGlitchEase](_float glitchIntensity, _float glitchDuration, NoiseTextureType noiseTexture)
		{
			if (!postRenderer) return;
			if (!noiseTexture) return;

			postRenderer->GetCommand<CGlitchCommand>()
				->SetDuration(glitchDuration)
				->SetIntensity(glitchIntensity)
				->SetNoiseTexture(noiseTexture)
				->SetEaseType(ChooseGlitchEase(glitchDuration))
				->SetEnable(true);
		};

	AddParallelTimeScaleAll(preset);

	AddCall([this, preset, postRenderer]() {
		if (!postRenderer) return;

		postRenderer->GetCommand<CSaturationCommand>()
			->SetIntensity(1.f)
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

	struct GlitchKeySec
	{
		_float timeSec;      // 발동 시점(초)
		_float intensity;    // 강도
		_float durSec;       // 지속 시간(초)
		const char* noiseKey; // 노이즈 텍스처 키(선택)
	};

	const GlitchKeySec glitchKeys[] =
	{
		{ 0.00f, 4.2f, 0.06f, "Eff_Noise_119.png" },
		{ 0.04f, 2.2f, 0.09f, "Eff_Noise_045_YC_01.png" },
		{ 0.32f, 2.2f, 0.26f, "Eff_Noise_086_LKJ_01.png" },
		{ 0.35f, 3.2f, 0.06f, "Eff_Noise_119.png" },
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

	_float accumulatedTimeSec = 0.f;
	const _uint glitchKeyCount = (_uint)(sizeof(glitchKeys) / sizeof(glitchKeys[0]));

	for (_uint keyIndex = 0; keyIndex < glitchKeyCount; ++keyIndex)
	{
		const _float targetTimeSec = clamp(glitchKeys[keyIndex].timeSec, 0.f, totalDuration);
		const _float waitTimeSec = max(0.f, targetTimeSec - accumulatedTimeSec);

		AddWait(waitTimeSec);
		accumulatedTimeSec += waitTimeSec;

		const _float glitchIntensity = glitchKeys[keyIndex].intensity;
		const _float glitchDurationSec = max(0.01f, min(glitchKeys[keyIndex].durSec, totalDuration));

		// 키에 noiseKey가 있으면 그걸 로드, 없으면 기본 텍스처 사용
		const char* noiseKeyForThis = glitchKeys[keyIndex].noiseKey;
		NoiseTextureType noiseTextureForThis =
			(noiseKeyForThis && noiseKeyForThis[0] != '\0')
			? ResourceManager()->Load_Texture(G_GlobalLevelKey, noiseKeyForThis)
			: defaultNoiseTexture;

		AddCall([RequestGlitch, glitchIntensity, glitchDurationSec, noiseTextureForThis]() mutable
			{
				RequestGlitch(glitchIntensity, glitchDurationSec, noiseTextureForThis);
			});
	}

	if (accumulatedTimeSec < totalDuration)
		AddWait(totalDuration - accumulatedTimeSec);

	AddWait(preset.fVFXDuration);
	AddCall([this, preset]() {
		m_BattleVFX.fCurPos = 0.f;
		m_BattleVFX.vNowColor = {};
		m_BattleVFX.isRunning = false;
		CamDirector()->EndWipeOut();
		UIDirector()->Show_HUD(CUIDirector::BATTLE);
		});

	Start(nullptr);
}

void CBattleFXFlow::StartVfx_Clear()
{
	Clear(false);
	auto& preset = m_BattleVFXData[ENUM(BATTLE_VFX_TYPE::CLEAR)];
	AddParallelTimeScaleAll(preset);
	CPostRenderer* pPost = RenderSystem()->GetPostRenderer();
	AddCall([pPost, preset]()
		{
			if (!pPost) return;
			pPost->GetCommand<CSaturationCommand>()
				->SetIntensity(1.f)
				->SetSaturationType(ENUM(SATURATIONTYPE::STATIC))
				->SetDuration(preset.fVFXDuration)
				->SetEaseType(EaseType::InOutCubic)
				->SetEnable(true);

			pPost->GetCommand<CRadialBlurCommand>()
				->SetDuration(preset.fBlurDuration)
				->SetEaseType(EaseType::OutSine)
				->SetIntensity(.6f)
				->SetEnable(true);
		});

	AddCall([this, preset]() { CameraManager()->SetFov(-15.f, 0.1, EaseType::InOutQuad, 15.f, preset.fVFXDuration- 0.05, EaseType::InOutSine);});
	AddWait(0.08);
	AddCall([this]() {
		UIDirector()->Show_Clear();
		UIDirector()->Hide_HUD(CUIDirector::BATTLE);
		});
	AddWait(preset.fVFXDuration);
	AddCall([this, preset]() {
		m_BattleVFX.fCurPos = 0.f;
		m_BattleVFX.vNowColor = {};
		m_BattleVFX.isRunning = false;
		UIDirector()->Show_HUD(CUIDirector::BATTLE);
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
	if (type == BATTLE_OBJ_TYPE::CAMERA || type == BATTLE_OBJ_TYPE::PLAYER)
		ObjectManager()->Set_LayerTimeScale({ G_GlobalLevelKey, m_layerTag[ENUM(type)] }, scale);
	else
		ObjectManager()->Set_LayerTimeScale({ nowLevelKey, m_layerTag[ENUM(type)] }, scale);

}

void CBattleFXFlow::ResetLayerTimeScale(BATTLE_OBJ_TYPE type)
{
	const string nowLevelKey = LevelManager()->Get_NowLevelKey();
	if (type == BATTLE_OBJ_TYPE::CAMERA|| type == BATTLE_OBJ_TYPE::PLAYER)
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

void CBattleFXFlow::ResetBattleVfxState()
{
	m_BattleVFX.fCurPos = 0.f;
	m_BattleVFX.vNowColor = {};
	m_BattleVFX.isRunning = false;
	m_BattleVFX.bCanIntersect = true;
	m_BattleVFX.eVFXType = BATTLE_VFX_TYPE::END; // 없으면 적당한 기본값
}

_bool CBattleFXFlow::IsValidTimeScale(const TIME_SCALE_DATA& timeScale)
{
	const _float eps = 1e-4f;
	if (timeScale.fDuration <= 0.f) return false;
	if (fabsf(timeScale.fValue - 1.f) <= eps) return false;
	if (timeScale.fValue < 0.f) return false;
	return true;
}

void CBattleFXFlow::LockBattleTime(_bool Lock)
{
	if (Lock) {
		for (size_t typeIndex = 0; typeIndex < ENUM(BATTLE_OBJ_TYPE::END); ++typeIndex)
			SetLayerTimeScale(BATTLE_OBJ_TYPE(typeIndex), 0.f);
	}
	else {
		for (size_t typeIndex = 0; typeIndex < ENUM(BATTLE_OBJ_TYPE::END); ++typeIndex)
			ResetLayerTimeScale(BATTLE_OBJ_TYPE(typeIndex));
	}
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
