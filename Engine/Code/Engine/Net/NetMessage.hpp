#pragma once
#include "Engine\Core\BytePacker.hpp"

//forward dec netmessage for callback
class NetMessage;

typedef bool (*NetMessageCallback)(const NetMessage& message);

struct NetMessageHeader
{
	uint8_t m_messageCallbackDefinitionIndex = UCHAR_MAX;
};

struct NetMessageDefinition
{
	int m_callbackId = -1;
	std::string m_callbackName;
	NetMessageCallback m_callback = nullptr;
};

class NetMessage : public BytePacker
{
public:
	NetMessage();
	~NetMessage();

	void WriteBool(bool writeBool);
	void WriteInt(int writeInt);
	void WriteFloat(float writeFloat);
	//...

	/*uint8_t ReadByte();
	bool ReadBool();
	int ReadInt();
	float ReadFloat();*/
	//..

public:
	NetMessageHeader* m_header = nullptr;
	NetMessageDefinition* m_definition = nullptr;
};