#include "pch.h"
#include "DebugCamTrace.h"
#include "GameInstance.h"
#include "Camera.h"

namespace
{
    float QuatDot(const Quaternion& a, const Quaternion& b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    }

    float RotDeltaDeg(const Quaternion& a, const Quaternion& b)
    {
        float d = QuatDot(a, b);
        if (d < 0.f) d = -d;
        d = clamp(d, 0.f, 1.f);
        return XMConvertToDegrees(2.f * acosf(d));
    }

    void CaptureCam(CCamera* cam, Vector3& outPos, Quaternion& outRot)
    {
        auto tf = cam->Get_Owner()->Get_Component<CTransform>();
        Matrix world(tf->Get_WorldMatrix());

        Vector3 scale{}, trans{};
        Quaternion rot = Quaternion::Identity;

        world.Decompose(scale, rot, trans);
        rot.Normalize();

        Vector3 off = cam->Get_ViewOffset();
        trans += off;

        outPos = trans;
        outRot = rot;
    }

    void CaptureOutput(Vector3& outPos, Quaternion& outRot)
    {
        Matrix world = *CAM->Get_InversedViewMatrix();

        Vector3 scale{}, trans{};
        Quaternion rot = Quaternion::Identity;

        world.Decompose(scale, rot, trans);
        rot.Normalize();

        outPos = trans;
        outRot = rot;
    }

    void PrintLine(const char* line)
    {
        OutputDebugStringA(line);
    }

    void PrintCamLine(const char* tag, const Vector3& p, const Quaternion& q)
    {
        char buf[512];
        sprintf_s(buf, "%s pos(%.3f %.3f %.3f) rot(%.5f %.5f %.5f %.5f)\n",
            tag, p.x, p.y, p.z, q.x, q.y, q.z, q.w);
        PrintLine(buf);
    }

    struct LastState
    {
        _bool has = false;

        Vector3 ap{};
        Quaternion aq = Quaternion::Identity;

        Vector3 bp{};
        Quaternion bq = Quaternion::Identity;

        Vector3 op{};
        Quaternion oq = Quaternion::Identity;
    };

    LastState last{};

    _bool Changed(const Vector3& p0, const Quaternion& q0, const Vector3& p1, const Quaternion& q1, float posEps, float rotEpsDeg)
    {
        const float dp = (p0 - p1).Length();
        const float dr = RotDeltaDeg(q0, q1);
        return dp > posEps || dr > rotEpsDeg;
    }
}

void DebugCamTrace::Trace_EveryFrame(CCamera* a, const char* aTag, CCamera* b, const char* bTag)
{
    if (!a || !b) return;

    Vector3 ap{}, bp{}, op{};
    Quaternion aq = Quaternion::Identity;
    Quaternion bq = Quaternion::Identity;
    Quaternion oq = Quaternion::Identity;

    CaptureCam(a, ap, aq);
    CaptureCam(b, bp, bq);
    CaptureOutput(op, oq);

    const float posEps = 0.0005f;
    const float rotEpsDeg = 0.02f;

    _bool needPrint = false;

    if (!last.has) needPrint = true;
    else
    {
        if (Changed(last.ap, last.aq, ap, aq, posEps, rotEpsDeg)) needPrint = true;
        if (Changed(last.bp, last.bq, bp, bq, posEps, rotEpsDeg)) needPrint = true;
        if (Changed(last.op, last.oq, op, oq, posEps, rotEpsDeg)) needPrint = true;
    }

    last.has = true;
    last.ap = ap;
    last.aq = aq;
    last.bp = bp;
    last.bq = bq;
    last.op = op;
    last.oq = oq;

    if (!needPrint) return;

    PrintLine("\n[CamTrace CHANGED]\n");
    PrintCamLine(aTag, ap, aq);
    PrintCamLine(bTag, bp, bq);
    PrintCamLine("OUT", op, oq);

    char d0[256];
    sprintf_s(d0, "d(A,B)   pos=%.6f rot=%.6fdeg\n", (ap - bp).Length(), RotDeltaDeg(aq, bq));
    PrintLine(d0);

    char d1[256];
    sprintf_s(d1, "d(A,OUT) pos=%.6f rot=%.6fdeg\n", (ap - op).Length(), RotDeltaDeg(aq, oq));
    PrintLine(d1);

    char d2[256];
    sprintf_s(d2, "d(B,OUT) pos=%.6f rot=%.6fdeg\n", (bp - op).Length(), RotDeltaDeg(bq, oq));
    PrintLine(d2);
}
