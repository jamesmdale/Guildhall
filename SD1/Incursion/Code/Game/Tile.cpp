#include "GameCommon.hpp"
#include "Tile.hpp"
void Update()
{
	
}

void Tile::Render()
{	
	Rgba color;
	if(m_tileDefinition->m_tileType == grass)
	{		
		color = Rgba(0.f,1.f,0.f,1.f);
	}
	if(m_tileDefinition->m_tileType == stone)
	{
		color = Rgba(.8f, .8f, .8f, 1.f);
	}

	g_theRenderer->DrawTexturedAABB(m_tileBounds, *(TileDefinition::s_spriteSheet->GetSpriteSheetTexture()), m_tileDefinition->m_textureAABB2.mins, m_tileDefinition->m_textureAABB2.maxs, Rgba(1.f, 1.f, 1.f, 1.f));
}