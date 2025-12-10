#pragma once
#include "Base.h"
NS_BEGIN(Engine)
class ENGINE_DLL CSoundData final :
    public CBase
{

private:
    CSoundData(const string& soundKey);
    virtual ~CSoundData() DEFAULT;
public:
    HRESULT Initialize(const string& filePath);
    FMOD::Sound* Get_SoundData() { return m_pSound; };
private:
    string m_SoundKey;
    FMOD::Sound* m_pSound = { nullptr };
public:
    static CSoundData* Create(const string& filePath, const string& soundKey);
    virtual void Free() override;

};

NS_END