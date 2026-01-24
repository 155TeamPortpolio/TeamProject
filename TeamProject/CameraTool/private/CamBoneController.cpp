#include "pch.h"
#include "CamBoneController.h"

#include "Animator3D.h"

Matrix CCamBoneController::GetSpaceRT() const
{
    return GetSpaceRefRT();
}

Matrix CCamBoneController::GetPosBoneRT() const
{
    if (!HasPosBone()) return GetSpaceRefRT();
    return GetBoneRT(m_desc->posBoneName);
}

Matrix CCamBoneController::GetLookAtBoneRT() const
{
    if (!HasLookAtBone()) return GetSpaceRefRT();
    return GetBoneRT(m_desc->lookAtBoneName);
}

Vector3 CCamBoneController::GetSpacePosWorld() const
{
    return GetSpaceRefRT().Translation();
}

Vector3 CCamBoneController::GetPosBonePosWorld() const
{
    return GetPosBoneRT().Translation();
}

Vector3 CCamBoneController::GetLookAtTargetWorld() const
{
    if (HasLookAtBone()) return GetLookAtBoneRT().Translation();
    return GetSpaceRefRT().Translation();
}

Matrix CCamBoneController::GetSpaceRefRT() const
{
    auto obj = OBJ->Request_Object(m_spaceRef);
    auto tr = obj->Get_Component<CTransform>();

    Matrix world = Matrix(tr->Get_WorldMatrix());

    Vector3 s{};
    Vector3 t{};
    Quaternion r = Quaternion::Identity;
    world.Decompose(s, r, t);
    r.Normalize();

    return Matrix::CreateFromQuaternion(r) * Matrix::CreateTranslation(t);
}

Matrix CCamBoneController::GetBoneRT(const string& boneName) const
{
    auto obj = OBJ->Request_Object(m_spaceRef);
    auto anim = obj->Get_Component<CAnimator3D>();

    Matrix boneWorld = Matrix(anim->Get_BoneMatrix(CAnimator3D::BoneSpace::WORLD, boneName));

    Vector3 s{};
    Vector3 t{};
    Quaternion r = Quaternion::Identity;
    boneWorld.Decompose(s, r, t);
    r.Normalize();

    return Matrix::CreateFromQuaternion(r) * Matrix::CreateTranslation(t);
}