#include "Game\Entity\Character.hpp"
#include "Game\Entity\Player.hpp"

int Character::s_characterIndexer = 0;

Character::Character()
{
}


Character::~Character()
{

}

void Character::RefreshRenderables()
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
