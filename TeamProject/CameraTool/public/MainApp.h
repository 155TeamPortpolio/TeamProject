#pragma once

NS_BEGIN(CameraTool)

class MainApp : public CBase
{
public:
	void Init();
	void Update(float dt);
	void Render();

public:
	void Set_Levels();

private:
	CGameInstance*       game{};
	ID3D11Device*        dev{};
	ID3D11DeviceContext* ctx{};

public:
	static MainApp* Create();
	virtual void Free() override;
};

NS_END