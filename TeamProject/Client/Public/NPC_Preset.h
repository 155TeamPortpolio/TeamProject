#pragma once
#include "Engine_Defines.h"
#include "Material.h"
#include "SkeletalModel.h"
#include "Animator3D.h"

static vector<string> NPC_MALE_ANIM = {
"_Idle_200",
"_Stand_Cough01_075",
"_Stand_DF_Idle01_051",
"_Stand_Idle01_001",
"_Stand_Idle02_002",
"_Stand_Look_048",
"_Stand_Positive_026",
"_Stand_Talk03_010",
"_Stand_Thinking01_008",
"_Stand_Wait01_006",
"_Stand_Wait01_007",
};

static vector<string> NPC_FEMALE_ANIM = {
"_Idle_200",
"_Stand_Cough01_075",
"_Stand_DF_Idle01_051",
"_Stand_Idle01_001",
"_Stand_Idle02_002",
"_Stand_Look_048",
"_Stand_Positive_026",
"_Stand_Talk03_010",
"_Stand_Thinking01_008",
"_Stand_Wait01_006",
"_Stand_Wait01_007",
};
struct NpcColorPreset
{
    _float4 HairColor = { 1.00f, 0.86f, 0.70f, 1.f };
    _float4 SkinColor = { 1.00f, 0.86f, 0.70f, 1.f };
    _float4 ClothColor = { 1.00f, 0.86f, 0.70f, 1.f };

private:
    struct FastRng
    {
        _uint state = 0xA341316Cu;
        void Seed(_uint seedValue) { state = (seedValue != 0u) ? seedValue : 0xA341316Cu; }

        _uint NextU32()
        {
            state ^= (state << 13);
            state ^= (state >> 17);
            state ^= (state << 5);
            return state;
        }

        _float Next01()
        {
            const _uint masked = (NextU32() & 0x00FFFFFFu);
            return (_float)masked / 16777215.0f;
        }

        _float Range(_float minValue, _float maxValue)
        {
            return minValue + (maxValue - minValue) * Next01();
        }
    };
    static _uint MixSeed(_uint seedBase, _uint salt)
    {
        _uint mixed = seedBase ^ (salt * 0x9E3779B9u);
        mixed ^= (mixed >> 16);
        mixed *= 0x85EBCA6Bu;
        mixed ^= (mixed >> 13);
        mixed *= 0xC2B2AE35u;
        mixed ^= (mixed >> 16);
        return mixed;
    }
    static _float WrapHue(_float hueDeg)
    {
        while (hueDeg >= 360.f) hueDeg -= 360.f;
        while (hueDeg < 0.f)  hueDeg += 360.f;
        return hueDeg;
    }
    static _float ClampHueInRange(_float hueDeg, _float minHueDeg, _float maxHueDeg)
    {
        hueDeg = WrapHue(hueDeg);

        // 이 구현은 "min < max"인 일반 구간만 사용한다고 가정 (피부는 20~55 같은 형태)
        if (hueDeg < minHueDeg) return minHueDeg;
        if (hueDeg > maxHueDeg) return maxHueDeg;
        return hueDeg;
    }
    static void RgbToHsv(const _float3& rgb, _float3& hsvOut)
    {
        const _float red = rgb.x;
        const _float green = rgb.y;
        const _float blue = rgb.z;

        const _float maxValue = max(red, max(green, blue));
        const _float minValue = min(red, min(green, blue));
        const _float delta = maxValue - minValue;

        _float hue = 0.f;
        _float sat = (maxValue <= 1e-6f) ? 0.f : (delta / maxValue);
        _float val = maxValue;

        if (delta > 1e-6f)
        {
            if (maxValue == red)        hue = (green - blue) / delta;
            else if (maxValue == green) hue = 2.f + (blue - red) / delta;
            else                        hue = 4.f + (red - green) / delta;

            hue *= 60.f;
            if (hue < 0.f) hue += 360.f;
        }

        hsvOut = { hue, sat, val };
    }
    static void HsvToRgb(const _float3& hsv, _float3& rgbOut)
    {
        _float hue = WrapHue(hsv.x);
        const _float sat = clamp(hsv.y, 0.f, 1.f);
        const _float val = clamp(hsv.z, 0.f, 1.f);

        if (sat <= 1e-6f)
        {
            rgbOut = { val, val, val };
            return;
        }

        const _float sector = hue / 60.f;
        const _int sectorIndex = (_int)sector;
        const _float fraction = sector - (_float)sectorIndex;

        const _float p = val * (1.f - sat);
        const _float q = val * (1.f - sat * fraction);
        const _float t = val * (1.f - sat * (1.f - fraction));

        switch (sectorIndex)
        {
        case 0:  rgbOut = { val, t,   p }; break;
        case 1:  rgbOut = { q,   val, p }; break;
        case 2:  rgbOut = { p,   val, t }; break;
        case 3:  rgbOut = { p,   q,   val }; break;
        case 4:  rgbOut = { t,   p,   val }; break;
        default: rgbOut = { val, p,   q }; break;
        }
    }
    static _float4 ApplyHsvJitterConstrained(
        _float4 baseColor,
        FastRng& rng,
        _float hueJitterDeg,
        _float satJitter,
        _float valJitter,
        _float hueMinDeg, _float hueMaxDeg,
        _float satMin, _float satMax,
        _float valMin, _float valMax,
        _bool  preventNeon)
    {
        _float3 rgb = { baseColor.x, baseColor.y, baseColor.z };
        _float3 hsv = {};
        RgbToHsv(rgb, hsv);

        hsv.x = ClampHueInRange(hsv.x + rng.Range(-hueJitterDeg, hueJitterDeg), hueMinDeg, hueMaxDeg);
        hsv.y = clamp(hsv.y + rng.Range(-satJitter, satJitter), satMin, satMax);
        hsv.z = clamp(hsv.z + rng.Range(-valJitter, valJitter), valMin, valMax);

        // 네온 방지: 채도가 높으면 명도를 살짝 낮춰서 쨍함 억제
        if (preventNeon)
        {
            if (hsv.y > 0.65f)
                hsv.z = min(hsv.z, 0.80f);
            if (hsv.z > 0.90f)
                hsv.y = min(hsv.y, 0.55f);
        }

        HsvToRgb(hsv, rgb);

        baseColor.x = clamp(rgb.x, 0.f, 1.f);
        baseColor.y = clamp(rgb.y, 0.f, 1.f);
        baseColor.z = clamp(rgb.z, 0.f, 1.f);
        baseColor.w = 1.f;
        return baseColor;
    }
    static void SetVariationColor(CMaterial* material, const char* instanceName, _float4* color)
    {
        if (!material || !instanceName || !color) return;

        auto materialInstance = material->Find_MaterialInstanceByName(instanceName);
        if (!materialInstance) return;
        materialInstance->Set_Param("fVariationColor", { color, "float4", sizeof(_float4) });
    }

public:
    void Randomize_Natural(_uint seedValue)
    {
        FastRng rngSkin;  rngSkin.Seed(MixSeed(seedValue, 0x1111u));
        FastRng rngHair;  rngHair.Seed(MixSeed(seedValue, 0x2222u));
        FastRng rngCloth; rngCloth.Seed(MixSeed(seedValue, 0x3333u));

        auto MakeColorFromHsv = [](_float hueDeg, _float sat, _float val) -> _float4
            {
                _float3 rgb = {};
                _float3 hsv = { hueDeg, sat, val };
                HsvToRgb(hsv, rgb);
                return _float4{ clamp(rgb.x,0.f,1.f), clamp(rgb.y,0.f,1.f), clamp(rgb.z,0.f,1.f), 1.f };
            };

        // --------------------
        // Skin (기존 제약 유지)
        // --------------------
        const _float skinPick = rngSkin.Next01();

        _float skinHueMin = 0.f, skinHueMax = 0.f;
        _float skinSatMin = 0.f, skinSatMax = 0.f;
        _float skinValMin = 0.f, skinValMax = 0.f;

        // 톤 그룹 (가중치 예시)
        // 10% very light / 25% light / 30% medium / 20% tan / 15% deep
        if (skinPick < 0.10f)
        {
            // Very Light (porcelain)
            skinHueMin = 18.f;  skinHueMax = 40.f;
            skinSatMin = 0.06f; skinSatMax = 0.22f;
            skinValMin = 0.88f; skinValMax = 1.00f;
        }
        else if (skinPick < 0.35f)
        {
            // Light
            skinHueMin = 18.f;  skinHueMax = 45.f;
            skinSatMin = 0.10f; skinSatMax = 0.30f;
            skinValMin = 0.78f; skinValMax = 0.98f;
        }
        else if (skinPick < 0.65f)
        {
            // Medium
            skinHueMin = 18.f;  skinHueMax = 50.f;
            skinSatMin = 0.14f; skinSatMax = 0.38f;
            skinValMin = 0.62f; skinValMax = 0.88f;
        }
        else if (skinPick < 0.85f)
        {
            // Tan
            skinHueMin = 16.f;  skinHueMax = 52.f;
            skinSatMin = 0.16f; skinSatMax = 0.44f;
            skinValMin = 0.48f; skinValMax = 0.78f;
        }
        else
        {
            // Deep
            skinHueMin = 14.f;  skinHueMax = 48.f;
            skinSatMin = 0.16f; skinSatMax = 0.50f;
            skinValMin = 0.28f; skinValMax = 0.62f;
        }

        // 언더톤 (웜/뉴트럴/쿨) - 같은 톤 그룹 안에서 Hue 범위를 살짝 좁혀 느낌을 다르게
        const _float undertonePick = rngSkin.Next01();

        if (undertonePick < 0.40f)
        {
            // Warm: 노랑/골드 쪽(대체로 Hue ↑)
            skinHueMin = max(skinHueMin, 28.f);
            skinHueMax = min(skinHueMax, 55.f);
            skinSatMin = max(skinSatMin, 0.12f);
        }
        else if (undertonePick < 0.80f)
        {
            // Neutral: 그대로
        }
        else
        {
            // Cool: 핑크/레드 쪽(대체로 Hue ↓)
            skinHueMin = max(skinHueMin, 12.f);
            skinHueMax = min(skinHueMax, 36.f);
            skinSatMax = min(skinSatMax, 0.42f);
        }

        const _float hueBase = rngSkin.Range(skinHueMin, skinHueMax);
        const _float satBase = rngSkin.Range(skinSatMin, skinSatMax);
        const _float valBase = rngSkin.Range(skinValMin, skinValMax);

        _float4 baseSkin = MakeColorFromHsv(hueBase, satBase, valBase);

        // 기존(2.5/0.03/0.06)보다 훨씬 크게 흔들되, 팔레트 범위 밖으로 못 나가게 Constrained
        SkinColor = ApplyHsvJitterConstrained(
            baseSkin, rngSkin,
            9.f, 0.07f, 0.10f,           // ← 피부용 큰 변주(필요시 6~12 / 0.05~0.10 / 0.07~0.12에서 조절)
            skinHueMin, skinHueMax,
            skinSatMin, skinSatMax,
            skinValMin, skinValMax,
            true);
        // --------------------
        // Hair (바리에이션 크게)
        //  - 대부분 자연 모발, 아주 소량 '염색 느낌' 옵션
        // --------------------
        {
            const _float hairRoll = rngHair.Next01();

            _float hairHueMin = 0.f, hairHueMax = 0.f;
            _float hairSatMin = 0.f, hairSatMax = 0.f;
            _float hairValMin = 0.f, hairValMax = 0.f;

            // 90%: 자연 모발 팔레트(검정/갈색/금발/적갈/회색)
            // 10%: 살짝 염색 느낌(하지만 네온 방지)
            if (hairRoll < 0.90f)
            {
                // 가중치 분기(대충 자연 분포 느낌)
                const _float naturalPick = rngHair.Next01();

                if (naturalPick < 0.30f)
                {
                    // Black
                    hairHueMin = 0.f;   hairHueMax = 60.f;
                    hairSatMin = 0.00f; hairSatMax = 0.25f;
                    hairValMin = 0.05f; hairValMax = 0.20f;
                }
                else if (naturalPick < 0.65f)
                {
                    // Dark Brown ~ Brown
                    hairHueMin = 10.f;  hairHueMax = 45.f;
                    hairSatMin = 0.20f; hairSatMax = 0.75f;
                    hairValMin = 0.12f; hairValMax = 0.55f;
                }
                else if (naturalPick < 0.85f)
                {
                    // Light Brown ~ Blonde
                    hairHueMin = 30.f;  hairHueMax = 60.f;
                    hairSatMin = 0.15f; hairSatMax = 0.65f;
                    hairValMin = 0.45f; hairValMax = 0.90f;
                }
                else if (naturalPick < 0.95f)
                {
                    // Auburn / Red
                    hairHueMin = 5.f;   hairHueMax = 25.f;
                    hairSatMin = 0.25f; hairSatMax = 0.85f;
                    hairValMin = 0.20f; hairValMax = 0.70f;
                }
                else
                {
                    // Gray / White-ish
                    hairHueMin = 0.f;   hairHueMax = 359.999f;
                    hairSatMin = 0.00f; hairSatMax = 0.12f;
                    hairValMin = 0.45f; hairValMax = 0.95f;
                }

                // 팔레트에서 "기본 HSV"를 뽑고, 그걸 살짝 흔들어서 자연스럽게
                const _float baseHue = rngHair.Range(hairHueMin, hairHueMax);
                const _float baseSat = rngHair.Range(hairSatMin, hairSatMax);
                const _float baseVal = rngHair.Range(hairValMin, hairValMax);
                _float4 baseHair = MakeColorFromHsv(baseHue, baseSat, baseVal);

                HairColor = ApplyHsvJitterConstrained(
                    baseHair, rngHair,
                    25.f, 0.16f, 0.16f,       // ← 기존보다 크게
                    hairHueMin, hairHueMax,
                    hairSatMin, hairSatMax,
                    hairValMin, hairValMax,
                    true);                     // 네온 방지
            }
            else
            {
                // 염색 느낌(너무 튀지 않게: 채도/명도 상한)
                // 파랑/보라/초록 계열을 약간만
                const _float dyePick = rngHair.Next01();

                if (dyePick < 0.40f)
                {
                    // Blue
                    hairHueMin = 200.f; hairHueMax = 245.f;
                }
                else if (dyePick < 0.75f)
                {
                    // Purple
                    hairHueMin = 255.f; hairHueMax = 305.f;
                }
                else
                {
                    // Green
                    hairHueMin = 95.f;  hairHueMax = 150.f;
                }

                hairSatMin = 0.20f; hairSatMax = 0.70f;
                hairValMin = 0.15f; hairValMax = 0.75f;

                const _float baseHue = rngHair.Range(hairHueMin, hairHueMax);
                const _float baseSat = rngHair.Range(hairSatMin, hairSatMax);
                const _float baseVal = rngHair.Range(hairValMin, hairValMax);
                _float4 baseHair = MakeColorFromHsv(baseHue, baseSat, baseVal);

                HairColor = ApplyHsvJitterConstrained(
                    baseHair, rngHair,
                    10.f, 0.12f, 0.12f,
                    hairHueMin, hairHueMax,
                    hairSatMin, hairSatMax,
                    hairValMin, hairValMax,
                    true);
            }
        }

        // --------------------
        // Cloth (바리에이션 크게)
        //  - 팔레트 4종: muted / vibrant / dark / light
        // --------------------
        {
            const _float clothPick = rngCloth.Next01();

            _float clothHueMin = 0.f, clothHueMax = 359.999f;
            _float clothSatMin = 0.f, clothSatMax = 0.f;
            _float clothValMin = 0.f, clothValMax = 0.f;

            // 55% muted (현실적인 옷)
            // 25% vibrant (색감 있지만 네온 방지)
            // 10% dark (무채/짙은 톤)
            // 10% light (밝은 파스텔/오프화이트)
            if (clothPick < 0.55f)
            {
                clothSatMin = 0.10f; clothSatMax = 0.55f;
                clothValMin = 0.18f; clothValMax = 0.85f;
            }
            else if (clothPick < 0.80f)
            {
                clothSatMin = 0.35f; clothSatMax = 0.90f;
                clothValMin = 0.20f; clothValMax = 0.80f; // ← 밝기 상한으로 네온 억제
            }
            else if (clothPick < 0.90f)
            {
                clothSatMin = 0.00f; clothSatMax = 0.35f;
                clothValMin = 0.08f; clothValMax = 0.35f;
            }
            else
            {
                clothSatMin = 0.05f; clothSatMax = 0.45f;
                clothValMin = 0.70f; clothValMax = 0.95f;
            }

            const _float baseHue = rngCloth.Range(clothHueMin, clothHueMax);
            const _float baseSat = rngCloth.Range(clothSatMin, clothSatMax);
            const _float baseVal = rngCloth.Range(clothValMin, clothValMax);
            _float4 baseCloth = MakeColorFromHsv(baseHue, baseSat, baseVal);

            ClothColor = ApplyHsvJitterConstrained(
                baseCloth, rngCloth,
                22.f, 0.22f, 0.20f,          // ← 기존보다 크게
                clothHueMin, clothHueMax,
                clothSatMin, clothSatMax,
                clothValMin, clothValMax,
                true);
        }
    }
    void LinkMaterial(CMaterial* material)
    {
        SetVariationColor(material, "Body", &SkinColor);
        SetVariationColor(material, "Face", &SkinColor);
        SetVariationColor(material, "Hair", &HairColor);
        SetVariationColor(material, "Eye", &HairColor);
        SetVariationColor(material, "Cloth", &ClothColor);
    }
    void Render_Colors()
    {
        ImGui::DragFloat4("Hair", &HairColor.x, 0.01f, 0.f, 1.f);
        ImGui::DragFloat4("Skin", &SkinColor.x, 0.01f, 0.f, 1.f);
        ImGui::DragFloat4("Cloth", &ClothColor.x, 0.01f, 0.f, 1.f);
    }
};

struct NpcModelPreset {
private:
    _bool isMale = {};
public:
    void RandomizeModel(_bool isWalker, CSkeletalModel* pModel, CMaterial* pMaterial, CAnimator3D* pAnimator) {
        _bool isMale = Helper::Get_Random_Bool();
        string gederWord = isMale ? "Male" : "Female";
        _int variation = Helper::Get_Random_Int(1, isMale ? 6 : 2);
        string model = gederWord + "0" + to_string(variation) + ".model";
        string material = gederWord + "0" + to_string(variation) + ".mat";
        string meta = gederWord + "0" + to_string(variation) + "_Meta.json";

        pModel->Link_Model("MainCity_Level", model);
        pMaterial->Link_Material("MainCity_Level", material);

        pAnimator->LinkAnimate_Model("MainCity_Level", model);
        pAnimator->Link_MetaData("MainCity_Level", meta);

        if (isWalker) {
            string walk = gederWord + "0" + to_string(variation) + (isMale ? "_Walk_Normal02_015" : "_Walk_Normal_020");
            pAnimator->Set_Animation(walk).Loop(true).Apply();
        }
        else{
            if (isMale) {
                _int Anim = Helper::Get_Random_Int(0, NPC_MALE_ANIM.size() - 1);
                pAnimator->Set_Animation(Anim).Loop(true).Apply();
            }
            else {
                _int Anim = Helper::Get_Random_Int(0, NPC_FEMALE_ANIM.size() - 1);
                pAnimator->Set_Animation(Anim).Loop(true).Apply();
            }
        }
    }
  
};
