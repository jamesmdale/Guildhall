#include "Engine\File\File.hpp"
#include <windows.h>
#include <stdio.h>
#include <chrono>
#include <ctime>
#include "Engine\Core\StringUtils.hpp"

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

bool WriteToFile(const char* fileName, std::vector<std::string> outputs)
{
	FILE* fp;
	fopen_s(&fp, fileName, "w+");
	fputs("*************************************************************", fp);
	for(int outputIndex = 0; outputIndex < (int)outputs.size(); outputIndex++)
	{
		int success = fputs((Stringf("%s%s", outputs[outputIndex].c_str(), "\n")).c_str(), fp);
		if(success < 0)
		{
			return false;
		}
	}
	fputs("*************************************************************", fp);
	
	fclose(fp);

	return true;
}

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