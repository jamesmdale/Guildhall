#pragma once

#include <string>
#include <vector>
#include "Engine\Math\Vector2.hpp"
#include "Engine\Math\Vector3.hpp"

void LoadObjectFromFile( const std::string& objFilePath, std::vector<Vector3>& outVertices, std::vector<float>& outIndicies, std::vector<Vector2>& outUVS, std::vector<Vector3>& outNormals);

