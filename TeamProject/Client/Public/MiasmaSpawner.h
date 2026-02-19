#pragma once
NS_BEGIN(Client)
enum class MiasmaType {Heavy, Grandier, Blade, Weapon};
class CMiasmaSpawner
{
public:
	void Spawn(MiasmaType type, _int count, _float3 Target, _float3 Owner, _float y, class CDefiler* pDefiler);
	void SpawnGrandier(_int count, _float3 Target, _float3 Owner, _float y);
	void SpawnBlade( _float3 Target, _float3 Owner, class CDefiler* pDefiler);
	void SpawnHeavy( _float3 Target, _float3 Owner);
	void SpawnWeapon( _float3 Target, _float3 Owner, class CDefiler* pDefiler);

public:
	_float3 ComputeArcSpawnPos(const _float3& ownerPos, const _float3& targetPos, _float minRadius, _float maxRadius, _float arcDegrees, _float yFixed);
	_float3 ComputeParabolarPos(const _float3& ownerPos, const _float3& targetPos);
	_float3 ComputeCircular(const _float3& centerPos,_float radius);

private:
	_bool m_parabolLeft = {};
	_uint m_WeaponThrowCount = {};
	_uint m_MiasmaBladeCount = {};
};

NS_END
