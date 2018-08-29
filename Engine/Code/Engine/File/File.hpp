// Loads a file to a memory buffer
// Free using free(ptr); 
#pragma once
#include "Engine\Core\EngineCommon.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <fstream>


class File
{
public:
	File();
	//File(const char* fileName);
	File(const std::string& filePath);
	~File();

	bool IsOpen(){return m_file.is_open();}
	void Open();
	void Close();

	bool WriteToFileNewline(const std::string& output);
	bool WriteToFileNewline(const std::vector<std::string>& outputStrings);

	bool WriteToFile(const std::string& output);
	bool WriteToFile(const std::vector<std::string>& outputStrings);

	//std::string ReadLineFromFile();
	//bool CanReadFromFile();	

public:
	const char* m_fileName = nullptr;
	std::ofstream m_file;
};

void* FileReadToNewBuffer( char const *filename );

bool WriteToFileImmediate(const char* fileName, const std::vector<std::string>& outputs);
bool WriteToFileImmediate(const char* fileName, const std::string& output);

bool RemoveFile(const char* filePath);

//appends the working directory to the front of whatever the file is.  (COMMONLY "RUN_WIN32")
std::string ReturnFullPath(const char* fileName);