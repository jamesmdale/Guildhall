#include "Engine\Core\DevConsole.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Window\Window.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Core\Command.hpp"
#include "Engine\File\File.hpp"
#include "Engine\Core\LogSystem.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Input\InputSystem.hpp"
#include "Engine\Renderer\Shader.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\LogSystem.hpp"
#include <ctime>
#include <stdarg.h>

static DevConsole* g_theDevConsole = nullptr;

//  =========================================================================================
bool DevConsoleMessageHandler( unsigned int wmMessageCode, size_t wParam, size_t lParam ) 
{
	UNUSED(lParam);
	if(g_theDevConsole->IsOpen())
	{
		unsigned char asKey = (unsigned char) wParam;

		switch( wmMessageCode )
		{			
			// process keys that don't pass to wmchar
			case WM_KEYDOWN:
			{		
				if(asKey == VK_LEFT) //left arrow
				{
					g_theDevConsole->DecrementCursorPosition();
				}

				if(asKey == VK_RIGHT)
				{
					g_theDevConsole->IncrementCursorPosition();
				}

				if(asKey == VK_UP) //left arrow
				{
					g_theDevConsole->PopulateWithPreviousHistoryItem();
				}

				if(asKey == VK_DOWN) //left arrow
				{
					g_theDevConsole->PopulateWithNextMostRecentHistoryItem();
				}

				if(asKey == VK_DELETE)
				{
					g_theDevConsole->RemoveCharacterAtNextIndex();
				}
				return false;
				break;				
			}
			case WM_CHAR:
			{
				if(asKey == '\x1b') //escape
				{
					std::string input = g_theDevConsole->GetCurrentInput();
					if(input != "")
					{
						g_theDevConsole->ClearInput();
					}
					else
					{
						g_theDevConsole->Close();
					}								
				}
				else if(asKey == '\r') //return
				{
					g_theDevConsole->ExecuteInput();
				}
				else if(asKey == '\b')
				{
					g_theDevConsole->RemoveCharacterAtIndex();
				}
				//else if(asKey = )
				else
				{					
					if(g_theDevConsole->CheckIfValidInput((int)asKey))
					{
						g_theDevConsole->AppendCharacterToInput(asKey);
					}					
				}				
				return false;
				break;
			}
		}
	}

	return true;	
}

//  =========================================================================================
DevConsole::DevConsole()
{
	SetAllowableCharacters();

	Window::GetInstance()->AddHandler( DevConsoleMessageHandler ); 
}

//  =========================================================================================
void DevConsole::Startup()
{
	//register commands
	CommandRegister("help", CommandRegistration(Help, ": Use to show all supported commands", "All commands displayed!"));
	CommandRegister("clear", CommandRegistration(Clear, ": Use to delete all dev console input history", "History cleared!"));
	CommandRegister("log_console_session", CommandRegistration(SaveLog, ": Use to save current console session to Data/Log", "Session logged!"));
	CommandRegister("echo_with_color", CommandRegistration(EchoWithColor, ": Use to print supplied text with chosen color in console", "ECHOED! Echoed! echoed...trails off"));

	//log registrations
	CommandRegister("log_flush", CommandRegistration(FlushLog,": force flushes log messages", "Log flushed!"));
	CommandRegister("log_enable", CommandRegistration(EnableLogOuputToDevConsole, ": prints contents to devconsole from log", "Showing log output!"));
	CommandRegister("log_disable", CommandRegistration(DisableLogOuputToDevConsole, ": discontinues prints of content to devconsole from log", "Hiding log output!"));
	CommandRegister("log_blacklist", CommandRegistration(LogBlacklistMode, ": only prints items that don't match tags in list"));
	CommandRegister("log_whitelist", CommandRegistration(LogWhiteListMode, ": only prints items that match tags in list"));
	CommandRegister("log_show_tag", CommandRegistration(LogShowTag, ": adds tags to be shown in print"));
	CommandRegister("log_hide_tag", CommandRegistration(LogHideTag, ": adds tags to be hidden in print"));

	//set camera info
	m_consoleCamera = new Camera();
	m_consoleCamera->SetColorTarget(Renderer::GetInstance()->GetDefaultRenderTarget());

	m_consoleCamera->SetOrtho(0.f, Window::GetInstance()->m_clientWidth, 0.f, Window::GetInstance()->m_clientHeight, -1.f, 1.f);
	m_consoleCamera->SetView(Matrix44::IDENTITY);
}

//  =========================================================================================
void DevConsole::Shutdown()
{
	delete(g_theDevConsole);
	g_theDevConsole = nullptr;
}

//  =========================================================================================
void DevConsole::PopulateWithNextMostRecentHistoryItem()
{
	int tempHistoryIndex = m_currentHistoryItemIndex;
	tempHistoryIndex--;

	if(m_currentHistoryItemIndex < 0)
		return;

	for(int historyIndex = tempHistoryIndex; historyIndex > -1; historyIndex--)
	{
		if(m_inputHistoryStack[historyIndex].m_isCommand)
		{
			m_currentInput = m_inputHistoryStack[historyIndex].m_printText;
			m_currentHistoryItemIndex = historyIndex;
			m_cursorPosition = (int)m_currentInput.length();
			return;
		}
	}
	 
	//if we got here nothing was found.  let's put the current history index to the max size of the list and loop again over the ones we missed just in case.
	//if we still don't find anything, there are no legitimate commands in the list.
	for(int historyIndex = (int)m_inputHistoryStack.size(); historyIndex > tempHistoryIndex; historyIndex--)
	{
		if(m_inputHistoryStack[historyIndex].m_isCommand)
		{
			m_currentInput = m_inputHistoryStack[historyIndex].m_printText;
			m_currentHistoryItemIndex = historyIndex;
			m_cursorPosition = (int)m_currentInput.length();
			return;
		}
	}
}

//  =========================================================================================
void DevConsole::PopulateWithPreviousHistoryItem()
{
	int tempHistoryIndex = m_currentHistoryItemIndex;
	tempHistoryIndex++;

	for(int historyIndex = tempHistoryIndex; historyIndex < (int)m_inputHistoryStack.size(); historyIndex++)
	{
		if(m_inputHistoryStack[historyIndex].m_isCommand)
		{
			m_currentInput = m_inputHistoryStack[historyIndex].m_printText;
			m_currentHistoryItemIndex = historyIndex;
			m_cursorPosition = (int)m_currentInput.length();
			return;
		}
	}

	//if we got here nothing was found.  let's put the current history index to the first index of the list and loop again over the ones we missed just in case.
	//if we still don't find anything, there are no legitimate commands in the list.
	for(int historyIndex = 0; historyIndex < tempHistoryIndex; historyIndex++)
	{
		if(m_inputHistoryStack[historyIndex].m_isCommand)
		{
			m_currentInput = m_inputHistoryStack[historyIndex].m_printText;
			m_currentHistoryItemIndex = historyIndex;
			m_cursorPosition = (int)m_currentInput.length();
			return;
		}
	}
}

// add queued messages to console =========================================================================================
void DevConsole::FlushConsoleQueue()
{
	HistoryItem entry;

	//console queue will flush 500 max at a time.
	int maxflush = (int)m_messageQueue.GetSize() > 500 ? 500 : (int)m_messageQueue.GetSize();
	
	for(int flushIndex = 0; flushIndex < maxflush; ++flushIndex)
	{		
		m_messageQueue.dequeue(&entry);
		AddNewHistoryItem(entry);
	}
}

void DevConsole::QueueMessage(const HistoryItem & item)
{
	m_messageQueue.enqueue(item);
}

// Handles all input =========================================================================================
void DevConsole::Update(float deltaSeconds)
{
	UpdateCursorTimer(deltaSeconds);
	FlushConsoleQueue();
}

// renders all to display =========================================================================================
void DevConsole::Render()
{
	Window* theWindow = Window::GetInstance();
	Renderer* theRenderer = Renderer::GetInstance();
	AABB2 consoleBounds = AABB2(0.f, 0.f, theWindow->m_clientWidth, theWindow->m_clientHeight);
	AABB2 consoleInputBounds = AABB2(0.f, 0.f, theWindow->m_clientWidth, TEXT_CELL_HEIGHT + TEXT_DRAW_PADDING_Y);
	
	int maxIndexToRender = RoundToNearestInt(theWindow->m_clientHeight/(TEXT_CELL_HEIGHT + TEXT_DRAW_PADDING_Y));

	theWindow = nullptr;
	delete(theWindow);

	std::string modifiedInputWithCursor = m_currentInput;
	if(m_doesShowCursor)
	{
		modifiedInputWithCursor.insert(m_cursorPosition, "|");
	}

	theRenderer->SetCamera(m_consoleCamera);
	
	theRenderer->ClearDepth(1.0f);
	theRenderer->EnableDepth(ALWAYS_DEPTH_TYPE, true);	

	theRenderer->SetTexture(*theRenderer->m_defaultTexture);
	theRenderer->SetShader(theRenderer->m_defaultShader);

	theRenderer->m_defaultShader->EnableColorBlending(BLEND_OP_ADD, BLEND_SOURCE_ALPHA, BLEND_ONE_MINUS_SOURCE_ALPHA);


	TODO("Convert devconsole to use meshes.");
	//MeshBuilder meshBuilder;

	/*meshBuilder.CreateQuad2D(consoleBounds, Rgba(0.f, 0.f, 0.f, .85f));
	theRenderer->DrawMesh(meshBuilder.CreateMesh<VertexPCU>());

	meshBuilder.CreateQuad2D(consoleInputBounds, Rgba(.5f, .5f, .5f, .90f));
	theRenderer->DrawMesh(meshBuilder.CreateMesh<VertexPCU>());*/

	theRenderer->DrawAABB(consoleBounds, Rgba(0.f, 0.f, 0.f, .85f));
	theRenderer->DrawAABB(consoleInputBounds, Rgba(.5f, .5f, .5f, .90f));

	theRenderer->DrawText2D(Vector2(TEXT_DRAW_PADDING_X, 0.f), modifiedInputWithCursor, TEXT_CELL_HEIGHT, Rgba::WHITE, 1.f, Renderer::GetInstance()->CreateOrGetBitmapFont("SquirrelFixedFont"));

	for(int historyIndex = 0; historyIndex < (int)m_inputHistoryStack.size(); historyIndex++)
	{
		if(historyIndex > maxIndexToRender)
		{
			break;
		}		

		float currentCellStartPosition = (float)(historyIndex + 1.f) * (TEXT_CELL_HEIGHT + TEXT_DRAW_PADDING_Y);
		theRenderer->DrawText2D(Vector2(TEXT_DRAW_PADDING_X, currentCellStartPosition), m_inputHistoryStack[historyIndex].m_printText, 15.f, m_inputHistoryStack[historyIndex].m_printColor, 1.f, Renderer::GetInstance()->CreateOrGetBitmapFont("SquirrelFixedFont"));
	}

	theRenderer->m_defaultShader->DisableBlending();

	theRenderer = nullptr;
}

// =========================================================================================
void DevConsole::Open()
{
	m_isDevConsoleOpen = true;
	m_currentInput = "";
	m_cursorPosition = 0;
	m_inputHistoryStack.clear();
	ResetHistoryIndex();
}

// =========================================================================================
void DevConsole::Close()
{
	m_isDevConsoleOpen = false;
	m_currentInput = "";
	m_cursorPosition = 0;
	m_inputHistoryStack.clear();

	ResetHistoryIndex();
}

// =========================================================================================
DevConsole* DevConsole::CreateInstance() 
{
	if (g_theDevConsole == nullptr) 
	{
		g_theDevConsole = new DevConsole(); 
	}
	return g_theDevConsole; 
}

// =========================================================================================
DevConsole* DevConsole::GetInstance()
{
	return g_theDevConsole; 
}

// Should add a line of coloured text to the output =========================================================================================
void DevConsolePrintf( Rgba const &color, char const* format, ...)
{
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	g_theDevConsole->QueueMessage( HistoryItem(color, std::string(textLiteral), false));
}

// Same as previous, be defaults to a color visible easily on your console =========================================================================================
void DevConsolePrintf( char const *format,  ...)
{
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	g_theDevConsole->QueueMessage( HistoryItem(Rgba::WHITE, std::string(textLiteral), false));
}

void DevConsolePrint(const Rgba& color, const std::string& formattedString)
{
	g_theDevConsole->QueueMessage( HistoryItem(Rgba::WHITE, formattedString, false));
}

//  =========================================================================================
void DevConsole::AddNewHistoryItem(const HistoryItem& item)
{
	if ((int)m_inputHistoryStack.size() > MAX_MAX_CONSOLE_SIZE)
	{
		m_inputHistoryStack.erase(m_inputHistoryStack.end() - 1);
	}

	m_inputHistoryStack.insert(m_inputHistoryStack.begin(), item);
}

//  =========================================================================================
void DevConsole::AppendCharacterToInput(unsigned char asKey)
{	
	m_currentInput.insert(m_cursorPosition, Stringf("%c", asKey));	

	IncrementCursorPosition();
	ResetHistoryIndex();
}

//  =========================================================================================
void DevConsole::ExecuteInput()
{
	if(m_currentInput != "")
	{			
		bool isValidCommand = CommandRun(m_currentInput.c_str());
		g_theDevConsole->AddNewHistoryItem(HistoryItem(Rgba::WHITE, m_currentInput, isValidCommand));
		ClearInput();
		ResetHistoryIndex();
	}
}

//  =========================================================================================
void DevConsole::RemoveCharacterAtIndex()
{
	if((int)m_currentInput.size() != 0 && m_cursorPosition > 0)
	{
		m_currentInput.erase(m_cursorPosition - 1, 1);
	}
	
	if(m_cursorPosition > 0)
	{
		DecrementCursorPosition();
	}
	
	
	m_currentInput.shrink_to_fit(); //makes sure the string size is correct after removal	
	ResetHistoryIndex();
}

//  =========================================================================================
void DevConsole::RemoveCharacterAtNextIndex()
{
	if((int)m_currentInput.size() != 0 && (int)m_currentInput.size() > m_cursorPosition)
	{
		m_currentInput.erase(m_cursorPosition, 1);
	}

	m_currentInput.shrink_to_fit(); //makes sure the string size is correct after removal	
}

//  =========================================================================================
void DevConsole::ClearInput()
{
	m_currentInput = "";
	m_cursorPosition = 0;
	ResetHistoryIndex();
}

//  =========================================================================================
void DevConsole::ClearHistory()
{
	m_inputHistoryStack.clear();
	ResetHistoryIndex();
}

//  =========================================================================================
void DevConsole::UpdateCursorTimer(float deltaSeconds)
{
	m_cursorCooldown += deltaSeconds;
	if(m_cursorCooldown > CURSOR_TIMER)
	{
		m_doesShowCursor = !m_doesShowCursor;
		m_cursorCooldown = 0.f;
	}
}

//  =========================================================================================
void DevConsole::DecrementCursorPosition()
{
	if(m_cursorPosition > 0)
	{
		m_cursorPosition--;
	}
}

void DevConsole::IncrementCursorPosition()
{
	if(m_cursorPosition < m_currentInput.size())
	{
		m_cursorPosition++;
	}
}

//  =========================================================================================
bool DevConsole::SaveSessionLog(std::string fileName)
{
	TODO("Update with time and date.  Could also allow user to input filepath and name if that is useful later");
	
	std::string filePath = Stringf("%s%s%s", "Data\\ConsoleLogs\\", fileName.c_str(), ".txt");
	std::vector<std::string> consoleInputText;

	for(int itemIndex = 0; itemIndex < (int)m_inputHistoryStack.size(); itemIndex++)
	{
		consoleInputText.push_back(m_inputHistoryStack[itemIndex].m_printText);
	}

	bool didSucceed = WriteToFileImmediate(filePath.c_str(), consoleInputText);

	return didSucceed;
}

//  =========================================================================================
void DevConsole::SetAllowableCharacters()
{
	//add numerics
	for(int i = 48; i <= 57; i++)
		m_allowableCharacters.push_back(i);

	//add capital letters
	for(int i = 65; i <= 90; i++ )
		m_allowableCharacters.push_back(i);

	//add lower case letters
	for(int i = 97; i <= 122; i++ )
		m_allowableCharacters.push_back(i);

	//add all other accepted cases	
	m_allowableCharacters.push_back(32); //space
	m_allowableCharacters.push_back(34); //"
	m_allowableCharacters.push_back(40);//(
	m_allowableCharacters.push_back(41);//)
	m_allowableCharacters.push_back(44);//,
	m_allowableCharacters.push_back(46);//.
	m_allowableCharacters.push_back(95);//_
}

//  =========================================================================================
bool DevConsole::CheckIfValidInput(int asKey)
{
	for(int characterIndex = 0; characterIndex < (int)m_allowableCharacters.size(); characterIndex++) 
	{
		if(asKey == m_allowableCharacters[characterIndex])
		{
			return true;
		}
	}

	return false;
}

//  =========================================================================================
// Devconsole related commands =========================================================================================
//  =========================================================================================

//  =========================================================================================
void Help(Command &cmd)
{
	if(!cmd.IsCorrectNumberOfParameters(0))
	{
		DevConsolePrintf(Rgba::RED, "INVALID: Incorrect number of parameters for command %s", cmd.GetName());
		return;
	}

	std::vector<std::string> commandListFormatted = GetRegisteredCommandList();

	for(int commandIndex = 0; commandIndex < (int)commandListFormatted.size(); commandIndex++)
	{
		DevConsolePrintf(commandListFormatted[commandIndex].c_str());
	}

	DevConsolePrintf(cmd.m_commandInfo->m_successMessage.c_str());
}

//  =========================================================================================
void Clear(Command &cmd)
{
	//used if you care about having correct parameter numbers
	if(!cmd.IsCorrectNumberOfParameters(0))
	{
		DevConsolePrintf(Rgba::RED, "INVALID: Incorrect number of parameters for command %s", cmd.GetName());
		return;
	}

	DevConsole::GetInstance()->ClearHistory();
	DevConsolePrintf(cmd.m_commandInfo->m_successMessage.c_str());
}

//  =========================================================================================
void SaveLog(Command &cmd)
{
	if(!cmd.IsCorrectNumberOfParameters(1))
	{
		DevConsolePrintf(Rgba::RED, "INVALID: Incorrect number of parameters for command %s", cmd.GetName());
		return;
	}

	std::string fileName = cmd.GetNextString();
	int txtTypeLocation = (int)fileName.find("\"");
	if(txtTypeLocation != -1)
	{
		DevConsolePrintf(Rgba::RED, "INVALID: File names cannot include quotes");
		return;
	}

	bool success = DevConsole::GetInstance()->SaveSessionLog(fileName);
	if(success)
	{
		DevConsolePrintf(cmd.m_commandInfo->m_successMessage.c_str());
	}
	else
	{
		DevConsolePrintf(Rgba::RED, "ERROR: Error occured when writing to file (check path)");
	}
}

//  =========================================================================================
void EchoWithColor(Command &cmd)
{
	if(!cmd.IsCorrectNumberOfParameters(2))
	{
		DevConsolePrintf(Rgba::RED, "INVALID: Incorrect number of parameters for command %s", cmd.GetName());
		return;
	}
	Rgba color;
	std::string message;
	try 
	{
		color = cmd.GetNextColor();
	}
	catch(const std::invalid_argument& e)
	{
		UNUSED(e);
		DevConsolePrintf(Rgba::RED, "INVALID: Could not read RGBA (Formatting \"(R,G,B,A)\" or \"(R,G,B)\" 0-255)");
		return;
	}

	message = cmd.GetNextString();
	if(message == "")
	{
		DevConsolePrintf(Rgba::RED, "INVALID: Could not read echo string (spaces are allowed if in \"\")");
		return;
	}

	DevConsolePrintf(color, message.c_str());
}

//  =========================================================================================
void FlushLog(Command& cmd)
{
	LogSystem::GetInstance()->FlushLoop();
	DevConsolePrintf("Log cleared!");
}

//  =========================================================================================
void EnableLogOuputToDevConsole(Command& cmd)
{
	//register
	LogSystem::GetInstance()->HookIntoLog(WriteLogToDevconsole, nullptr);
	DevConsolePrintf("Printing log to console!");
}

//  =========================================================================================
void DisableLogOuputToDevConsole(Command& cmd)
{
	//register
	LogSystem::GetInstance()->UnhookIntoLog(WriteLogToDevconsole, nullptr);
	DevConsolePrintf("Hiding log print from console!");
}

//  =========================================================================================
void LogBlacklistMode(Command & cmd)
{
	LogSystem::GetInstance()->LogBlacklistTags();
	DevConsolePrintf("Enabled blacklist mode for log tags. (Exclued items that match tags in list)");
	DevConsolePrintf("Clearing tag list...");
}

//  =========================================================================================
void LogWhiteListMode(Command & cmd)
{
	LogSystem::GetInstance()->LogWhitelistTags();
	DevConsolePrintf("Enabled whitelist mode for log tags (Only shows items that match tags in list)");
	DevConsolePrintf("Clearing tag list...");
}

//  =========================================================================================
void LogShowTag(Command & cmd)
{
	std::string tag = cmd.GetNextString();

	if (tag != "")
	{
		LogSystem::GetInstance()->LogShowTag(tag.c_str());

		eLogTagMode mode = LogSystem::GetInstance()->GetLogMode();
		
		if(mode == TAG_MODE_BLACKLIST)
			DevConsolePrintf("Removed tag from blacklist!");

		if (mode == TAG_MODE_WHITELIST)
			DevConsolePrintf("Added tag to whitelist!");
	}
	else
	{
		DevConsolePrintf(Rgba::RED, "Tag required (ex: log_hide_tag TAGNAME)");
	}	
}

//  =========================================================================================
void LogHideTag(Command & cmd)
{
	std::string tag = cmd.GetNextString();

	if (tag != "")
	{
		LogSystem::GetInstance()->LogHideTag(tag.c_str());

		eLogTagMode mode = LogSystem::GetInstance()->GetLogMode();

		if (mode == TAG_MODE_WHITELIST)
			DevConsolePrintf("Removed tag from blacklist!");

		if (mode == TAG_MODE_BLACKLIST)
			DevConsolePrintf("Added tag to whitelist!");
	}
	else
	{
		DevConsolePrintf(Rgba::RED, "Tag required (ex: log_hide_tag TAGNAME)");
	}
}


//  =========================================================================================
//  Log System Hooks =========================================================================================
//  =========================================================================================
void WriteLogToDevconsole(const LogEntry& log, void * filePointer)
{
	if(DevConsole::GetInstance()->IsOpen())
	{
		std::string tag = ToLowerAsNew(log.m_tag.c_str());

		Rgba messageColor = Rgba::WHITE;

		if(tag == "error")
			messageColor = Rgba::RED;
		else if (tag == "warning")
			messageColor = Rgba::YELLOW;
	
		std::string output = Stringf("%s: %s", tag.c_str(), log.m_text.c_str());
		DevConsolePrint(messageColor, output);
		//DevConsolePrintf(messageColor, "Logger - %s: %s", tag, log.m_text.c_str());
	}
}


