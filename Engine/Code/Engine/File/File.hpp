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
	File(const char* fileName);
	~File();

	bool IsOpen(){return m_file->is_open();}
	void Open();
	void Close();

	bool WriteToFile(const std::string& output);
	bool WriteToFile(const std::vector<std::string>& ouputStrings);

	//std::string ReadLineFromFile();
	//bool CanReadFromFile();	

public:
	const char* m_fileName = nullptr;
	std::fstream* m_file = nullptr;
};

void* FileReadToNewBuffer( char const *filename );

bool WriteToFileImmediate(const char* fileName, const std::vector<std::string>& outputs);
bool WriteToFileImmediate(const char* fileName, const std::string& output);

//appends the working directory to the front of whatever the file is.  (COMMONLY "RUN_WIN32")
std::string ReturnFullPath(const char* fileName);