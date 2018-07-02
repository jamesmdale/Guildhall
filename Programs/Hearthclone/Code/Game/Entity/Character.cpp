#include "Game\Entity\Character.hpp"

int Character::s_characterIndexer = 0;

Character::Character()
{
}


Character::~Character()
{

}

int Character::GenerateNewCharacterId()
{
	s_characterIndexer++;
	return s_characterIndexer;
}

void Character::ResetCharacterIndexer()
{
	s_characterIndexer = 0;
}
