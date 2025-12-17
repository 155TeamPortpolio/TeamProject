#pragma once

#include "CamObject.h"

NS_BEGIN(Client)

template<typename T>
class CCamSpawnObject abstract : public CCamObject
{
protected:
    CCamSpawnObject() DEFAULT;
    CCamSpawnObject(const CCamSpawnObject& rhs) : CCamObject(rhs) {}
    virtual ~CCamSpawnObject() DEFAULT;

public:
    static T* Create()
    {
        auto inst = new T();
        if (FAILED(inst->Initialize_Prototype()))
        {
            MSG_BOX("Object Create Failed");
            Safe_Release(inst);
        }
        return inst;
    }

    CGameObject* Clone(INIT_DESC* pArg) override
    {
        auto inst = new T(*static_cast<T*>(this));
        if (FAILED(inst->Initialize(pArg)))
        {
            MSG_BOX("Object Clone Failed");
            Safe_Release(inst);
        }
        return inst;
    }
};

NS_END