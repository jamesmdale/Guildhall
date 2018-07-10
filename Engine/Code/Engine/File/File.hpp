// Loads a file to a memory buffer
// Free using free(ptr); 
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "Engine\Core\EngineCommon.hpp"
#include <string>
#include <vector>

void* FileReadToNewBuffer( char const *filename );

bool WriteToFile(const char* fileName, const std::vector<std::string>& outputs);
bool WriteToFile(const char * fileName, const std::string & output);

//appends the working directory to the front of whatever the file is.  (COMMONLY "RUN_WIN32")
std::string ReturnFullPath(const char* fileName);