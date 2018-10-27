#pragma once
#include "Engine\Core\Rgba.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\Command.hpp"
#include "Engine\Camera\Camera.hpp"
#include "Engine\Core\LogSystem.hpp"
#include "Engine\Utility\ThreadSafeQueue.hpp"
#include <vector>
#include <string>

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

	const float TEXT_CELL_HEIGHT = 15.f;
	const float REMOTE_TEXT_CELL_HEIGHT = 10.f;
	const float REMOTE_TEXT_CELL_PADDING = 15.f;
	const float TEXT_DRAW_PADDING_Y = 5.f;
	const float TEXT_DRAW_PADDING_X = 5.f;
	const float CURSOR_TIMER = 0.3f;

public:	

	// Handles all input
	void Update(float deltaSeconds);

	// Renders the display
	void Render(); 
	void RenderRemoteCommandService();
	void RenderNetSession();
	
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

	void AddNewHistoryItem(const HistoryItem& item);
	bool SaveSessionLog(std::string fileName);
	bool SaveCommandInputHistory(std::string fileName);
	bool LoadPreviousConsoleSession(std::string fileName);

	void Startup();
	void Shutdown();

	void ResetHistoryIndex(){m_currentHistoryItemIndex = -1;};
	void PopulateWithNextMostRecentHistoryItem(); //decrements the m_currentHistoryItemIndex vector and sets the item to the current input
	void PopulateWithPreviousHistoryItem(); //increments the m_currentHistoryItemIndex vector and sets the item to the current input

	void FlushConsoleQueue();
	void QueueMessage(const HistoryItem& item);

private: 
	bool m_isDevConsoleOpen = false;
	std::vector<HistoryItem> m_historyStack;
	std::vector<HistoryItem> m_inputHistoryStack;
	std::string m_currentInput;
	std::vector<int> m_allowableCharacters;

	//message queue
	ThreadSafeQueue<HistoryItem> m_messageQueue;

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
void DevConsolePrint(const Rgba & color, const std::string & formattedString);

//commands
void Help(Command& cmd);
void Clear(Command& cmd);
void SaveLog(Command& cmd);
void EchoWithColor(Command& cmd);

//log commands
void FlushLog(Command& cmd);
void EnableLogOuputToDevConsole(Command& cmd);
void DisableLogOuputToDevConsole(Command& cmd);
void LogBlacklistMode(Command& cmd);
void LogWhiteListMode(Command& cmd);
void LogShowTag(Command& cmd);
void LogHideTag(Command& cmd);
void SaveCommandHistory(Command& cmd);

//log system hooks
void WriteLogToDevconsole(const LogEntry& log, void* filePointer);



