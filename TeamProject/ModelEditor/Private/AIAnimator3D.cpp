#include "AIAnimator3D.h"
#include "AIAnimationClip.h"
#include "AIModelData.h"
#include "Helper_Func.h"
#include "GameInstance.h"

CAIAnimator3D::CAIAnimator3D()
{
}

HRESULT CAIAnimator3D::Initialize(const aiScene* scene, CAIModelData* modelData)
{
    animClips.clear();
    animClips.reserve(scene->mNumAnimations);

    for (_uint i = 0; i < scene->mNumAnimations; i++)
    {
        CAIAnimationClip* aiClip = CAIAnimationClip::Create(scene->mAnimations[i], modelData);
        if (!aiClip) return E_FAIL;

        animClips.push_back(static_cast<CAnimationClip*>(aiClip));
    }

    data = static_cast<CModelData*>(modelData);
    Safe_AddRef(data);

    _float4x4 identity;
    XMStoreFloat4x4(&identity, XMMatrixIdentity());

    transMats.resize(data->Get_BoneCount(), identity);
    manipulateMats.resize(data->Get_BoneCount(), identity);
    combinedMats.resize(data->Get_BoneCount(), identity);

    for (size_t i = 0; i < data->Get_BoneCount(); i++)
        transMats[i] = data->Get_TransformMatrix(i);

    for (size_t i = 0; i < data->Get_BoneCount(); i++)
    {
        int parent = data->Get_BoneParentIndex(i);

        if (parent == -1) combinedMats[i] = transMats[i];
        else
        {
            _matrix parentCombine = XMLoadFloat4x4(&combinedMats[parent]);
            _matrix myTrans = XMLoadFloat4x4(&transMats[i]);
            XMStoreFloat4x4(&combinedMats[i], myTrans * parentCombine);
        }
    }

    tPose = combinedMats;
    basePose.resize(data->Get_BoneCount(), identity);

    Resize_Layer(1);

    animLayers[0].baseLayer = true;
    animLayers[0].layerType = ANIM_LAYER_STATE::BASE;
    animLayers[0].weight = 1.f;
    animLayers[0].rootBoneIdx = data->Find_BoneIndexByName("Root");

    return S_OK;
}

void CAIAnimator3D::Render_GUI()
{
    __super::Render_GUI();
}

HRESULT CAIAnimator3D::Save_Animation(const string& savePath, const _float4x4* worldMat)
{
    string animSavePath = savePath + "\\Anim\\";
    std::filesystem::create_directories(animSavePath);

    ANIM_META meta;

    for (auto& clip : animClips)
    {
        std::filesystem::path filePath = std::filesystem::path(animSavePath) / (clip->Get_Name() + ".anim");
        std::ofstream ofs(filePath, std::ios::binary);

        static_cast<CAIAnimationClip*>(clip)->Save_File(ofs);

        ofs.close();

        ANIM_CLIP one{};
        one.ClipTag = clip->Get_Name();
        meta.Clips.push_back(one);
    }

    meta.PreTransform = *worldMat;

    size_t pos = meta.Clips.back().ClipTag.find("_Ani_");
    string name = meta.Clips.back().ClipTag.substr(0, pos);

    Helper::SaveJson<ANIM_META>(meta, savePath + name + "_Meta.json");

    return S_OK;
}

CAIAnimator3D* CAIAnimator3D::Create(const aiScene* scene, CAIModelData* modelData)
{
    CAIAnimator3D* inst = new CAIAnimator3D();

    if (FAILED(inst->Initialize(scene, modelData)))
    {
        Safe_Release(inst);
        MSG_BOX("Create Failed : Engine | CAIAnimator3D");
        return nullptr;
    }

    return inst;
}

void CAIAnimator3D::Free()
{
    __super::Free();
}