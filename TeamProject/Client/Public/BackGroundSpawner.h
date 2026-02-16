#pragma once
NS_BEGIN(Client)
class CBackGroundSpawner 
{
public:
	void CreatePedestrian(string AreaTag);

private:
	vector<OBJECT_HANDLE> pedestrinaHandles;
};

NS_END