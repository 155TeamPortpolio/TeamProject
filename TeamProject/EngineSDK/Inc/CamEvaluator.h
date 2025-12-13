#pragma once

#include "ICamEvaluator.h"

NS_BEGIN(Engine)

class ENGINE_DLL CamEvaluator
{
public:
	// 이 시퀀스를 평가할 준비를 끝내는 초기화 단계, 키 데이터를 Eval용 구조로 변환/준비하는 단계
	bool    Build(const CamSequenceDesc& seqDesc); 
	// 현재 재생 시간(playTime)로 Once/Loop/PingPong 규칙에 맞게 실제 평가 시간 t를 만듬. 매 프레임 호출되는 '현재 시점의 카메라 포즈 계산기' 역할.
	Campose Evaluate(float playTime) const;       

	void SetPosEvaluator(unique_ptr<ICamPosEvaluator> _posEval) { posEval = move(_posEval); }
	void SetRotEvaluator(unique_ptr<ICamRotEvaluator> _rotEval) { rotEval = move(_rotEval); }
	void SetFovEvaluator(unique_ptr<ICamFovEvaluator> _fovEval) { fovEval = move(_fovEval); }

	_float GetDuration() const { return duration; }

private:
	// 재생 시간(playTime)을, 시퀀스 길이(duration)안에서 실제로 평가할 시간 t로 바꿔주는 함수. Evaluate()가 보간할 때 쓰는 "정규화/재매핑된 시간"을 만든다.
	// 보간 자체(Linear/Spline/Slerp)는 전혀 안 건드리고 "어떤 시간으로 평가할지"만 결정
	_float MapTime(float playTime) const;

private:
	const CamSequenceDesc* seqDesc{};
	_float duration{};

	unique_ptr<ICamPosEvaluator> posEval;
	unique_ptr<ICamRotEvaluator> rotEval;
	unique_ptr<ICamFovEvaluator> fovEval;
};

NS_END