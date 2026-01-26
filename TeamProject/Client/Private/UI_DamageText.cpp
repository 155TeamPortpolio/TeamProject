#include "pch.h"
#include "UI_DamageText.h"

#include "ObjectContainer.h"
#include "GameInstance.h"
#include "LevelMgr.h"

namespace
{
    static const string kAtlasTexKey = "DamageText.png";

    constexpr _uint  kFrameCountX = 8;
    constexpr _uint  kFrameCountY = 8;

    // 한 글리프(숫자 한 자리)의 기준 높이(px)
    // 실제 렌더링 높이는 여기에 스케일(s)을 곱해서 결정됨
    constexpr _float kGlyphHeightPx = 96.f;

    // 글리프들 사이의 기본 간격(px)
    // kOverlapHold와 함께 최종 spacing을 계산하는 베이스 값
    constexpr _float kGlyphSpacingPx = 2.f;

    // 홀드(중간) 구간에서 글리프가 서로 얼마나 겹치게 할지 비율(0~1)
    // 1에 가까울수록 겹침이 거의 없고, 작을수록 더 많이 겹침
    // overlapPx = glyphW * (1 - kOverlapHold)
    constexpr _float kOverlapHold = 0.3f;

    // 전체 생애 동안 위로 떠오르는(상승) 픽셀량
    // 0이면 상승 모션 없음
    constexpr _float kRisePx = 0.f;

    // 전체 "등장" 구간의 총 시간(초)
    // 여러 자리일 때 digitInStaggerSec로 각 자리 등장 시작을 분산시키되,
    // 전체적으로 kInTotalSec 안에 다 들어오게 맞춤
    constexpr _float kInTotalSec = 0.4f;

    // 등장 후 화면에 유지되는(홀드) 시간(초)
    constexpr _float kHoldSec = 1.50f;

    // 전체 "퇴장" 구간의 총 시간(초)
    // 여러 자리일 때 digitOutStaggerSec로 각 자리 퇴장 시작을 분산시키되,
    // 전체적으로 kOutTotalSec 안에 다 들어오게 맞춤
    constexpr _float kOutTotalSec = 0.40f;

    // 한 자리(글리프)당 인(등장) 애니메이션 지속 시간(초)
    // 여러 자리일 때 각 자리별 인 애니가 이 시간만큼 진행됨
    constexpr _float kDigitInSec = 0.20f;

    // 한 자리(글리프)당 아웃(퇴장) 애니메이션 지속 시간(초)
    constexpr _float kDigitOutSec = 0.20f;

    // 알파(투명도) 아웃이 digitOutSec 중 얼마를 차지할지 비율(0~1)
    // alphaOutSec = digitOutSec * kAlphaOutSecRatio
    constexpr _float kAlphaOutSecRatio = 0.8f;

    // 스케일 애니메이션의 시작/홀드/끝 값
    // - Start: 처음 팡! 튀어나올 때 크기
    // - Hold : 유지 구간에서의 안정 크기
    // - End  : 사라질 때의 최종 크기
    constexpr _float kScaleStart = 1.85f;
    constexpr _float kScaleHold  = 0.68f;
    constexpr _float kScaleEnd   = 0.42f;

    // 스케일/알파에 적용하는 이징 타입
    // - ScaleEase: 크기 변화에 쓰는 곡선
    // - AlphaInEase: 등장 알파 곡선
    // - AlphaOutEase: 퇴장 알파 곡선
    constexpr EaseType kScaleEase = EaseType::OutCubic;
    constexpr EaseType kAlphaInEase = EaseType::InCubic;
    constexpr EaseType kAlphaOutEase = EaseType::OutQuad;

    // 자리수(count)에 따라 계산되는 "시간표" 묶음
    // 여러 자리일 때 각 자리의 시작 시점을 스태거(stagger)로 분산시키기 위해 사용
    struct DamageTextTiming
    {
        // 각 자리(글리프)별 인 애니 지속 시간
        _float digitInSec = 0.f;

        // 자리별 인 애니 시작 간격(스태거)
        // i번째 자리 inStart = i * digitInStaggerSec
        _float digitInStaggerSec = 0.f;

        // 각 자리(글리프)별 아웃 애니 지속 시간
        _float digitOutSec = 0.f;

        // 자리별 아웃 애니 시작 간격(스태거)
        // i번째 자리 outStart = exitStartSec + i * digitOutStaggerSec
        _float digitOutStaggerSec = 0.f;

        // 퇴장(아웃) 구간이 시작되는 시각(초)
        // exitStartSec = kInTotalSec + kHoldSec
        _float exitStartSec = 0.f;

        // 전체 애니 종료 시각(초)
        // endSec = exitStartSec + kOutTotalSec
        _float endSec = 0.f;

        // 알파 아웃에 실제로 쓰는 시간(초)
        // digitOutSec에서 kAlphaOutSecRatio만큼만 사용하도록 잘라 쓴 값
        _float alphaOutSec = 0.f;
    };

    // 자리수(count)에 맞춰 DamageTextTiming을 계산하는 함수
    // - 인/아웃 총 시간을 kInTotalSec/kOutTotalSec에 맞추면서
    //   여러 자리일 때 각 자리 시작을 균등 분산(stagger)함
    // - 한 자리면 스태거가 0이고, 인/아웃 시간이 총 시간과 동일하게 맞춰짐
    static DamageTextTiming CalcTiming(_uint count)
    {
        DamageTextTiming t{};

        if (count > 1u)
        {
            t.digitInSec = kDigitInSec;
            t.digitInStaggerSec = (kInTotalSec - t.digitInSec) / (_float)(count - 1u);
        }
        else
        {
            t.digitInSec = kInTotalSec;
            t.digitInStaggerSec = 0.f;
        }

        if (count > 1u)
        {
            t.digitOutSec = kDigitOutSec;
            t.digitOutStaggerSec = (kOutTotalSec - t.digitOutSec) / (_float)(count - 1u);
        }
        else
        {
            t.digitOutSec = kOutTotalSec;
            t.digitOutStaggerSec = 0.f;
        }

        t.exitStartSec = kInTotalSec + kHoldSec;
        t.endSec = t.exitStartSec + kOutTotalSec;

        t.alphaOutSec = t.digitOutSec * kAlphaOutSecRatio;

        return t;
    }
}

CUI_DamageText::CUI_DamageText(const CUI_DamageText& rhs) : CUI_WorldToScreen(rhs)
{
    m_glyphAspect = 1.f;
    m_time = 0.f;
    m_baseAnchorOffset = Vector2(0.f, 0.f);

    m_digits.clear();
    m_baseTotalW = 1.f;
    m_baseOffsets.clear();
    m_glyphs.clear();
}

HRESULT CUI_DamageText::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    Add_Component<CObjectContainer>();
    return S_OK;
}

HRESULT CUI_DamageText::Initialize(INIT_DESC* arg)
{
    __super::Initialize(arg);

    m_digits.clear();
    m_baseOffsets.clear();
    m_glyphs.clear();

    m_time = 0.f;
    m_glyphAspect = 1.f;
    m_worldPos = _float3(0.f, 0.f, 0.f);

    auto base = Get_AnchorOffset();
    m_baseAnchorOffset = Vector2(base.x, base.y);

    Set_Color(_float4(1.f, 1.f, 1.f, 1.f));

    return S_OK;
}

void CUI_DamageText::Update(_float dt)
{
    __super::Update(dt);

    if (m_digits.empty()) return;

    Update_WorldToScreen(m_worldPos);

    auto base = Get_AnchorOffset();
    m_baseAnchorOffset = Vector2(base.x, base.y);

    Update_Anim(dt);
    Apply_LayoutScaled();

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_DamageText::UI_Active(void* arg)
{
    auto desc = static_cast<DAMAGE_DESC*>(arg);

    m_time = 0.f;
    m_worldPos = _float3(desc->pos.x, desc->pos.y, desc->pos.z);

    SetRenderLayer(RENDER_LAYER::Default);
    Update_WorldToScreen(m_worldPos);

    auto base = Get_AnchorOffset();
    m_baseAnchorOffset = Vector2(base.x, base.y);

    SetDamage(desc->damage);
}

void CUI_DamageText::UI_DeActive(void* arg)
{
    for (_uint i = 0; i < (_uint)m_glyphs.size(); ++i)
        GetGlyph(i)->Set_Alpha(0.f);

    m_digits.clear();
    m_baseOffsets.clear();
    m_baseTotalW = 1.f;

    m_time = 0.f;

    UIManager()->Remove_UIObject(this);
}

void CUI_DamageText::SetDamage(_int damage)
{
    m_digits = to_string(damage);

    const _uint count = (_uint)m_digits.size();
    Ensure_GlyphCount(count);

    for (_uint i = 0; i < count; ++i)
    {
        const _uint digit = (_uint)(m_digits[i] - '0');
        const _uint frameIndex = GetDigitFrameIdx(digit);

        auto glyph = GetGlyph(i);
        glyph->Set_FrameIndex(frameIndex);
        glyph->Set_Color(m_vColor);
        glyph->Set_Alpha(0.f);
    }

    for (_uint i = count; i < (_uint)m_glyphs.size(); ++i)
        GetGlyph(i)->Set_Alpha(0.f);

    {
        const auto px = GetGlyph(0)->Get_PxSize();
        m_glyphAspect = px.x / px.y;
    }

    Rebuild_BaseLayout();
    Apply_LayoutScaled();
}

void CUI_DamageText::Rebuild_BaseLayout()
{
    const _uint count = (_uint)m_digits.size();

    m_baseOffsets.clear();
    m_baseOffsets.resize(count);

    const _float glyphW = kGlyphHeightPx * m_glyphAspect;

    const _float overlapPx = glyphW * (1.f - kOverlapHold);
    const _float spacing = kGlyphSpacingPx - overlapPx;

    _float totalW = glyphW * (_float)count;
    if (count > 1u) totalW += spacing * (_float)(count - 1u);

    m_baseTotalW = max(1.f, totalW);

    _float x = -m_baseTotalW * 0.5f;
    for (_uint i = 0; i < count; ++i)
    {
        m_baseOffsets[i] = Vector2(x, 0.f);
        x += glyphW + spacing;
    }
}

void CUI_DamageText::Apply_LayoutScaled()
{
    const _uint count = (_uint)m_digits.size();
    const auto t = CalcTiming(count);

    _float rise = 0.f;
    if (kRisePx > 0.f)
    {
        const _float u = clamp(m_time / t.endSec, 0.f, 1.f);
        rise = kRisePx * u;
    }

    Set_AnchorOffset(Vector2(m_baseAnchorOffset.x, m_baseAnchorOffset.y - rise));

    const _float glyphW = kGlyphHeightPx * m_glyphAspect;

    for (_uint i = 0; i < count; ++i)
    {
        const _float inStart = (_float)i * t.digitInStaggerSec;

        _float s = kScaleStart;
        if (m_time >= inStart)
        {
            _float u = (m_time - inStart) / t.digitInSec;
            u = clamp(u, 0.f, 1.f);
            u = Math::ApplyEase(kScaleEase, u);
            s = Math::Lerp(kScaleStart, kScaleHold, u);
        }

        const _float outStart = t.exitStartSec + (_float)i * t.digitOutStaggerSec;
        if (m_time >= outStart)
        {
            _float u = (m_time - outStart) / t.digitOutSec;
            u = clamp(u, 0.f, 1.f);
            u = Math::ApplyEase(kScaleEase, u);
            s = Math::Lerp(kScaleHold, kScaleEnd, u);
        }

        const Vector2 base = m_baseOffsets[i];

        const _float dx = (glyphW - glyphW * s) * 0.5f;
        const _float dy = (kGlyphHeightPx - kGlyphHeightPx * s) * 0.5f;

        auto glyph = GetGlyph(i);
        glyph->Set_AnchorOffset(Vector2(base.x + dx, base.y + dy));
        glyph->Set_HeightPx(kGlyphHeightPx * s);
    }

    Set_Size(Vector2(m_baseTotalW, kGlyphHeightPx));
}

void CUI_DamageText::Update_Anim(_float dt)
{
    const _uint count = (_uint)m_digits.size();
    if (count == 0u) return;

    m_time += dt;

    const auto t = CalcTiming(count);

    for (_uint i = 0; i < count; ++i)
    {
        const _float inStart = (_float)i * t.digitInStaggerSec;

        _float alphaIn = 0.f;
        if (m_time >= inStart)
        {
            _float u = (m_time - inStart) / t.digitInSec;
            u = clamp(u, 0.f, 1.f);
            alphaIn = Math::ApplyEase(kAlphaInEase, u);
        }

        const _float outStart = t.exitStartSec + (_float)i * t.digitOutStaggerSec;

        _float alphaOut = 1.f;
        if (m_time >= outStart)
        {
            _float u = (m_time - outStart) / t.alphaOutSec;
            u = clamp(u, 0.f, 1.f);
            alphaOut = 1.f - Math::ApplyEase(kAlphaOutEase, u);
        }

        const _float alpha = clamp(alphaIn * alphaOut, 0.f, 1.f);
        GetGlyph(i)->Set_Alpha(alpha * m_vColor.w);
    }

    if (m_time >= t.endSec)
        UI_DeActive();
}

void CUI_DamageText::Ensure_GlyphCount(_uint count)
{
    auto container = Get_Component<CObjectContainer>();
    while ((_uint)m_glyphs.size() < count)
    {
        auto pDesc         = new CUI_AtlasSprite::ATLAS_DESC;
        pDesc->texKey      = kAtlasTexKey;
        pDesc->frameCountX = kFrameCountX;
        pDesc->frameCountY = kFrameCountY;
        pDesc->frameIdx    = 0;
        pDesc->heightPx    = kGlyphHeightPx;

        auto builder = Builder::Create_UIObject({G_GlobalLevelKey, "Proto_GameObject_AtlasSprite"});
        builder.Add_UIDesc(pDesc);

        CUI_Object* obj = builder.Build("damageGlyph" + to_string((_uint)m_glyphs.size()));
        auto glyph = static_cast<CUI_AtlasSprite*>(obj);

        container->Add_Child(glyph);
        m_glyphs.push_back(glyph);

        glyph->Align_To(ANCHOR::Left | ANCHOR::Top);
        glyph->Set_Pivot({0.f, 0.f});
    }
}

_uint CUI_DamageText::GetDigitFrameIdx(_uint digit) const
{
    const _uint col = (digit >= 8) ? 1u : 0u;
    const _uint rowBottom = (digit >= 8) ? (digit - 8u) : digit;
    const _uint rowTop = (kFrameCountY - 1u) - rowBottom;
    return rowTop * kFrameCountX + col;
}

CGameObject* CUI_DamageText::Create()
{
    auto inst = new CUI_DamageText();
    if (FAILED(inst->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_DamageText");
        Safe_Release(inst);
    }
    return inst;
}

CGameObject* CUI_DamageText::Clone(INIT_DESC* pArg)
{
    auto inst = new CUI_DamageText(*this);
    if (FAILED(inst->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_DamageText");
        Safe_Release(inst);
    }
    return inst;
}