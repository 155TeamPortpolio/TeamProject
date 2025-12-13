#pragma once

#include "ICamEvaluator.h"

NS_BEGIN(Engine)

class CamEvaluator
{
public:
	bool Build(const CamSequenceDesc& seqDesc);

	Campose Evaluate(float playTime) const;

	void SetPosEvaluator(unique_ptr<ICamPosEvaluator> _posEval) { posEval = move(_posEval); }
	void SetRotEvaluator(unique_ptr<ICamRotEvaluator> _rotEval) { rotEval = move(_rotEval); }
	void SetFovEvaluator(unique_ptr<ICamFovEvaluator> _fovEval) { fovEval = move(_fovEval); }

	_float GetDuration() const { return duration; }

private:
	_float MapTime(float playTime) const;

private:
	const CamSequenceDesc* seqDesc{};
	_float duration{};

	unique_ptr<ICamPosEvaluator> posEval;
	unique_ptr<ICamRotEvaluator> rotEval;
	unique_ptr<ICamFovEvaluator> fovEval;
};

NS_END