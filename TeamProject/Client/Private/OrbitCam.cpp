#include "pch.h"
#include "OrbitCam.h"
#include "GameInstance.h"
#include "CharacterController.h"
#include "Character.h"
#include "GUIUtil.h"
#include "Helper_Func.h"
#include "EventListener.h"

void COrbitCam::Awake()
{
    auto cc = Get_Component<CCharacterController>();

    cc->Resize(0.2f, 0.2f);
    cc->Set_GravityEnabled(false);
    cc->Set_StepOffset(0.f);
    cc->Set_SlopeLimit(89.f);
    cc->Set_MinMoveDist(0.01f);
    cc->Set_ContactOffset(0.001f);
    cc->Set_RestOffset(0.f);
}

HRESULT COrbitCam::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    Add_Component<CCharacterController>();
    Add_Component<CEventListener>();

    pose.targetRotDeg = Vector2(0.f, profile.startPitchDeg);
    pose.curRotDeg = pose.targetRotDeg;

    pose.targetDist = profile.startDistance;
    pose.curDist = pose.targetDist;

    pose.targetPivot = Vector3::Zero;
    pose.curPivot = pose.targetPivot;

    pose.pivotOverrideOffset = Vector3::Zero;

    ClampTargets();
    return S_OK;
}

HRESULT COrbitCam::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);
    auto event = Get_Component<CEventListener>();

    event->Add_Listener<TARGET_LOCK_DESC>([&](TARGET_LOCK_DESC desc)
        {
            if (!desc.tHandle.isValid()) return;
            if (desc.bLock == true) SetLockOn(desc.tHandle);
            else ClearLockOn();
        });

    return S_OK;
}

void COrbitCam::SetTarget(OBJECT_HANDLE handle)
{
    autoYawHoldTimer = profile.autoYawFollowDelay;
    hasPrevTargetFoot = false;

    const float keepTargetDist = pose.targetDist;

    if (!targetHandle.isValid())
    {
        targetHandle = handle;
        targetSwitch = {};
        pose.pivotOverrideOffset = Vector3::Zero;

        auto obj = ObjectManager()->Request_Object(handle);
        SetTargetFrontView(obj, profile.startDistance, profile.startPitchDeg, profile.startHeightOffset);
        return;
    }

    if (handle.hObjID == targetHandle.hObjID) return;

    const Vector3 holdPivotWorld = pose.curPivot;

    targetHandle = handle;

    targetSwitch.active = true;
    targetSwitch.elapsed = 0.f;
    targetSwitch.holdPivotWorld = holdPivotWorld;

    pose.targetRotDeg = pose.curRotDeg;

    pose.targetDist = clamp(keepTargetDist, profile.minDist, profile.maxDist);
    pose.curDist = clamp(pose.curDist, profile.minDist, profile.maxDist);

    ClampTargets();
}

void COrbitCam::ClearTarget()
{
    targetHandle.Reset();
    lockOnCtrl.ForceClear();
}

void COrbitCam::SetLockOn(OBJECT_HANDLE handle)
{
    if (!handle.isValid()) return;

    if (!lockOnCtrl.IsActive())
    {
        lockOnCtrl.Enter(handle, pose.targetDist, profile);
        autoYawHoldTimer = 0.f;
        return;
    }

    if (handle.hObjID == lockOnCtrl.GetHandle().hObjID) return;

    lockOnCtrl.Switch(handle);
}

void COrbitCam::ClearLockOn()
{
    if (!lockOnCtrl.IsActiveOrBlending()) return;

    lockOnCtrl.BeginExit(profile);
    autoYawHoldTimer = profile.autoYawFollowDelay;
}

void COrbitCam::SyncFromCurTransform()
{
    const Vector3 pivot = GetPivotTargetPos();
    pose.targetPivot = pivot;
    pose.curPivot = pivot;

    auto cc = Get_Component<CCharacterController>();
    const PxExtendedVec3& c = cc->Get_Controller()->getPosition();
    const Vector3 camPos((float)c.x, (float)c.y, (float)c.z);

    Vector3 toPivot = pivot - camPos;
    const float rawDist = toPivot.Length();
    toPivot /= rawDist;

    const float yawRad = atan2f(toPivot.x, toPivot.z);
    const float pitchRad = asinf(clamp(-toPivot.y, -1.f, 1.f));

    pose.curRotDeg.x = XMConvertToDegrees(yawRad);
    pose.curRotDeg.y = XMConvertToDegrees(pitchRad);
    pose.targetRotDeg = pose.curRotDeg;

    pose.curDist = rawDist;
    pose.targetDist = rawDist;

    ClampTargets();

    m_pTransform->Set_WorldPos(XMVectorSet((float)c.x, (float)c.y, (float)c.z, 1.f));
    m_pTransform->LookAt(Vector4(pivot.x, pivot.y, pivot.z, 1.f));
}

void COrbitCam::SetTargetFrontView(CGameObject* obj, float distance, float pitchDeg, float heightOffset)
{
    targetHandle = obj->Get_Handle();

    const Vector3 pivot = GetPivotTargetPos();
    pose.targetPivot = pivot;
    pose.curPivot = pivot;

    auto targetTf = obj->Get_Component<CTransform>();
    Vector3 forward = targetTf->Dir(STATE::LOOK);
    forward.y = 0.f;
    forward.Normalize();

    const float horizontalDist = sqrtf(distance * distance - heightOffset * heightOffset);

    Vector3 camPos = pivot + forward * horizontalDist;
    camPos.y += heightOffset;

    auto cc = Get_Component<CCharacterController>();
    cc->Set_Position(XMVectorSet(camPos.x, camPos.y, camPos.z, 1.f));

    const PxExtendedVec3& c = cc->Get_Controller()->getPosition();
    const Vector3 curPos((float)c.x, (float)c.y, (float)c.z);

    Vector3 toPivot = pivot - curPos;
    float dist = toPivot.Length();

    pose.curDist = dist;
    pose.targetDist = dist;

    toPivot /= dist;

    const float yawRad = atan2f(toPivot.x, toPivot.z);

    pose.curRotDeg.x = XMConvertToDegrees(yawRad);
    pose.curRotDeg.y = pitchDeg;

    pose.targetRotDeg = pose.curRotDeg;

    ClampTargets();

    m_pTransform->Set_WorldPos(XMVectorSet((float)c.x, (float)c.y, (float)c.z, 1.f));
    m_pTransform->LookAt(Vector4(pivot.x, pivot.y, pivot.z, 1.f));
}

void COrbitCam::SnapFromCamPose(const Vector3& camPos, const Quaternion& camRot)
{
    auto cc = Get_Component<CCharacterController>();
    cc->Set_Position(XMVectorSet(camPos.x, camPos.y, camPos.z, 1.f));

    auto obj = ObjectManager()->Request_Object(targetHandle);
    auto targetCC = obj->Get_Component<CCharacterController>();

    const Vector4 foot4 = targetCC->Get_FootPosition();
    const Vector3 foot{foot4.x, foot4.y, foot4.z};

    const Vector3 basePivot = foot + Vector3(0.f, targetCC->Get_HalfSize() * 1.5f + profile.offsetY, 0.f);

    Vector3 forward = Vector3::Transform(Vector3(0.f, 0.f, 1.f), camRot);
    forward.Normalize();

    Vector3 toBase = basePivot - camPos;
    float d = toBase.Length();
    d = clamp(d, profile.minDist, profile.maxDist);

    const Vector3 desiredPivot = camPos + forward * d;

    pose.pivotOverrideOffset = desiredPivot - basePivot;

    pose.targetPivot = desiredPivot;
    pose.curPivot = desiredPivot;

    Vector3 toPivot = desiredPivot - camPos;
    const float rawDist = toPivot.Length();
    toPivot /= rawDist;

    const float yawRad = atan2f(toPivot.x, toPivot.z);
    const float pitchRad = asinf(clamp(-toPivot.y, -1.f, 1.f));

    pose.curRotDeg.x = XMConvertToDegrees(yawRad);
    pose.curRotDeg.y = XMConvertToDegrees(pitchRad);
    pose.targetRotDeg = pose.curRotDeg;

    pose.curDist = rawDist;
    pose.targetDist = rawDist;

    ClampTargets();

    const PxExtendedVec3& c = cc->Get_Controller()->getPosition();
    m_pTransform->Set_WorldPos(XMVectorSet((float)c.x, (float)c.y, (float)c.z, 1.f));
    m_pTransform->LookAt(Vector4(desiredPivot.x, desiredPivot.y, desiredPivot.z, 1.f));
}

void COrbitCam::CaptureSnapshot(OrbitCamSnapshot& out) const
{
    out.pose = pose;
    out.targetSwitch = targetSwitch;
    lockOnCtrl.Capture(out.lockOn);
    out.autoYawHoldTimer = autoYawHoldTimer;
    out.prevTargetFoot = prevTargetFoot;
    out.hasPrevTargetFoot = hasPrevTargetFoot;
    out.targetHandle = targetHandle;
}

void COrbitCam::RestoreSnapshot(const OrbitCamSnapshot& s)
{
    targetHandle = s.targetHandle;
    pose = s.pose;
    targetSwitch = s.targetSwitch;
    lockOnCtrl.Restore(s.lockOn);
    autoYawHoldTimer = s.autoYawHoldTimer;
    prevTargetFoot = s.prevTargetFoot;
    hasPrevTargetFoot = s.hasPrevTargetFoot;

    const Vector3 pivot = GetPivotTargetPos();
    pose.targetPivot = pivot;
    pose.curPivot = pivot;

    const float yawRad = XMConvertToRadians(pose.curRotDeg.x);
    const float pitchRad = XMConvertToRadians(pose.curRotDeg.y);
    const Quaternion q = Quaternion::CreateFromYawPitchRoll(yawRad, pitchRad, 0.f);

    const Vector3 backDir = Vector3::Transform(Vector3(0.f, 0.f, -1.f), q);
    const Vector3 camPos = pivot + backDir * pose.curDist;

    auto cc = Get_Component<CCharacterController>();
    cc->Set_Position(XMVectorSet(camPos.x, camPos.y, camPos.z, 1.f));

    lockOnCtrl.UpdateBlend(0.f);

    OrbitLockOnEvalResult lockRes{};
    if (lockOnCtrl.IsActiveOrBlending())
    {
        lockRes = lockOnCtrl.Evaluate(0.f, profile, targetHandle, pose.curRotDeg.x, pose.curDist,
            [&](OBJECT_HANDLE h) { return GetBasePivotTargetPos(h); });
    }

    Vector3 lookAt = pivot;
    if (lockRes.weight > 0.f) lookAt = Vector3::Lerp(pivot, lockRes.focusPos, lockRes.weight);

    m_pTransform->Set_WorldPos(Vector4(camPos.x, camPos.y, camPos.z, 1.f));
    m_pTransform->LookAt(Vector4(lookAt.x, lookAt.y, lookAt.z, 1.f));
}

void COrbitCam::Priority_Update(_float dt)
{
    if (!targetHandle.isValid()) return;

    if (lockOnCtrl.IsActiveOrBlending() && !lockOnCtrl.GetHandle().isValid())
        ClearLockOn();

    UpdateTargetSwitch(dt);

    pose.targetPivot = GetPivotTargetPos();

    lockOnCtrl.UpdateBlend(dt);

    UpdateInput(dt);

    OrbitLockOnEvalResult lockRes{};
    if (lockOnCtrl.IsActiveOrBlending())
    {
        lockRes = lockOnCtrl.Evaluate(dt, profile, targetHandle, pose.targetRotDeg.x, pose.targetDist,
            [&](OBJECT_HANDLE h) { return GetBasePivotTargetPos(h); });

        pose.targetRotDeg.x += lockRes.yawAddDeg;
        if (lockRes.hasDist) pose.targetDist = lockRes.dist;
    }

    if (lockRes.weight <= 0.f) UpdateAutoYawFollow(dt);

    ClampTargets();
    SmoothStates(dt);
    ApplyOrbitPose(dt, lockRes);
}

void COrbitCam::UpdateInput(_float dt)
{
    auto& io = ImGui::GetIO();

    if (io.WantCaptureMouse || ImGui::IsAnyItemActive() || ImGui::IsAnyItemHovered()) return;

    const float dx = InputDevice()->Mouse_DeltaX();
    const float dy = InputDevice()->Mouse_DeltaY();

    const float w = lockOnCtrl.GetWeight();

    if (w <= 0.f) pose.targetRotDeg.x += dx * input.sensitivityX;
    pose.targetRotDeg.y += dy * input.sensitivityY;

    if (dx != 0.f || dy != 0.f) autoYawHoldTimer = profile.autoYawFollowDelay;

    const float wheel = InputDevice()->Mouse_DeltaW() * 0.5f;
    if (wheel != 0.f) pose.targetDist -= wheel * input.zoomSpeed;
}

void COrbitCam::ClampTargets()
{
    pose.targetRotDeg.y = clamp(pose.targetRotDeg.y, profile.pitchMin, profile.pitchMax);
    pose.targetDist = clamp(pose.targetDist, profile.minDist, profile.maxDist);
}

void COrbitCam::SmoothStates(_float dt)
{
    float rot = 1.f - expf(-profile.rotSmoothSpeed * dt);
    rot = clamp(rot, 0.f, 1.f);
    pose.curRotDeg = pose.curRotDeg + (pose.targetRotDeg - pose.curRotDeg) * rot;

    float dist = 1.f - expf(-profile.distSmoothSpeed * dt);
    dist = clamp(dist, 0.f, 1.f);
    pose.curDist = pose.curDist + (pose.targetDist - pose.curDist) * dist;

    float pivot = 1.f - expf(-profile.pivotSmoothSpeed * dt);
    pivot = clamp(pivot, 0.f, 1.f);
    pose.curPivot = pose.curPivot + (pose.targetPivot - pose.curPivot) * pivot;
}

Vector3 COrbitCam::GetPivotTargetPos() const
{
    const Vector3 basePivot = GetBasePivotTargetPos(targetHandle);
    return basePivot + pose.pivotOverrideOffset;
}

float COrbitCam::GetEffectiveDist() const
{
    if (pose.curDist > profile.maxDist) return profile.maxDist;
    return pose.curDist;
}

void COrbitCam::ApplyOrbitPose(_float dt, const OrbitLockOnEvalResult& lockRes)
{
    const Vector3 pivot = GetPivotPos();

    const float yawRad = XMConvertToRadians(pose.curRotDeg.x);
    const float pitchRad = XMConvertToRadians(pose.curRotDeg.y);
    const Quaternion q = Quaternion::CreateFromYawPitchRoll(yawRad, pitchRad, 0.f);

    const float dist = GetEffectiveDist();
    const Vector3 backDir = Vector3::Transform(Vector3(0.f, 0.f, -1.f), q);
    const Vector3 desiredPos = pivot + backDir * dist;

    auto cc = Get_Component<CCharacterController>();
    const PxExtendedVec3& c0 = cc->Get_Controller()->getPosition();
    const Vector3 curPos((float)c0.x, (float)c0.y, (float)c0.z);

    const Vector3 disp = desiredPos - curPos;
    cc->Move_Displacement(Vector4(disp.x, disp.y, disp.z, 0.f), dt);

    const PxExtendedVec3& c1 = cc->Get_Controller()->getPosition();
    const Vector3 newPos((float)c1.x, (float)c1.y, (float)c1.z);

    float actualDist = (newPos - pivot).Dot(backDir);
    actualDist = clamp(actualDist, profile.minDist, profile.maxDist);

    float delta = actualDist - pose.curDist;

    float maxStep = 0.f;
    if (delta < 0.f) maxStep = profile.collisionZoomInSpeed * dt;
    else maxStep = profile.collisionZoomOutSpeed * dt;

    delta = clamp(delta, -maxStep, maxStep);
    pose.curDist += delta;

    Vector3 lookAt = pivot;
    if (lockRes.weight > 0.f) lookAt = Vector3::Lerp(pivot, lockRes.focusPos, lockRes.weight);

    m_pTransform->Set_WorldPos(Vector4((float)c1.x, (float)c1.y, (float)c1.z, 1.f));
    m_pTransform->LookAt(Vector4(lookAt.x, lookAt.y, lookAt.z, 1.f));
}

void COrbitCam::UpdateAutoYawFollow(_float dt)
{
    if (!profile.useAutoYawFollow) return;

    if (autoYawHoldTimer > 0.f)
    {
        autoYawHoldTimer -= dt;
        return;
    }

    const Vector3 foot = GetTargetFootPos();

    if (!hasPrevTargetFoot)
    {
        prevTargetFoot = foot;
        hasPrevTargetFoot = true;
        return;
    }

    Vector3 delta = foot - prevTargetFoot;
    prevTargetFoot = foot;

    delta.y = 0.f;

    const float len = delta.Length();
    if (len == 0.f) return;

    delta /= len;

    Vector3 camLook = m_pTransform->Dir(STATE::LOOK);
    Vector3 camRight = m_pTransform->Dir(STATE::RIGHT);

    camLook.y = 0.f;
    camRight.y = 0.f;

    camLook.Normalize();
    camRight.Normalize();

    const float localZ = delta.Dot(camLook);
    const float localX = delta.Dot(camRight);

    if (localZ < 0.f && fabsf(localZ) > fabsf(localX)) return;

    const float desiredYawDeg = XMConvertToDegrees(atan2f(delta.x, delta.z));
    const float deltaYawDeg = Math::WrapDeg(desiredYawDeg - pose.targetRotDeg.x);

    float a = 1.f - expf(-profile.autoYawFollowSpeed * dt);
    a = clamp(a, 0.f, 1.f);

    pose.targetRotDeg.x += deltaYawDeg * a;
}

Vector3 COrbitCam::GetTargetFootPos() const
{
    auto obj = ObjectManager()->Request_Object(targetHandle);
    auto cc = obj->Get_Component<CCharacterController>();

    const Vector4 foot4 = cc->Get_FootPosition();
    return Vector3(foot4.x, foot4.y, foot4.z);
}

Vector3 COrbitCam::GetBasePivotTargetPos(OBJECT_HANDLE handle) const
{
    auto obj = ObjectManager()->Request_Object(handle);
    auto cc = obj->Get_Component<CCharacterController>();

    const Vector4 foot4 = cc->Get_FootPosition();
    const Vector3 foot{foot4.x, foot4.y, foot4.z};

    return foot + Vector3(0.f, cc->Get_HalfSize() * 1.5f + profile.offsetY, 0.f);
}

void COrbitCam::UpdateTargetSwitch(_float dt)
{
    if (!targetSwitch.active) return;

    targetSwitch.elapsed += dt;

    float t = targetSwitch.elapsed / profile.targetSwitchBlendSec;
    if (t >= 1.f)
    {
        targetSwitch.active = false;
        pose.pivotOverrideOffset = Vector3::Zero;
        return;
    }

    t = clamp(t, 0.f, 1.f);
    t = Math::ApplyEase(profile.targetSwitchEase, t);

    const Vector3 basePivotNow = GetBasePivotTargetPos(targetHandle);
    const Vector3 keepOffsetNow = targetSwitch.holdPivotWorld - basePivotNow;

    pose.pivotOverrideOffset = Vector3::Lerp(keepOffsetNow, Vector3::Zero, t);
}

COrbitCam* COrbitCam::Create()
{
    auto inst = new COrbitCam();
    if (FAILED(inst->Initialize_Prototype()))
    {
        MSG_BOX("Object Create Failed : COrbitCam");
        Safe_Release(inst);
    }
    return inst;
}

CGameObject* COrbitCam::Clone(INIT_DESC* pArg)
{
    auto inst = new COrbitCam(*this);
    if (FAILED(inst->Initialize(pArg)))
    {
        MSG_BOX("Object Clone Failed : COrbitCam");
        Safe_Release(inst);
    }
    return inst;
}

void COrbitCam::Render_GUI()
{
    __super::Render_GUI();

    if (!ImGui::CollapsingHeader("OrbitCam Debug", ImGuiTreeNodeFlags_DefaultOpen)) return;

    auto& io = ImGui::GetIO();

    struct TrackedValue
    {
        int   kind = 0;
        int   count = 0;
        int   i = 0;
        float f[4]{};
    };

    static unordered_map<ImGuiID, TrackedValue> s_prev{};
    static unordered_map<ImGuiID, float>        s_hot{};

    for (auto it = s_hot.begin(); it != s_hot.end();)
    {
        it->second -= io.DeltaTime;
        if (it->second <= 0.f) it = s_hot.erase(it);
        else ++it;
    }

    auto Mark = [&](ImGuiID id, const TrackedValue& cur)
        {
            auto it = s_prev.find(id);
            _bool diff = false;

            if (it == s_prev.end()) diff = true;
            else
            {
                const TrackedValue& p = it->second;
                if (p.kind != cur.kind || p.count != cur.count || p.i != cur.i) diff = true;
                else
                {
                    for (int k = 0; k < 4; ++k)
                        if (p.f[k] != cur.f[k]) { diff = true; break; }
                }
            }

            if (diff)
            {
                s_prev[id] = cur;
                s_hot[id] = 0.25f;
            }
        };

    auto IsHot = [&](ImGuiID id) { return s_hot.find(id) != s_hot.end(); };

    auto TextRight = [&](const char* text)
        {
            const float w = ImGui::CalcTextSize(text).x;
            const float avail = ImGui::GetContentRegionAvail().x;
            const float pad = (avail > w) ? (avail - w) : 0.f;
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + pad);
            ImGui::TextUnformatted(text);
        };

    auto BeginTable3 = [&](const char* id)
        {
            return ImGui::BeginTable(id, 3,
                ImGuiTableFlags_RowBg |
                ImGuiTableFlags_BordersOuterH |
                ImGuiTableFlags_BordersInnerV |
                ImGuiTableFlags_SizingStretchProp);
        };

    auto SetupTable3 = [&]()
        {
            ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthStretch, 0.48f);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch, 0.27f);
            ImGui::TableSetupColumn("Edit", ImGuiTableColumnFlags_WidthStretch, 0.25f);
            ImGui::TableHeadersRow();
        };

    auto RowLabel = [&](const char* label)
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted(label);
        };

    auto RowValueText = [&](ImGuiID id, const char* valueText)
        {
            ImGui::TableSetColumnIndex(1);

            const _bool hot = IsHot(id);
            if (hot) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.2f, 0.2f, 1.f));
            TextRight(valueText);
            if (hot) ImGui::PopStyleColor();
        };

    auto RowEmptyEdit = [&]()
        {
            ImGui::TableSetColumnIndex(2);
            ImGui::TextUnformatted("");
        };

    auto MakeId = [&](const char* label) { return ImGui::GetID(label); };

    auto RowBool = [&](const char* label, _bool v)
        {
            const ImGuiID id = MakeId(label);
            TrackedValue cur{};
            cur.kind = 1;
            cur.i = v ? 1 : 0;
            Mark(id, cur);

            RowLabel(label);
            RowValueText(id, v ? "Yes" : "No");
            RowEmptyEdit();
        };

    auto RowFloat = [&](const char* label, float v, const char* fmt)
        {
            char buf[128]{};
            sprintf_s(buf, fmt, v);

            const ImGuiID id = MakeId(label);
            TrackedValue cur{};
            cur.kind = 2;
            cur.count = 1;
            cur.f[0] = v;
            Mark(id, cur);

            RowLabel(label);
            RowValueText(id, buf);
            RowEmptyEdit();
        };

    auto RowVec2 = [&](const char* label, const Vector2& v, const char* fmt)
        {
            char buf[128]{};
            sprintf_s(buf, fmt, v.x, v.y);

            const ImGuiID id = MakeId(label);
            TrackedValue cur{};
            cur.kind = 3;
            cur.count = 2;
            cur.f[0] = v.x;
            cur.f[1] = v.y;
            Mark(id, cur);

            RowLabel(label);
            RowValueText(id, buf);
            RowEmptyEdit();
        };

    auto RowVec3 = [&](const char* label, const Vector3& v, const char* fmt)
        {
            char buf[128]{};
            sprintf_s(buf, fmt, v.x, v.y, v.z);

            const ImGuiID id = MakeId(label);
            TrackedValue cur{};
            cur.kind = 4;
            cur.count = 3;
            cur.f[0] = v.x;
            cur.f[1] = v.y;
            cur.f[2] = v.z;
            Mark(id, cur);

            RowLabel(label);
            RowValueText(id, buf);
            RowEmptyEdit();
        };

    auto RowFloatEdit = [&](const char* label, float* v, float speed, float vmin, float vmax, const char* fmt)
        {
            char buf[128]{};
            sprintf_s(buf, fmt, *v);

            const ImGuiID id = MakeId(label);
            TrackedValue cur{};
            cur.kind = 2;
            cur.count = 1;
            cur.f[0] = *v;
            Mark(id, cur);

            RowLabel(label);
            RowValueText(id, buf);

            ImGui::TableSetColumnIndex(2);
            ImGui::PushID(label);
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::DragFloat("##edit", v, speed, vmin, vmax, fmt);
            ImGui::PopID();
        };

    auto RowBoolEdit = [&](const char* label, _bool* v)
        {
            const ImGuiID id = MakeId(label);
            TrackedValue cur{};
            cur.kind = 1;
            cur.i = *v ? 1 : 0;
            Mark(id, cur);

            RowLabel(label);
            RowValueText(id, *v ? "Yes" : "No");

            ImGui::TableSetColumnIndex(2);
            ImGui::PushID(label);
            ImGui::Checkbox("##edit", v);
            ImGui::PopID();
        };

    if (ImGui::CollapsingHeader("Status", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::PushID("Status");
        if (BeginTable3("##OrbitCam_Status"))
        {
            SetupTable3();
            RowBool("Target Valid", targetHandle.isValid());
            RowFloat("AutoYaw HoldTimer", autoYawHoldTimer, "%.3f");
            ImGui::EndTable();
        }
        ImGui::PopID();
    }

    if (ImGui::CollapsingHeader("Pose", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::PushID("Pose");
        if (BeginTable3("##OrbitCam_Pose"))
        {
            SetupTable3();
            RowVec2("RotDeg Cur (Yaw,Pitch)", pose.curRotDeg, "(%.3f, %.3f)");
            RowVec2("RotDeg Target (Yaw,Pitch)", pose.targetRotDeg, "(%.3f, %.3f)");
            RowFloat("Dist Cur", pose.curDist, "%.4f");
            RowFloat("Dist Target", pose.targetDist, "%.4f");
            RowVec3("Pivot Cur", pose.curPivot, "(%.3f, %.3f, %.3f)");
            RowVec3("Pivot Target", pose.targetPivot, "(%.3f, %.3f, %.3f)");
            RowVec3("Pivot Offset", pose.pivotOverrideOffset, "(%.3f, %.3f, %.3f)");
            ImGui::EndTable();
        }
        ImGui::PopID();
    }

    if (ImGui::CollapsingHeader("Input (Editable)", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::PushID("Input");
        if (BeginTable3("##OrbitCam_Input"))
        {
            SetupTable3();
            RowFloatEdit("Sensitivity X", &input.sensitivityX, 0.001f, 0.f, 10.f, "%.3f");
            RowFloatEdit("Sensitivity Y", &input.sensitivityY, 0.001f, 0.f, 10.f, "%.3f");
            RowFloatEdit("Zoom Speed", &input.zoomSpeed, 0.01f, 0.f, 50.f, "%.3f");
            ImGui::EndTable();
        }
        ImGui::PopID();
    }

    if (ImGui::CollapsingHeader("Profile (Editable)", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::PushID("Profile");
        if (BeginTable3("##OrbitCam_Profile"))
        {
            SetupTable3();

            RowFloatEdit("Min Dist", &profile.minDist, 0.01f, 0.f, 50.f, "%.3f");
            RowFloatEdit("Max Dist", &profile.maxDist, 0.01f, 0.f, 50.f, "%.3f");

            RowFloatEdit("Pitch Min", &profile.pitchMin, 0.1f, -89.f, 89.f, "%.2f");
            RowFloatEdit("Pitch Max", &profile.pitchMax, 0.1f, -89.f, 89.f, "%.2f");

            RowFloatEdit("Smooth Rot", &profile.rotSmoothSpeed, 0.1f, 0.f, 100.f, "%.2f");
            RowFloatEdit("Smooth Dist", &profile.distSmoothSpeed, 0.1f, 0.f, 100.f, "%.2f");
            RowFloatEdit("Smooth Pivot", &profile.pivotSmoothSpeed, 0.1f, 0.f, 100.f, "%.2f");

            RowFloatEdit("Offset Y", &profile.offsetY, 0.01f, -10.f, 10.f, "%.3f");

            RowBoolEdit("Use AutoYaw Follow", &profile.useAutoYawFollow);
            RowFloatEdit("AutoYaw Follow Speed", &profile.autoYawFollowSpeed, 0.01f, 0.f, 50.f, "%.3f");
            RowFloatEdit("AutoYaw Follow Delay", &profile.autoYawFollowDelay, 0.01f, 0.f, 10.f, "%.3f");

            RowFloatEdit("Collision Zoom In Speed", &profile.collisionZoomInSpeed, 0.1f, 0.f, 100.f, "%.2f");
            RowFloatEdit("Collision Zoom Out Speed", &profile.collisionZoomOutSpeed, 0.1f, 0.f, 100.f, "%.2f");

            RowFloatEdit("Target Switch Blend Sec", &profile.targetSwitchBlendSec, 0.01f, 0.01f, 10.f, "%.3f");

            ImGui::EndTable();
        }

        ImGui::Text("Target Switch Ease: %d", (_int)profile.targetSwitchEase);
        ImGui::PopID();
    }

    if (ImGui::CollapsingHeader("CCT", ImGuiTreeNodeFlags_DefaultOpen))
    {
        auto cc = Get_Component<CCharacterController>();
        const PxExtendedVec3& cpos = cc->Get_Controller()->getPosition();
        const PxExtendedVec3& fpos = cc->Get_Controller()->getFootPosition();

        const Vector3 camPos((float)cpos.x, (float)cpos.y, (float)cpos.z);
        const Vector3 footPos((float)fpos.x, (float)fpos.y, (float)fpos.z);

        const _float3 vel0 = cc->Get_Velocity();
        const Vector3 vel(vel0.x, vel0.y, vel0.z);

        ImGui::PushID("CCT");
        if (BeginTable3("##OrbitCam_CCT"))
        {
            SetupTable3();
            RowVec3("Pos", camPos, "(%.3f, %.3f, %.3f)");
            RowVec3("Foot", footPos, "(%.3f, %.3f, %.3f)");
            RowBool("Grounded", cc->Is_Grounded());
            RowVec3("Vel", vel, "(%.3f, %.3f, %.3f)");
            RowFloat("MinMoveDist", cc->Get_MinMoveDist(), "%.6f");
            RowFloat("ContactOffset", cc->Get_ContactOffset(), "%.6f");
            RowFloat("RestOffset", cc->Get_RestOffset(), "%.6f");
            ImGui::EndTable();
        }
        ImGui::PopID();
    }

    if (ImGui::CollapsingHeader("Orbit Metrics", ImGuiTreeNodeFlags_DefaultOpen))
    {
        auto cc = Get_Component<CCharacterController>();
        const PxExtendedVec3& cpos = cc->Get_Controller()->getPosition();
        const Vector3 camPos((float)cpos.x, (float)cpos.y, (float)cpos.z);

        const Vector3 pivot = GetPivotPos();

        const float yawRad = XMConvertToRadians(pose.curRotDeg.x);
        const float pitchRad = XMConvertToRadians(pose.curRotDeg.y);
        const Quaternion q = Quaternion::CreateFromYawPitchRoll(yawRad, pitchRad, 0.f);

        Vector3 backDir = Vector3::Transform(Vector3(0.f, 0.f, -1.f), q);
        backDir.Normalize();

        const float distLen = (pivot - camPos).Length();
        const float distProj = (camPos - pivot).Dot(backDir);
        const float distErr = distLen - distProj;

        const Vector3 desiredPos = pivot + backDir * pose.curDist;
        const Vector3 toDesired = desiredPos - camPos;

        ImGui::PushID("Metrics");
        if (BeginTable3("##OrbitCam_Metrics"))
        {
            SetupTable3();
            RowVec3("Pivot", pivot, "(%.3f, %.3f, %.3f)");
            RowVec3("BackDir", backDir, "(%.3f, %.3f, %.3f)");
            RowFloat("Dist Cur", pose.curDist, "%.4f");
            RowFloat("Dist Len", distLen, "%.4f");
            RowFloat("Dist Proj", distProj, "%.4f");
            RowFloat("Len - Proj", distErr, "%.6f");
            RowVec3("DesiredPos", desiredPos, "(%.3f, %.3f, %.3f)");
            RowVec3("ToDesired", toDesired, "(%.3f, %.3f, %.3f)");
            RowFloat("ToDesired Len", toDesired.Length(), "%.4f");
            ImGui::EndTable();
        }
        ImGui::PopID();
    }

    if (ImGui::CollapsingHeader("Target Switch", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::PushID("TargetSwitch");
        if (BeginTable3("##OrbitCam_TargetSwitch"))
        {
            SetupTable3();
            RowBool("Active", targetSwitch.active);
            RowFloat("Elapsed", targetSwitch.elapsed, "%.3f");
            RowVec3("HoldPivot", targetSwitch.holdPivotWorld, "(%.3f, %.3f, %.3f)");
            ImGui::EndTable();
        }
        ImGui::PopID();
    }
}