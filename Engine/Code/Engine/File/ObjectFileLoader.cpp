#include "Engine\File\ObjectFileLoader.hpp"
#include "Engine\File\File.hpp"
#include <string>
#include "Engine\Core\StringUtils.hpp"
#include <stdio.h>
#include <stdlib.h>
#include "Engine\File\ObjectFileLoader.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\ErrorWarningAssert.hpp"
#include "Engine\Renderer\Mesh.hpp"
#include "Engine\Renderer\MeshBuilder.hpp"

void LoadObjectFromFile(const std::string & objFilePath, std::vector<Vector3>& outVertices, std::vector<float>& outIndicies, std::vector<Vector2>& outUVS, std::vector<Vector3>& outNormals)
{
	MeshBuilder mb;

	std::string buffer = (const char*) FileReadToNewBuffer(objFilePath.c_str());
	
	std::vector<std::string> lines = SplitStringOnCharacter(buffer, '\n');
	RemoveStringsStartingWithString(lines, "#");
	RemoveEmptyStrings(lines);

	for(int lineIndex = 0; lineIndex < (int)lines.size(); lineIndex++)
	{
		std::string identifierType = SplitStringOnFirstWord(lines[lineIndex]);

		if(identifierType == "v")
		{			
			Vector3 vertex;
			std::vector<std::string> tokens = SplitStringOnCharacter(lines[lineIndex], ' ');
			RemoveStringsStartingWithString(tokens, "v");
			RemoveEmptyStrings(tokens);

			vertex.x = -1 * ConvertStringToFloat(tokens[0]);
			vertex.y = ConvertStringToFloat(tokens[1]);
			vertex.z = ConvertStringToFloat(tokens[2]);

			outVertices.push_back(vertex);			
		}

		else if(identifierType == "vt")
		{			
			Vector2 uv;
			std::vector<std::string> tokens = SplitStringOnCharacter(lines[lineIndex], ' ');
			RemoveStringsStartingWithString(tokens, "vt");
			RemoveEmptyStrings(tokens);

			uv.x = ConvertStringToFloat(tokens[0]);
			uv.y = ConvertStringToFloat(tokens[1]);			

			outUVS.push_back(uv);			
		}

		else if(identifierType == "vn")
		{			
			Vector3 normal;
			std::vector<std::string> tokens = SplitStringOnCharacter(lines[lineIndex], ' ');
			RemoveStringsStartingWithString(tokens, "vn");
			RemoveEmptyStrings(tokens);

			normal.x = -1 * ConvertStringToFloat(tokens[0]);
			normal.y = ConvertStringToFloat(tokens[1]);
			normal.z = ConvertStringToFloat(tokens[2]);

			outNormals.push_back(normal);		
		}

		else if(identifierType == "f")
		{			
			std::vector<std::string> tokens = SplitStringOnCharacter(lines[lineIndex], ' ');
			RemoveStringsStartingWithString(tokens, "f");
			RemoveEmptyStrings(tokens);

			for(int tokenIndex = 0; tokenIndex < (int)tokens.size(); tokenIndex++)
			{
				std::vector<std::string> subTokens = SplitStringOnCharacter(tokens[tokenIndex], '/');

				for(int subTokenIndex = 0; subTokenIndex < (int)subTokens.size(); subTokenIndex)
				{
					/*mb.
					index = ConvertStringToInt(subTokens[subTokenIndex]);
					outIndicies.push_back(index);*/
				}
			}
		}
	}
//position, uv, normal.....start pushing at end of m_vertices array
	return;
}
