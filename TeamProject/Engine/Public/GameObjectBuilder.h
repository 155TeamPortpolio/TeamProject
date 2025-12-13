#pragma once
#include "Engine_Defines.h"
#include "Build_Struct.h"

NS_BEGIN(Engine)
class ENGINE_DLL CGameObjectBuilder
{
public:
    CGameObjectBuilder(const CLONE_DESC& cloneDesc);
    ~CGameObjectBuilder();
public:
    class CGameObject* Build (const string& instanceKey,_uint* id = nullptr);
    CGameObjectBuilder& Camera(const CAMERA_DESC& camera);
    CGameObjectBuilder& Light(const LIGHT_INIT_DESC& light);

    CGameObjectBuilder& Position(const _float3 position);
    CGameObjectBuilder& Rotate(const _float3 rotate);
    CGameObjectBuilder& Scale(const _float3 scale);
    
    CGameObjectBuilder& Collider(const COLLIDER_DESC& desc);
   // CGameObjectBuilder& AABB_Collider(const AABB_COLLIDER_DESC& desc);
   //CGameObjectBuilder& OBB_Collider(const OBB_COLLIDER_DESC& desc);
   //CGameObjectBuilder& Sphere_Collider(const SPHERE_COLLIDER_DESC& desc);

    CGameObjectBuilder& Set_Parent(const PARENT_DESC& parent);


    CGameObjectBuilder& Add_ObjDesc(GAMEOBJECT_DESC* pArg);

private:
    class CGameInstance* m_pGameInstance = { nullptr };
    LAYER_DESC* m_layerDesc = { nullptr };
    //CLONE_DESC* m_CloneDesc = {nullptr};
    CLONE_DESC m_CloneDesc = {};
    GAMEOBJECT_DESC* m_pObjDesc = { nullptr };

    unordered_map<type_index, COMPONENT_DESC*> m_CompDesc;
};

NS_END