#pragma once

NS_BEGIN(CameraTool)

class CCamBoneController
{
public:
    void  SetSpaceRef(OBJECT_HANDLE h) { m_spaceRef = h; }
    void  SetDesc(const CamBoneAttachDesc* desc) { m_desc = desc; }

    _bool IsEnabled() { return m_desc && m_desc->enabled && m_spaceRef.isValid() && !m_desc->boneName.empty(); }

    Matrix  GetSpaceRT() const;
    Matrix  GetBoneRT() const;
    Matrix  GetRefRT();
    Vector3 GetLookAtTargetWorld();

private:
    Matrix GetSpaceRefRT() const;
    Matrix GetBoneRT(const string& boneName) const;

private:
    OBJECT_HANDLE            m_spaceRef{};
    const CamBoneAttachDesc* m_desc{};
};

NS_END