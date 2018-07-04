#pragma once
#include <vector>
#include <string>
#include "Engine\Core\Rgba.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\Command.hpp"
#include "Engine\Camera\Camera.hpp"


struct HistoryItem
{
public:
	HistoryItem(Rgba color = Rgba(1.f, 1.f, 1.f, 1.f), std::string printText = "", bool isCommand = false)
	{
		m_printColor = color;
		m_printText = printText;
		m_isCommand = isCommand;
	};

	Rgba m_printColor = Rgba(1.f, 1.f, 1.f, 1.f);
	std::string m_printText = "";
	bool m_isCommand = false;
};


class DevConsole 
{
private:
	DevConsole();

	const float TEXT_CELL_HEIGHT = 20.f;
	const float TEXT_DRAW_PADDING_Y = 5.f;
	const float TEXT_DRAW_PADDING_X = 5.f;
	const float CURSOR_TIMER = 0.3f;

public:	

	// Handles all input
	void Update(float deltaSeconds);

	// Renders the display
	void Render(); 

	void Open(); 
	void Close(); 
	bool IsOpen(){return m_isDevConsoleOpen;}; 
	std::string GetCurrentInput(){return m_currentInput;};
	void AppendCharacterToInput(unsigned char asKey);
	void ClearInput();
	void ClearHistory();
	void ExecuteInput();
	void RemoveCharacterAtIndex();
	void RemoveCharacterAtNextIndex();
	void UpdateCursorTimer(float deltaSeconds);
	void DecrementCursorPosition();
	void IncrementCursorPosition();
	void SetAllowableCharacters();
	bool CheckIfValidInput(int asKey);
	void AddNewHistoryItem(HistoryItem item);
	bool SaveSessionLog(std::string fileName);

	void Startup();
	void Shutdown();

	void ResetHistoryIndex(){m_currentHistoryItemIndex = -1;};
	void PopulateWithNextMostRecentHistoryItem(); //decrements the m_currentHistoryItemIndex vector and sets the item to the current input
	void PopulateWithPreviousHistoryItem(); //increments the m_currentHistoryItemIndex vector and sets the item to the current input

private: 
	bool m_isDevConsoleOpen = false;
	std::vector<HistoryItem> m_inputHistoryStack;
	std::string m_currentInput;
	std::vector<int> m_allowableCharacters;

	//cursor data
	int m_cursorPosition = 0;
	float m_cursorCooldown = 0.0; //amount of time between hide/show blinking cursor.
	bool m_doesShowCursor = false;
	int m_currentHistoryItemIndex = -1;

public:
	static DevConsole* GetInstance();
	static DevConsole* CreateInstance();
	Camera* m_consoleCamera;
};


// Global functions
// Should add a line of coloured text to the output 
void DevConsolePrintf( Rgba const &color, char const* format, ...); 

// Same as previous, be defaults to a color visible easily on your console
void DevConsolePrintf( char const* format, ...); 

void Help(Command &cmd);
void Clear(Command &cmd);
void SaveLog(Command &cmd);
void EchoWithColor(Command &cmd);
