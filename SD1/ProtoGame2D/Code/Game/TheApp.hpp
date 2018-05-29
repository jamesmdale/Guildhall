#pragma once
class TheApp
{
public:
	TheApp();
	~TheApp();
	void Update();
	void Render();
	void Initialize();
	void RunFrame();
	bool IsQuitting();
};

extern TheApp* g_TheApp;
