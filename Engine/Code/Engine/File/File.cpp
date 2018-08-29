#include "Engine\File\File.hpp"
#include "Engine\Core\WindowsCommon.hpp"
#include <stdio.h>
#include <chrono>
#include <ctime>
#include "Engine\Core\StringUtils.hpp"
#include <fstream>

//  =============================================================================
File::File()
{
}

//  =============================================================================
//File::File(const char* fileName)
//{
//	m_fileName = fileName;
//	m_file = std::ofstream(fileName);
//}

//  =============================================================================
File::File(const std::string& filePath)
{
	m_fileName = filePath.c_str();
	m_file = std::ofstream(filePath);
}

//  =============================================================================
File::~File()
{
	delete(m_fileName);
	m_fileName = nullptr;
}

//  =============================================================================
void File::Open()
{
	if(!IsOpen())
		m_file.open(m_fileName);
}

//  =============================================================================
void File::Close()
{
	if(IsOpen())
		m_file.close();
}

//  =============================================================================
bool File::WriteToFileNewline(const std::string& output)
{
	if (!IsOpen())
		return false;
		
	m_file << Stringf("%s\n", output.c_str());
	
	return true;
}

//  =============================================================================
bool File::WriteToFileNewline(const std::vector<std::string>& outputStrings)
{
	if (!IsOpen())
		return false;

	for(int outputIndex = 0; outputIndex < (int)outputStrings.size(); ++outputIndex)
	m_file << Stringf("%s\n", outputStrings[outputIndex].c_str());

	return true;
}

//  =============================================================================
bool File::WriteToFile(const std::string & output)
{
	if (!IsOpen())
		return false;

	m_file << Stringf("%s", output.c_str());

	return true;
}

//  =============================================================================
bool File::WriteToFile(const std::vector<std::string>& outputStrings)
{
	if (!IsOpen())
		return false;

	for (int outputIndex = 0; outputIndex < (int)outputStrings.size(); ++outputIndex)
		m_file << Stringf("%s", outputStrings[outputIndex].c_str());

	return true;
}

//std::string File::ReadLineFromFile()
//{
//	//return false;
//	return "";
//}
//
//bool File::CanReadFromFile()
//{
//	//return this->get()
//	return 
//}

//  =============================================================================
// Static Functions  =============================================================================
//  =============================================================================
void* FileReadToNewBuffer( char const *filename )
{
	FILE *fp = nullptr;
	errno_t errorVal = fopen_s( &fp, filename, "r" );	
	UNUSED(errorVal); //useful when step-debugging just to check error code

	if (fp == nullptr) {
		return nullptr;
	}

	size_t size = 0U;
	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	byte_t *buffer = (byte_t*) malloc(size + 1U); // space for NULL

	size_t read = fread( buffer, 1, size, fp );
	fclose(fp);

	buffer[read] = NULL;

	return buffer; 
}

//  =============================================================================
bool WriteToFileImmediate(const char* fileName, const std::vector<std::string>& outputs)
{
	std::ofstream writer(fileName);
	if (!writer.is_open())
		return false;

	for(int outputIndex = 0; outputIndex < (int)outputs.size(); outputIndex++)
	{
		writer << Stringf("%s\n", outputs[outputIndex].c_str());
	}	
	writer.close();

	return true;	
}

//  =============================================================================
bool WriteToFileImmediate(const char* fileName, const std::string& output)
{
	std::ofstream writer(fileName);

	if (!writer.is_open())
		return false;	

	writer << Stringf("%s\n", output.c_str());
	writer.close();
	return true;		
}

bool RemoveFile(const char* filePath)
{
	int deleteSuccess = remove(filePath);

	//if deleteSuccess == 0, the file was deleted.
	return deleteSuccess == 0;
}

//  =============================================================================
std::string ReturnFullPath(const char* fileName)
{
	const int fileLength = 512;
	TCHAR currentDir[fileLength];   
	char dirInChar[fileLength];
	GetCurrentDirectory(fileLength, currentDir);

	int loopIndex = 0;
	int charactersToProcess = fileLength - 1;
	for(loopIndex; charactersToProcess; loopIndex++)
	{
		if((char)currentDir[loopIndex] == '\0')
		{

			break;
		}
		dirInChar[loopIndex] = (char)currentDir[loopIndex];
	}
	std::string currentWorkingDir(dirInChar);
	currentWorkingDir = currentWorkingDir.erase(loopIndex);
	std::string fileNameAsString(fileName);
	std::string searchString = "/";
	std::string replaceString = "\\";

	size_t pos = 0;
	while ((pos = fileNameAsString.find(searchString, pos)) != std::string::npos)
	{
		fileNameAsString.replace(pos, searchString.length(), replaceString);
		pos += replaceString.length();
	}

	std::string filepath = currentWorkingDir.append("\\").append(fileNameAsString);

	return filepath;
}

