#include "Engine\Core\WindowsCommon.hpp"
#include "Engine\File\FileHelpers.hpp"
#include <stdio.h>

//  =============================================================================
bool CreateFolder(const char* filePath)
{
	LPCWSTR convertedPath = ConvertToLPCWSTR(filePath);
	return CreateDirectory(convertedPath, NULL) ? true : false;
}

//  =============================================================================
LPCWSTR ConvertToLPCWSTR(const char* string)
{
	wchar_t wString[4096];
	MultiByteToWideChar(CP_ACP, 0, string, -1, wString, 4096);
	return wString;
}