#pragma once
#include "pch.h"

class EffectFlow
{
    using FnOnce = function<void()>;
    using FnUpdate = function<void(_float)>;     
    using FnCond = function<_bool(void)>;
    enum class StepType : _uint
    {
        Once,
        Wait,
        Tween,
        WaitUntil
    };
    struct Step
    {
        StepType type = StepType::Once;
        _float duration = 0.0f;   
        _float elapsed = 0.0f;
        _bool fired = false;

        FnOnce onOnce;           
        FnUpdate onTween;     
        FnCond cond;           
    };

    struct Sequence
    {
        vector<Step> steps;
        size_t index = 0;
       _bool done = false;

        void Reset()
        {
            index = 0;
            done = false;
            for (auto& step : steps)
            {
                step.elapsed = 0.0f;
                step.fired = false;
            }
        }
    };
public:
    size_t BeginSequence()
    {
        sequences.push_back(Sequence{});
        return sequences.size() - 1;
    }

    void EndSequence(size_t seqId)
    {
        if (seqId >= sequences.size()) return;
        sequences[seqId].Reset();
    }

    void AddOnce(size_t seqId, FnOnce fn)
    {
        Step step;
        step.type = StepType::Once;
        step.onOnce = move(fn);
        sequences[seqId].steps.push_back(move(step));
    }

    void AddWait(size_t seqId, _float seconds)
    {
        Step step;
        step.type = StepType::Wait;
        step.duration = (seconds < 0.0f) ? 0.0f : seconds;
        sequences[seqId].steps.push_back(move(step));
    }

    void AddTween(size_t seqId, _float seconds, FnUpdate onUpdate)
    {
        Step step;
        step.type = StepType::Tween;
        step.duration = (seconds < 0.0f) ? 0.0f : seconds;
        step.onTween = move(onUpdate);
        sequences[seqId].steps.push_back(move(step));
    }

    void AddWaitUntil(size_t seqId, FnCond cond)
    {
        Step step;
        step.type = StepType::WaitUntil;
        step.cond = move(cond);
        sequences[seqId].steps.push_back(move(step));
    }

    void Start()
    {
        running = true;
        for (auto& seq : sequences)
            seq.Reset();
    }

    void Stop()
    {
        running = false;
    }

    void Tick(_float dt)
    {
        if (!running) return;
        if (dt < 0.0f) dt = 0.0f;

        bool anyAlive = false;

        for (auto& seq : sequences)
        {
            if (seq.done) continue;

            anyAlive = true;
            TickSequence(seq, dt);
        }

        if (!anyAlive)
            running = false;
    }

    bool IsDoneAll() const
    {
        if (sequences.empty()) return true;
        for (const auto& seq : sequences)
            if (!seq.done) return false;
        return true;
    }

    bool IsRunning() const { return running; }

private:
    void TickSequence(Sequence& seq, float dt)
    {
        if (seq.index >= seq.steps.size())
        {
            seq.done = true;
            return;
        }

        float remainDt = dt;

        while (remainDt > 0.0f && !seq.done)
        {
            if (seq.index >= seq.steps.size())
            {
                seq.done = true;
                break;
            }

            Step& step = seq.steps[seq.index];

            switch (step.type)
            {
            case StepType::Once:
            {
                if (!step.fired)
                {
                    step.fired = true;
                    if (step.onOnce) step.onOnce();
                }
                seq.index++;
                continue; // dt 소모 없이 다음 step
            }

            case StepType::Wait:
            {
                float need = step.duration - step.elapsed;
                if (need <= 0.0f)
                {
                    seq.index++;
                    continue;
                }

                float use = (remainDt < need) ? remainDt : need;
                step.elapsed += use;
                remainDt -= use;

                if (step.elapsed >= step.duration)
                {
                    seq.index++;
                    continue;
                }
                return; // 아직 대기중
            }

            case StepType::Tween:
            {
                float need = step.duration - step.elapsed;
                if (step.duration <= 0.0f)
                {
                    if (step.onTween) step.onTween(1.0f);
                    seq.index++;
                    continue;
                }

                float use = (remainDt < need) ? remainDt : need;
                step.elapsed += use;
                remainDt -= use;

                float t = step.elapsed / step.duration;
                if (t < 0.0f) t = 0.0f;
                if (t > 1.0f) t = 1.0f;

                if (step.onTween) step.onTween(t);

                if (step.elapsed >= step.duration)
                {
                    seq.index++;
                    continue;
                }
                return; // tween 진행중
            }

            case StepType::WaitUntil:
            {
                bool ok = false;
                if (step.cond) ok = step.cond();

                if (ok)
                {
                    seq.index++;
                    continue;
                }
                return; // 조건 충족까지 대기
            }

            default:
                seq.index++;
                continue;
            }
        }
    }

private:
    vector<Sequence> sequences;
    _bool running = false;
};
