#pragma once

NS_BEGIN(Engine)

enum class EaseType
{
	None, InOutSine, OutCubic, InOutCubic, OutSine, InOutQuad, InSine, InCubic, InQuad, InCirc, InOutCirc, OutCirc, OutQuad,
	InQuart, InQuint, InOutQuart, OutQuart, InOutQuint, OutQuint, InOutExpo, OutExpo, InExpo, OutBack, InOutBack, InBack,
	OutElastic, InOutElastic, InElastic, OutBounce, InOutBounce, InBounce,
};

namespace Math
{
	//Float 보간 함수
	ENGINE_DLL _float      Lerp(_float x, _float y, _float t);

	ENGINE_DLL const char* GetEaseLabel(EaseType v);
	ENGINE_DLL _float      ApplyEase(EaseType type, _float t);

	// Easing 보간 (Camera 추천 순위 1~30)  (중복 없음)
	// [A] 범용/안정 (일반 이동/패닝/팔로우 기본값)
	ENGINE_DLL _float EaseInOutSine(_float t);    // 1.  EaseInOutSine:  가장 자연스러운 기본 S-curve(범용/안정)
	ENGINE_DLL _float EaseOutCubic(_float t);     // 2.  EaseOutCubic:   빠른 출발 + 부드러운 감속(이동 기본값)
	ENGINE_DLL _float EaseInOutCubic(_float t);   // 3.  EaseInOutCubic: S-curve가 더 힘 있게 느껴짐(전투/연출에 자주)
	ENGINE_DLL _float EaseOutSine(_float t);      // 4.  EaseOutSine:    가벼운 감속(짧은 전환/미세 조정)
	ENGINE_DLL _float EaseInOutQuad(_float t);    // 5.  EaseInOutQuad:  반응이 직관적인 S-curve(툴/편집기에도 좋음)

	// [B] 부드러운 출발 (컷 시작 튐 제거/진입 숨기기)
	ENGINE_DLL _float EaseInSine(_float t);       // 6.  EaseInSine:     초반이 아주 부드러움(‘툭’ 시작 제거)
	ENGINE_DLL _float EaseInCubic(_float t);      // 7.  EaseInCubic:    초반 움직임을 숨기고 뒤에서 확 가속(컷 진입 숨김)
	ENGINE_DLL _float EaseInQuad(_float t);       // 8.  EaseInQuad:     InCubic보다 약한 뜸 들이기(작은 이동에 적당)
	ENGINE_DLL _float EaseInCirc(_float t);       // 9.  EaseInCirc:     초반이 더 오래 느림(정지/무게감 강조)

	// [C] 단단한 정착/멈춤 (피사체에 “착” 붙는 느낌)
	ENGINE_DLL _float EaseInOutCirc(_float t);    // 10. EaseInOutCirc:  시작/끝이 더 단단히 부드러움(정착감 강조)
	ENGINE_DLL _float EaseOutCirc(_float t);      // 11. EaseOutCirc:    끝에서 확실히 멈춤(락온/정착, 과하면 무거움)
	ENGINE_DLL _float EaseOutQuad(_float t);      // 12. EaseOutQuad:    단순하고 깔끔한 감속(짧은 전환에 무난)

	// [D] 더 강한 ‘힘’ (전투/연출, 가속감/감속감 강화)
	ENGINE_DLL _float EaseInQuart(_float t);      // 13. EaseInQuart:    초반을 더 오래 숨기고 후반에 강하게 가속(진입/당김 연출)
	ENGINE_DLL _float EaseInQuint(_float t);      // 14. EaseInQuint:    InQuart보다 더 극단(후반 급가속). 과하면 멀미/끊김 느낌 주의
	ENGINE_DLL _float EaseInOutQuart(_float t);   // 15. EaseInOutQuart: 강한 S-curve(짧고 임팩트 있는 컷)
	ENGINE_DLL _float EaseOutQuart(_float t);     // 16. EaseOutQuart:   강하게 멈추는 감속(급정지 연출, 멀미 주의)
	ENGINE_DLL _float EaseInOutQuint(_float t);   // 17. EaseInOutQuint: Quart보다 더 강한 S-curve(연출용)
	ENGINE_DLL _float EaseOutQuint(_float t);     // 18. EaseOutQuint:   강한 감속(빠른 접근 후 꽂히는 정착)

	// [E] 긴 거리/극단 리듬 (일상 카메라엔 과하고 “쇼트/연출”용)
	ENGINE_DLL _float EaseInOutExpo(_float t);    // 19. EaseInOutExpo:  초반 거의 정지→중간 매우 빠름→끝 부드러움(큰 거리 연출)
	ENGINE_DLL _float EaseOutExpo(_float t);      // 20. EaseOutExpo:    빠르게 도달 후 부드럽게 마감(큰 거리/줌에 가끔)
	ENGINE_DLL _float EaseInExpo(_float t);       // 21. EaseInExpo:     초반을 거의 숨김→후반 급가속(컷 진입 연출 전용)

	// [F] 강조/오버슈트 (손맛/강조, 과하면 멀미)
	ENGINE_DLL _float EaseOutBack(_float t);      // 22. EaseOutBack:    목표를 살짝 지나쳤다 복귀(줌/락온 강조)
	ENGINE_DLL _float EaseInOutBack(_float t);    // 23. EaseInOutBack:  시작/끝 모두 오버슈트(강조 연출, 사용 빈도 낮음)
	ENGINE_DLL _float EaseInBack(_float t);       // 24. EaseInBack:     출발이 뒤로 살짝 당겼다 출발(특수 연출)

	// [G] 바운스/엘라스틱 (카메라엔 거의 “특수 상황”)
	ENGINE_DLL _float EaseOutElastic(_float t);   // 25. EaseOutElastic:   끝에서 스프링처럼 흔들림(연출/개그, 멀미 주의)
	ENGINE_DLL _float EaseInOutElastic(_float t); // 26. EaseInOutElastic: 전체 구간 스프링(카메라엔 거의 비추천)
	ENGINE_DLL _float EaseInElastic(_float t);    // 27. EaseInElastic:    시작부터 튕김(특수 연출용)

	ENGINE_DLL _float EaseOutBounce(_float t);    // 28. EaseOutBounce:   끝에서 통통 튐(UI/오브젝트용, 카메라는 드묾)
	ENGINE_DLL _float EaseInOutBounce(_float t);  // 29. EaseInOutBounce: 중간 포함 바운스(카메라엔 거의 비추천)
	ENGINE_DLL _float EaseInBounce(_float t);     // 30. EaseInBounce:    시작이 바운스(특수 연출)
}

NS_END