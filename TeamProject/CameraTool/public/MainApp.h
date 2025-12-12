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
	ID3D11Device*        device{};
	ID3D11DeviceContext* context{};

public:
	static MainApp* Create();
	virtual void Free() override;
};

NS_END