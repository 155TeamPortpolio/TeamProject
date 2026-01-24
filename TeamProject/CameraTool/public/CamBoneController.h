#pragma once

NS_BEGIN(Engine)
struct CamBoneAttachDesc;
NS_END

NS_BEGIN(CameraTool)

class CCamBoneController
{
public:
    void  SetSpaceRef(OBJECT_HANDLE h) { m_spaceRef = h; }
    void  SetDesc(const CamBoneAttachDesc* desc) { m_desc = desc; }

    _bool HasDesc() const { return m_desc && m_desc->enabled; }
    _bool HasPosBone() const { return HasDesc() && m_desc->usePosBone && !m_desc->posBoneName.empty(); }
    _bool HasLookAtBone() const { return HasDesc() && m_desc->useLookAtBone && !m_desc->lookAtBoneName.empty(); }

    Matrix  GetSpaceRT() const;

    Matrix  GetPosBoneRT() const;
    Matrix  GetLookAtBoneRT() const;

    Vector3 GetSpacePosWorld() const;
    Vector3 GetPosBonePosWorld() const;
    Vector3 GetLookAtTargetWorld() const;

private:
    Matrix GetSpaceRefRT() const;
    Matrix GetBoneRT(const string& boneName) const;

private:
    OBJECT_HANDLE            m_spaceRef{};
    const CamBoneAttachDesc* m_desc{};
};

NS_END