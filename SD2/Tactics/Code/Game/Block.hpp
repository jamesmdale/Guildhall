#pragma 
#include "Game\BlockDefinition.hpp"

class Block
{
public:
	Block(){};

	int m_index = -1;
	bool m_isEmpty = true;
	BlockDefinition* m_definition;
};