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
    "_Stand_Calm01_017",
    "_Stand_Calm01_018",
    "_Stand_Idle01_001",
    "_Stand_Positive_028",
    "_Stand_Talk02_007",
    "_Stand_Talk03_008",
    "_Stand_Talk04_009",
    "_Stand_Talk05_010",
    "_Stand_Talk_006",
    "_Stand_Thinking02_012",
};

struct NpcColorPreset
{
    _float4 HairColor = { 1.00f, 0.86f, 0.70f, 1.f };
    _float4 SkinColor = { 1.00f, 0.86f, 0.70f, 1.f };
    _float4 ClothColor = { 1.00f, 0.86f, 0.70f, 1.f };
    _float4 EyeColor = { 0.12f, 0.10f, 0.08f, 1.f }; 

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

    static void SetVariationColor_TryNames(CMaterial* material, _float4* color, const char* const* nameList, size_t nameCount)
    {
        if (!material || !color || !nameList || nameCount == 0) return;

        for (size_t nameIndex = 0; nameIndex < nameCount; ++nameIndex)
        {
            const char* instanceName = nameList[nameIndex];
            if (!instanceName) continue;

            auto materialInstance = material->Find_MaterialInstanceByName(instanceName);
            if (!materialInstance) continue;

            materialInstance->Set_Param("fVariationColor", { color, "float4", sizeof(_float4) });
        }
    }

public:
    void Randomize_Natural(_uint seedValue)
    {
        FastRng rngSkin;  rngSkin.Seed(MixSeed(seedValue, 0x1111u));
        FastRng rngHair;  rngHair.Seed(MixSeed(seedValue, 0x2222u));
        FastRng rngCloth; rngCloth.Seed(MixSeed(seedValue, 0x3333u));
        FastRng rngEye;   rngEye.Seed(MixSeed(seedValue, 0x4444u)); // ? 추가

        auto MakeColorFromHsv = [](_float hueDeg, _float sat, _float val) -> _float4
            {
                _float3 rgb = {};
                _float3 hsv = { hueDeg, sat, val };
                HsvToRgb(hsv, rgb);
                return _float4{ clamp(rgb.x,0.f,1.f), clamp(rgb.y,0.f,1.f), clamp(rgb.z,0.f,1.f), 1.f };
            };

        // --------------------
        // Skin (원래 로직 유지)
        // --------------------
        const _float skinPick = rngSkin.Next01();

        _float skinHueMin = 0.f, skinHueMax = 0.f;
        _float skinSatMin = 0.f, skinSatMax = 0.f;
        _float skinValMin = 0.f, skinValMax = 0.f;

        if (skinPick < 0.10f)
        {
            skinHueMin = 18.f;  skinHueMax = 40.f;
            skinSatMin = 0.06f; skinSatMax = 0.22f;
            skinValMin = 0.88f; skinValMax = 1.00f;
        }
        else if (skinPick < 0.35f)
        {
            skinHueMin = 18.f;  skinHueMax = 45.f;
            skinSatMin = 0.10f; skinSatMax = 0.30f;
            skinValMin = 0.78f; skinValMax = 0.98f;
        }
        else if (skinPick < 0.65f)
        {
            skinHueMin = 18.f;  skinHueMax = 50.f;
            skinSatMin = 0.14f; skinSatMax = 0.38f;
            skinValMin = 0.62f; skinValMax = 0.88f;
        }
        else if (skinPick < 0.85f)
        {
            skinHueMin = 16.f;  skinHueMax = 52.f;
            skinSatMin = 0.16f; skinSatMax = 0.44f;
            skinValMin = 0.48f; skinValMax = 0.78f;
        }
        else
        {
            skinHueMin = 14.f;  skinHueMax = 48.f;
            skinSatMin = 0.16f; skinSatMax = 0.50f;
            skinValMin = 0.28f; skinValMax = 0.62f;
        }

        const _float undertonePick = rngSkin.Next01();
        if (undertonePick < 0.40f)
        {
            skinHueMin = max(skinHueMin, 28.f);
            skinHueMax = min(skinHueMax, 55.f);
            skinSatMin = max(skinSatMin, 0.12f);
        }
        else if (undertonePick < 0.80f)
        {
        }
        else
        {
            skinHueMin = max(skinHueMin, 12.f);
            skinHueMax = min(skinHueMax, 36.f);
            skinSatMax = min(skinSatMax, 0.42f);
        }

        const _float hueBase = rngSkin.Range(skinHueMin, skinHueMax);
        const _float satBase = rngSkin.Range(skinSatMin, skinSatMax);
        const _float valBase = rngSkin.Range(skinValMin, skinValMax);

        _float4 baseSkin = MakeColorFromHsv(hueBase, satBase, valBase);

        SkinColor = ApplyHsvJitterConstrained(
            baseSkin, rngSkin,
            9.f, 0.07f, 0.10f,
            skinHueMin, skinHueMax,
            skinSatMin, skinSatMax,
            skinValMin, skinValMax,
            true);

        // --------------------
        // Hair (원래 로직 유지)
        // --------------------
        {
            const _float hairRoll = rngHair.Next01();

            _float hairHueMin = 0.f, hairHueMax = 0.f;
            _float hairSatMin = 0.f, hairSatMax = 0.f;
            _float hairValMin = 0.f, hairValMax = 0.f;

            if (hairRoll < 0.90f)
            {
                const _float naturalPick = rngHair.Next01();

                if (naturalPick < 0.30f)
                {
                    hairHueMin = 0.f;   hairHueMax = 60.f;
                    hairSatMin = 0.00f; hairSatMax = 0.25f;
                    hairValMin = 0.05f; hairValMax = 0.20f;
                }
                else if (naturalPick < 0.65f)
                {
                    hairHueMin = 10.f;  hairHueMax = 45.f;
                    hairSatMin = 0.20f; hairSatMax = 0.75f;
                    hairValMin = 0.12f; hairValMax = 0.55f;
                }
                else if (naturalPick < 0.85f)
                {
                    hairHueMin = 30.f;  hairHueMax = 60.f;
                    hairSatMin = 0.15f; hairSatMax = 0.65f;
                    hairValMin = 0.45f; hairValMax = 0.90f;
                }
                else if (naturalPick < 0.95f)
                {
                    hairHueMin = 5.f;   hairHueMax = 25.f;
                    hairSatMin = 0.25f; hairSatMax = 0.85f;
                    hairValMin = 0.20f; hairValMax = 0.70f;
                }
                else
                {
                    hairHueMin = 0.f;   hairHueMax = 359.999f;
                    hairSatMin = 0.00f; hairSatMax = 0.12f;
                    hairValMin = 0.45f; hairValMax = 0.95f;
                }

                const _float baseHue = rngHair.Range(hairHueMin, hairHueMax);
                const _float baseSat = rngHair.Range(hairSatMin, hairSatMax);
                const _float baseVal = rngHair.Range(hairValMin, hairValMax);
                _float4 baseHair = MakeColorFromHsv(baseHue, baseSat, baseVal);

                HairColor = ApplyHsvJitterConstrained(
                    baseHair, rngHair,
                    25.f, 0.16f, 0.16f,
                    hairHueMin, hairHueMax,
                    hairSatMin, hairSatMax,
                    hairValMin, hairValMax,
                    true);
            }
            else
            {
                const _float dyePick = rngHair.Next01();

                if (dyePick < 0.40f)
                {
                    hairHueMin = 200.f; hairHueMax = 245.f;
                }
                else if (dyePick < 0.75f)
                {
                    hairHueMin = 255.f; hairHueMax = 305.f;
                }
                else
                {
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
        // Cloth (원래 로직 유지)
        // --------------------
        {
            const _float clothPick = rngCloth.Next01();

            _float clothHueMin = 0.f, clothHueMax = 359.999f;
            _float clothSatMin = 0.f, clothSatMax = 0.f;
            _float clothValMin = 0.f, clothValMax = 0.f;

            if (clothPick < 0.55f)
            {
                clothSatMin = 0.10f; clothSatMax = 0.55f;
                clothValMin = 0.18f; clothValMax = 0.85f;
            }
            else if (clothPick < 0.80f)
            {
                clothSatMin = 0.35f; clothSatMax = 0.90f;
                clothValMin = 0.20f; clothValMax = 0.80f;
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
                22.f, 0.22f, 0.20f,
                clothHueMin, clothHueMax,
                clothSatMin, clothSatMax,
                clothValMin, clothValMax,
                true);
        }

        {
            const _float eyePick = rngEye.Next01();

            // Brown / Hazel / Green / Blue / Gray
            _float eyeHueMin = 0.f, eyeHueMax = 0.f;
            _float eyeSatMin = 0.10f, eyeSatMax = 0.55f;
            _float eyeValMin = 0.12f, eyeValMax = 0.55f;

            if (eyePick < 0.55f) { eyeHueMin = 15.f;  eyeHueMax = 45.f; }   // brown
            else if (eyePick < 0.70f) { eyeHueMin = 45.f;  eyeHueMax = 70.f; }   // hazel/amber
            else if (eyePick < 0.82f) { eyeHueMin = 85.f;  eyeHueMax = 150.f; }  // green
            else if (eyePick < 0.94f) { eyeHueMin = 195.f; eyeHueMax = 240.f; }  // blue
            else { eyeHueMin = 0.f;   eyeHueMax = 359.9f; eyeSatMin = 0.f; eyeSatMax = 0.12f; eyeValMin = 0.20f; eyeValMax = 0.70f; } // gray

            const _float baseHue = rngEye.Range(eyeHueMin, eyeHueMax);
            const _float baseSat = rngEye.Range(eyeSatMin, eyeSatMax);
            const _float baseVal = rngEye.Range(eyeValMin, eyeValMax);

            _float4 baseEye = MakeColorFromHsv(baseHue, baseSat, baseVal);

            EyeColor = ApplyHsvJitterConstrained(
                baseEye, rngEye,
                8.f, 0.10f, 0.10f,
                eyeHueMin, eyeHueMax,
                eyeSatMin, eyeSatMax,
                eyeValMin, eyeValMax,
                true);
        }
    }

    void LinkMaterial(CMaterial* material)
    {
        const char* const bodyNames[] = { "Body", "Torso", "Skin_Body", "SkinBody" };
        const char* const faceNames[] = { "Face", "Head", "Skin_Head", "SkinHead", "Face_01", "Head_01" };

        SetVariationColor_TryNames(material, &SkinColor, bodyNames, _countof(bodyNames));
        SetVariationColor_TryNames(material, &SkinColor, faceNames, _countof(faceNames)); 

        SetVariationColor(material, "Hair", &HairColor);
        SetVariationColor(material, "Ebr", &HairColor);

        SetVariationColor(material, "Eye", &EyeColor);
        SetVariationColor(material, "Cloth", &ClothColor);
    }

    void Render_Colors()
    {
        ImGui::DragFloat4("Hair", &HairColor.x, 0.01f, 0.f, 1.f);
        ImGui::DragFloat4("Skin", &SkinColor.x, 0.01f, 0.f, 1.f);
        ImGui::DragFloat4("Eye", &EyeColor.x, 0.01f, 0.f, 1.f);
        ImGui::DragFloat4("Cloth", &ClothColor.x, 0.01f, 0.f, 1.f);
    }
};

struct NpcModelPreset
{
private:
    _bool isMale = false;

public:
    void RandomizeModel(_bool isWalker, CSkeletalModel* pModel, CMaterial* pMaterial, CAnimator3D* pAnimator)
    {
        isMale = Helper::Get_Random_Bool();

        string genderWord = isMale ? "Male" : "Female";
        _int variation = Helper::Get_Random_Int(1, isMale ? 6 : 5);

        string model = genderWord + "0" + to_string(variation) + ".model";
        string material = genderWord + "0" + to_string(variation) + ".mat";
        string meta = genderWord + "0" + to_string(variation) + "_Meta.json";

        HRESULT Modelhr =pModel->Link_Model("MainCity_Level", model);
        HRESULT MatHr = pMaterial->Link_Material("MainCity_Level", material);

        if (FAILED(Modelhr) || FAILED(MatHr))
            return;

        pModel->Hide_MehsByName("Package");
        pModel->Hide_MehsByName("Static");
        pAnimator->LinkAnimate_Model("MainCity_Level", model);
        pAnimator->Link_MetaData("MainCity_Level", meta);

        if (isWalker)
        {
            string walk = genderWord + "0" + to_string(variation) + (isMale ? "_Walk_Normal02_015" : "_Walk_Normal_020");
            pAnimator->Set_Animation(walk).Loop(true).Apply();
        }
        else
        {
            if (isMale)
            {
                const _int animIndex = Helper::Get_Random_Int(0, (_int)NPC_MALE_ANIM.size() - 1);
                string idleAnim = genderWord + "0" + to_string(variation) + NPC_MALE_ANIM[animIndex];
                pAnimator->Set_Animation(idleAnim).Loop(true).Apply();
            }
            else
            {
                const _int animIndex = Helper::Get_Random_Int(0, (_int)NPC_FEMALE_ANIM.size() - 1);
                string idleAnim = genderWord + "0" + to_string(variation) + NPC_FEMALE_ANIM[animIndex];
                pAnimator->Set_Animation(idleAnim).Loop(true).Apply();
            }
        }
    }
};