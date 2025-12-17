#pragma once

NS_BEGIN(CameraTool)

// "어떤 상황/이벤트 때문에 이 카메라를 쓰는지"
// 요청이 "왜" 올라왓는지 분류 (규칙/디버그/로깅/정책의 핵심)
// CamDirector 규칙 테이블의 Key가 되는 값.
enum class CamRequestType 
{
	GamePlay_Field, GamePlay_Battle
};

enum class CamBlendMode // 전환 방식 
{

};

enum class CamRequestEndRule // 요청의 종료 방식(자동 종료/조건 종료)
{

};
//enum class CamApplyMask // 비트플래그 “Modifier(흔들림만 위에 얹기)”로 갈 때 핵심
//{
//
//};

NS_END