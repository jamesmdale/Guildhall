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

	/*void InitializeGameConfig();
	void InitializeTileDefinitions();
	void InitializeMapDefinitions();*/
};

extern TheApp* g_TheApp;
