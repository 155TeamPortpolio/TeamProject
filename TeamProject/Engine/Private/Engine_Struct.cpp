#include "Engine_Defines.h"
#include "GameInstance.h"
#include "GameObject.h"

_bool Engine::tagObjectHandle::isValid()
{
	CGameObject* pObj = CGameInstance::GetInstance()->Get_ObjectMgr()->Request_Object({ Level,Layer,hObjID });
	if(pObj)
		return true;
	return false;
}
void Engine::tagObjectHandle::Reset()
{
	Level.clear();
	Layer.clear();
	hObjID = 0;
	return;
}

CGameObject* Engine::tagObjectHandle::Get()
{
	CGameObject* pObj = CGameInstance::GetInstance()->Get_ObjectMgr()->Request_Object({ Level,Layer,hObjID });
	return pObj;
}

void Engine::tagObjectHandle::Release()
{

auto mgr = CGameInstance::GetInstance()->Get_ObjectMgr();

CGameObject* pObj = mgr->Request_Object({ Level, Layer, hObjID });
if (!pObj) { Reset(); return; }

mgr->Remove_Object(pObj);
Reset();
}

PARTICLE_NODE Engine::tagParticleNode::FromJson(nlohmann::ordered_json& json)
{
	PARTICLE_NODE node{};

	node.eType = json.value("effect_type", node.eType);
	node.TextureKey = json.value("texture_key", node.TextureKey);
	node.TexturePath = json.value("texture_path", node.TexturePath);

	node.fDelayTime = json.value("delay_time", node.fDelayTime);
	node.fDuration = json.value("duration", node.fDuration);
	node.isLoop = json.value("is_loop", node.isLoop);
	node.isWorld = json.value("is_world", node.isWorld);
	node.iBurstCount = json.value("burst_count", node.iBurstCount);
	node.fSpawnPerSec = json.value("spawn_per_sec", node.fSpawnPerSec);
	node.iMaxSpawnParticleCount = json.value("max_spawn_particle_count", node.iMaxSpawnParticleCount);
	
	node.vStartSpeed.x = json.at("start_speed").at("x").get<_float>();
	node.vStartSpeed.y = json.at("start_speed").at("y").get<_float>();
	node.vStartLifeTime.x = json.at("start_life_time").at("x").get<_float>();
	node.vStartLifeTime.y = json.at("start_life_time").at("y").get<_float>();
	node.vStartSize.x = json.at("start_size").at("x").get<_float>();
	node.vStartSize.y = json.at("start_size").at("y").get<_float>();

	node.vSpawnAreaMin.x = json.at("spawn_area_min").at("x").get<_float>();
	node.vSpawnAreaMin.y = json.at("spawn_area_min").at("y").get<_float>();
	node.vSpawnAreaMin.z = json.at("spawn_area_min").at("z").get<_float>();

	node.vSpawnAreaMax.x = json.at("spawn_area_max").at("x").get<_float>();
	node.vSpawnAreaMax.y = json.at("spawn_area_max").at("y").get<_float>();
	node.vSpawnAreaMax.z = json.at("spawn_area_max").at("z").get<_float>();

	node.useGravity = json.value("use_gravity", node.useGravity);
	node.fGravityScale = json.value("gravity_scale", node.fGravityScale);

	node.fDampScale = json.value("damp_scale", node.fDampScale);

	node.vStartScale.x = json.at("start_scale").at("x").get<_float>();
	node.vStartScale.y = json.at("start_scale").at("y").get<_float>();
	node.vEndScale.x = json.at("end_scale").at("x").get<_float>();
	node.vEndScale.y = json.at("end_scale").at("y").get<_float>();

	node.vStartColor.x = json.at("start_color").at("x").get<_float>();
	node.vStartColor.y = json.at("start_color").at("y").get<_float>();
	node.vStartColor.z = json.at("start_color").at("z").get<_float>();
	node.vStartColor.w = json.at("start_color").at("w").get<_float>();

	node.vEndColor.x = json.at("end_color").at("x").get<_float>();
	node.vEndColor.y = json.at("end_color").at("y").get<_float>();
	node.vEndColor.z = json.at("end_color").at("z").get<_float>();
	node.vEndColor.w = json.at("end_color").at("w").get<_float>();

	node.isParticleAnimated = json.value("particle_animated", node.isParticleAnimated);
	node.isRandomFrameIndex = json.value("random_frame_index", node.isRandomFrameIndex);
	node.iCol = json.value("col", node.iCol);
	node.iRow = json.value("row", node.iRow);
	node.iMaxFrameIndex = json.value("max_frame_index", node.iMaxFrameIndex);

	node.vStrength.x = json.at("strength").at("x").get<_float>();
	node.vStrength.y = json.at("strength").at("y").get<_float>();
	node.vStrength.z = json.at("strength").at("z").get<_float>();

	node.vFrequency.x = json.at("frequency").at("x").get<_float>();
	node.vFrequency.y = json.at("frequency").at("y").get<_float>();
	node.vFrequency.z = json.at("frequency").at("z").get<_float>();

	node.vScrollSpeed.x = json.at("scroll_speed").at("x").get<_float>();
	node.vScrollSpeed.y = json.at("scroll_speed").at("y").get<_float>();
	node.vScrollSpeed.z = json.at("scroll_speed").at("z").get<_float>();

	return node;
}

MESH_NODE Engine::tagMeshNode::FromJson(nlohmann::ordered_json& json)
{
	MESH_NODE node{};

	node.eType = json.value("effect_type", node.eType);
	node.ModelTag = json.value("model_key", node.ModelTag);
	node.MaterialTag = json.value("material_key", node.MaterialTag);

	node.fDelayTime = json.value("delay_time", node.fDelayTime);
	node.fDuration = json.value("duration", node.fDuration);
	node.isLoop = json.value("is_loop", node.isLoop);

	node.vBaseColor.x = json.at("base_color").at("x").get<_float>();
	node.vBaseColor.y = json.at("base_color").at("y").get<_float>();
	node.vBaseColor.z = json.at("base_color").at("z").get<_float>();
	node.vBaseColor.w = json.at("base_color").at("w").get<_float>();

	node.AlphaFadeEase = json.value("alpha_fade_ease", node.AlphaFadeEase);
	node.vAlphaFade.x = json.at("alpha_fade").at("x").get<_float>();
	node.vAlphaFade.y = json.at("alpha_fade").at("y").get<_float>();

	node.ScaleEase = json.value("scale_ease", node.ScaleEase);
	node.vStartScale.x = json.at("start_scale").at("x").get<_float>();
	node.vStartScale.y = json.at("start_scale").at("y").get<_float>();
	node.vStartScale.z = json.at("start_scale").at("z").get<_float>();
	node.vEndScale.x = json.at("end_scale").at("x").get<_float>();
	node.vEndScale.y = json.at("end_scale").at("y").get<_float>();
	node.vEndScale.z = json.at("end_scale").at("z").get<_float>();

	node.UVEase = json.value("uv_ease", node.UVEase);
	node.vStartUVOffset.x = json.at("start_uv_offset").at("x").get<_float>();
	node.vStartUVOffset.y = json.at("start_uv_offset").at("y").get<_float>();
	node.vEndUVOffset.x = json.at("end_uv_offset").at("x").get<_float>();
	node.vEndUVOffset.y = json.at("end_uv_offset").at("y").get<_float>();

	node.iCol = json.value("col", node.iCol);
	node.iRow = json.value("row", node.iRow);
	node.iMaxFrameIndex = json.value("max_frame_index", node.iMaxFrameIndex);

	node.DissolveEase = json.value("dissolve_ease", node.DissolveEase);
	node.fDissolveStartProgress = json.value("dissolve_start_progress", node.fDissolveStartProgress);

	return node;
}

EFFECT_ASSET Engine::tagEffectAsset::FromJson(nlohmann::ordered_json& json)
{
	EFFECT_ASSET Effect{};

	Effect.iNodeCount = json.value("node_count", Effect.iNodeCount);
	Effect.fDuration = json.value("duration", Effect.fDuration);
	Effect.isLoop = json.value("is_loop", Effect.isLoop);

	return Effect;
}
