#pragma once
#include "Base.h"
class CBattleFXFlow :
	public CBase
{
	/*연출 단계 콜백 ->true: 계속, false: 다음 step */
using StepFunc = function<bool(float)>; 
private:
	CBattleFXFlow();
	~CBattleFXFlow()DEFAULT;

public:
	void Start(function<void()> onEnd = {});
	void Clear(_bool callOnEnd = true);
	void Update(_float dt);

public:
	_bool IsRunning() const { return m_isRunning; }
	void AddWait(_float duration);
	void AddCall(function<void()> fn);

private:
	_bool m_isRunning = false;

	size_t m_stepIndex = 0;
	_float m_timeInStep = 0.f;

	function<void()> m_onEnd;
	vector<StepFunc> m_steps;

public:
	static CBattleFXFlow* Create();
	void Free();
};

