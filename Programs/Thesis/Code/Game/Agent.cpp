#include "Engine\Core\Transform2D.hpp"
#include "Game\Agent.hpp"
#include "Game\GameStates\GameState.hpp"
#include "Game\Map\Map.hpp"
#include "Game\GameCommon.hpp"
#include "Game\GameStates\PlayingState.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Engine\Window\Window.hpp"



Agent::Agent()
{
}


Agent::Agent(Vector2 startingPosition, IsoSpriteAnimSet* animationSet, Map* mapReference)
{
	m_position = startingPosition;
	m_animationSet = animationSet;
	m_currentMap = mapReference;

	m_goalLocation = m_position;

	m_animationSet->SetCurrentAnim("walk");
}

Agent::~Agent()
{
	m_currentMap = nullptr;
}

void Agent::Update(float deltaSeconds)
{
	//if we have a path, follow it.
	if (m_currentPath.size() > 0)
	{
		if (m_currentMap->GetTileCoordinateOfPosition(m_position) != m_currentMap->GetTileCoordinateOfPosition(m_currentPath.at(m_currentPath.size() - 1)))
		{
			m_goalLocation = m_currentPath.at(m_currentPath.size() - 1);

			m_forward = m_goalLocation - m_position;
			m_forward.NormalizeAndGetLength();
		}
		else
		{
			//if we are down to our final destination and we are in the same tile, just snap to the location in that tile
			if (m_currentPath.size() == 1)
			{
				m_position = m_currentPath.at(m_currentPath.size() - 1);
			}

			m_currentPath.erase(m_currentPath.end() - 1);

			if(m_currentPath.size() != 0)
				m_goalLocation = m_currentPath.at(m_currentPath.size() - 1);

			m_forward = m_goalLocation - m_position;
			m_forward.NormalizeAndGetLength();
		}

		m_position += (m_forward * (m_movespeed * deltaSeconds));
	}	

	m_transform.SetLocalPosition(m_position);
	UpdateSpriteRenderDirection();

	m_animationSet->Update(deltaSeconds);
}

void Agent::Render()
{
	Renderer* theRenderer = Renderer::GetInstance();

	Sprite sprite = *m_animationSet->GetCurrentSprite(m_spriteDirection);

	float dimensions = Window::GetInstance()->GetClientWidth() * g_tilePercentageOfWindow;

	Texture* texture = sprite.GetSpriteTexture();
	Vector2 spritePivot = sprite.m_definition->m_pivot;

	IntVector2 spriteDimensions = sprite.GetDimensions();

	AABB2 bounds;
	bounds.mins.x = 0.f - (spritePivot.x) * dimensions;
	bounds.maxs.x = bounds.mins.x + 1.f * dimensions;
	bounds.mins.y = 0.f - (spritePivot.y) * dimensions;
	bounds.maxs.y = bounds.mins.y + 1.f * dimensions;

	theRenderer->SetShader(theRenderer->CreateOrGetShader("agents"));
	theRenderer->DrawTexturedAABB(m_transform.GetWorldMatrix(), bounds, *theRenderer->CreateOrGetTexture(sprite.m_definition->m_diffuseSource), Vector2(sprite.GetNormalizedUV().mins.x, sprite.GetNormalizedUV().maxs.y), Vector2(sprite.GetNormalizedUV().maxs.x, sprite.GetNormalizedUV().mins.y), Rgba::WHITE);
	theRenderer->SetShader(theRenderer->CreateOrGetShader("default"));

	theRenderer = nullptr;
}

void Agent::UpdateSpriteRenderDirection()
{
	//calculate the largest dot between facing or turned away
	float dotValue = 0;
	float northDot = DotProduct(Vector2(MAP_NORTH), Vector2(m_forward));
	float southDot = DotProduct(Vector2(MAP_SOUTH), Vector2(m_forward));
	float eastDot = DotProduct(Vector2(MAP_EAST), Vector2(m_forward));
	float westDot = DotProduct(Vector2(MAP_WEST), Vector2(m_forward));

	IntVector2 direction;

	//set the direction we are most aligned with between north and south
	float highestLongitudeDot = 0;
	if (northDot > southDot)
	{
		direction.y = 1;
		highestLongitudeDot = northDot;
	}
	else
	{
		direction.y = -1;
		highestLongitudeDot = southDot;
	}
	
	//set the direction we are most aligned with between east and west
	float highestLatitudeDot = 0;
	if (eastDot > westDot)
	{
		direction.x = 1;
		highestLatitudeDot = eastDot;
	}
	else
	{
		direction.x = -1;
		highestLatitudeDot = westDot;
	}

	//compare north and south dot to the highest of the east and west dot. Whichever is higher is the direction we are most facing.
	if (highestLongitudeDot > highestLatitudeDot)
		direction.x = 0;
	else
		direction.y = 0;

	//set the final direction.
	m_spriteDirection = direction;
}
