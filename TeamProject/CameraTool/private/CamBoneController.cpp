#include "pch.h"
#include "CamBoneController.h"

#include "Animator3D.h"

Matrix CCamBoneController::GetSpaceRT() const
{
    return GetSpaceRefRT();
}

Matrix CCamBoneController::GetBoneRT() const
{
    if (!m_desc) return GetSpaceRefRT();
    if (m_desc->boneName.empty()) return GetSpaceRefRT();
    return GetBoneRT(m_desc->boneName);
}

Matrix CCamBoneController::GetRefRT()
{
    Matrix spaceRT = GetSpaceRefRT();
    if (!IsEnabled()) return spaceRT;

    if (m_desc->mode == CamBoneMode::Parent) return GetBoneRT(m_desc->boneName);
    return spaceRT;
}

Vector3 CCamBoneController::GetLookAtTargetWorld()
{
    Matrix spaceRT = GetSpaceRefRT();
    if (!IsEnabled()) return spaceRT.Translation();

    if (m_desc->mode == CamBoneMode::LookAt)
        return GetBoneRT(m_desc->boneName).Translation();

    return spaceRT.Translation();
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