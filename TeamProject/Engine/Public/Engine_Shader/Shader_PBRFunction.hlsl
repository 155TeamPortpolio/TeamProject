#ifndef __SHADER_PBRFUNCTION_HLSL__
#define __SHADER_PBRFUNCTION_HLSL__

// material
// 표면 거칠기 표현 (roughness가 낮으면 날카로운 반사, 높으면 넓은 반사)
float DistributionGGX(float3 normal, float3 halfdir, float roughness)
{
    //halfdir - 하프 벡터 (시선벡터와 라이트벡터의 중간)
    //roughness - 표면 거칠기
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float NdotH = saturate(dot(normal, halfdir)); //법선 하프벡터 내적
    
    float num = alpha2;
    float denom = 3.14 * pow((pow(NdotH, 2) * (alpha2 - 1.0f) + 1.0f), 2);
   
    return num / denom; //높을수록 날카로운 반사 //GGX 분포값
}

// 마이크로 패싯 (위에 말한 거칠기임) 간의 그림자와 마스킹
// 표면의 미세한 굴곡으로 빛이 차폐되는 현상 시뮬레이션

float GeometrySchlickGGX(float NdotV, float roughness)
{
    //NdotV - 법선 시선벡터 내적
    float r = roughness + 1.f;
    float k = (r * r) / 8.f; 
    
    float NdotV_clamped = max(NdotV, 0.001);
    float denom = NdotV_clamped * (1.f - k) + k;
    
    return NdotV_clamped / denom;
}

float GeometrySmith(float3 normal, float3 view, float3 light, float roughness)
{
    //normal  - 표면 법선
    //view - 시선 벡터
    //light = light 벡터
    
    float NdotV = max(dot(normal, view), 0.001);
    float NdotL = max(dot(normal, light), 0.001);

    float GGX1 = GeometrySchlickGGX(NdotL, roughness);
    float GGX2 = GeometrySchlickGGX(NdotV, roughness);
    
    return GGX1 * GGX2;
}


//각도에 따른 반사율 계산 
float3 FresnelSchlick(float HdotV, float3 F0)
{
    //F0 - 수직 입사 반사율 (0도 각도에서의 기본 반사율)
    //시선벡터와 하프벡터의 내적 - HdotV
    
    //슐릭 근사식
    return F0 + (1.0 - F0) * pow(clamp(1.0 - HdotV, 0.0, 1.0), 5.0);
}


//재질 타입에 따른 기본 반사율
float3 CalculateF0(float3 albedo, float metalic)
{
    //비금속
    float3 defaultrelfect = float3(0.04, 0.04, 0.04);
    
    return lerp(defaultrelfect, albedo, metalic);
}

// 에너지 보존 법칙 적용 - 반사광과 굴절광의 비율을 물리적으로 계산하는 함수
void CalculateEnergyConservation(float3 fresenel, float metalic, out float3 kSpecular, out float3 kDiffuse)
{
    //fresenel - 프레넬 반사율
    //kSpecular - 반사된 빛의 비율
    //kDiffuse = 굴절되어 내부로 들어간 빛의 비율 (Diffuse 반사)
    
    kSpecular = fresenel;
    
    //에너지 보존 법칙: 입사광 = 반사광 + 굴절광
    kDiffuse = float3(1.0f, 1.0f, 1.0f) - kSpecular;
    //금속 = 굴절광 X    
    kDiffuse = kDiffuse * (1.0 - metalic);
}

float3 CalculatePBR(float3 albedo, float3 normal, float metalic, float roughness, float ambientocclusion,
float3 view, float3 light, float3 lightcolor, float lightIntensity)
{
    float3 N = normalize(normal);
    float3 V = normalize(view);
    float3 L = normalize(light);
    float3 H = normalize(V + L);
    
    float NdotL = max(dot(N, L), 0.0);
    
    float NdotV = max(dot(N, V), 0.001);
    float VdotH = max(dot(V, H), 0.001);
    
    float3 F0 = CalculateF0(albedo, metalic);
    float D = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    float3 F = FresnelSchlick(VdotH, F0);
    
    float3 numerator = D * F * G;
    float denominator = max(4.f * NdotV * NdotL, 0.001);
    float3 specular = numerator / denominator;
    
    float3 kSpecular, kDiffuse;
    CalculateEnergyConservation(F, metalic, kSpecular, kDiffuse);
    float3 diffuse = kDiffuse * albedo / 3.14;
    
    float3 BRDF = diffuse + specular;
    
    return  BRDF * lightcolor * lightIntensity * NdotL;
}

float3 CalculateDirectionalLight(float3 albedo, float3 normal, float metalic, float roughness, float ambientocclusion,
float3 view, float3 light, float3 lightcolor, float lightIntensity, float shadowFactor)
{
    float3 Luminance = CalculatePBR(albedo, normal, metalic, roughness, ambientocclusion, view, light,
                                  lightcolor, lightIntensity);
    
    return Luminance* shadowFactor;
}

float3 CalculatePointLight(float3 albedo, float3 normal, float metalic, float roughness, float ambientocclusion, float3 worldPos,
float3 view, float3 light, float3 lightcolor, float lightIntensity, float3 lightposition, float lightrange, float shadowFactor)
{
    float3 lightdir = normalize(lightposition - worldPos);
    float distance = length(lightposition - worldPos);
    lightdir = lightdir / distance;
    
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
    float rangeFactor = max(0.0, 1.0 - (distance / lightrange));
    attenuation *= rangeFactor * rangeFactor;
    
    float3 Luminance = CalculatePBR(albedo, normal, metalic, roughness, ambientocclusion, view, lightdir,
                                  lightcolor, lightIntensity * attenuation);
    
    return Luminance * shadowFactor;
}


/*
=== PBR 핵심 개념 요약 ===

벡터 의미:
- N: Surface Normal (표면 법선) - 표면이 향하는 방향
- V: View Direction (시선 방향) - 픽셀에서 카메라로 향하는 방향  
- L: Light Direction (라이트 방향) - 픽셀에서 라이트로 향하는 방향
- H: Half Vector (하프 벡터) - V와 L의 중간, 완전 반사 방향

Cook-Torrance BRDF:
- D: Normal Distribution - 마이크로패싯 분포 (거칠기 효과)
- G: Geometry Function - 자기차폐 효과 (미세한 그림자)
- F: Fresnel - 각도별 반사율 (프레넬 효과)

물리적 정확성:
- 에너지 보존: kS + kD = 1 (반사 + 굴절 = 100%)
- 금속 vs 비금속: 서로 다른 반사 특성
- 실제 광학 법칙 기반 계산

성능 최적화:
- Deferred Rendering으로 재질 정보 재사용
- G-Buffer에 필요한 데이터만 저장
- 라이트별 계산은 동일한 재질 데이터로 수행
*/
#endif