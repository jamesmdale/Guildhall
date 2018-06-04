#include "Game\Actor.hpp"
#include "Game\IsoSpriteAnimSetDefinition.hpp"
#include "Engine\Renderer\Renderer.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Game\GameCommon.hpp"

Actor::Actor(IntVector2 startingMapPosition, ActorFaction startingFaction, ActorDefinition* definition, Map* mapReference, IntVector2 facingDirection)
{
	m_positionXYZ.x = (float)startingMapPosition.x;
	m_positionXYZ.z = (float)startingMapPosition.y;

	m_positionXYZ.y = mapReference->GetHeightForPositionXZ(startingMapPosition) - 1.f;

	m_faction = startingFaction;
	m_definition = definition;

	std::map<std::string, IsoSpriteAnimSetDefinition*>::iterator spriteDefIterator = IsoSpriteAnimSetDefinition::s_isoSpriteAnimSetDefinitions.find(m_definition->m_animSetId);
	if (spriteDefIterator != IsoSpriteAnimSetDefinition::s_isoSpriteAnimSetDefinitions.end())
	{
		m_animationSet = new IsoSpriteAnimSet(spriteDefIterator->second);
	}		

	m_health = m_definition->m_maxHealth;
	m_currentMap = mapReference;

	m_currentForwardInMapDirection = facingDirection;
}

void Actor::Update(float deltaSeconds)
{
	IntVector2 actorCoord = GetPositionInXZ();
	int height = m_currentMap->GetHeightForPositionXZ(actorCoord) - 1.f;
	Vector3 actorCenterPosition = Vector3(m_positionXYZ.x, m_positionXYZ.y + 0.5f, m_positionXYZ.z);

	m_transform.SetPosition(actorCenterPosition);
	m_transform.SetScale(Vector3(1.f, 1.5f, 1.f));
	m_transform.SetRotation(Vector3(0.f, 90.f - g_theGame->m_gameCamera->m_rotation, 0.f));

	//sets sprite direction from camera
	SetSpriteRenderDirection();	

	m_animationSet->Update(deltaSeconds);
}

void Actor::Render()
{
	TODO("Add a more sophisticated way to visualize factions");
	Rgba actorColor = Rgba::RED;			
	if(m_faction == BLUE_ACTOR_FACTION)
	{
		actorColor = Rgba::BLUE;
	}

	Renderer* theRenderer = Renderer::GetInstance();	

	IntVector2 isoSpriteScale = m_animationSet->GetCurrentIsoSprite()->GetCurrentSpriteScaleByDirection(m_spriteDirection);
	Sprite sprite = *m_animationSet->GetCurrentSprite(m_spriteDirection);
	//Sprite sprite = *m_animationSet->GetCurrentSprite(m_currentFacingDirection);

	Texture* texture = sprite.GetSpriteTexture();
	Vector2 spritePivot = sprite.m_definition->m_pivot;

	m_transform.m_scale.x *= isoSpriteScale.x;
	m_transform.m_scale.y *= isoSpriteScale.y;

	IntVector2 spriteDimensions = sprite.GetDimensions();

	//mtransform.SetRotation(Vector3(g_theGame->m_gameCamera->m_azimuth, g_theGame->m_gameCamera->m_rotation, 0.f));

	AABB2 bounds;

	bounds.mins.x = 0.f - (spritePivot.x);
	bounds.maxs.x = bounds.mins.x + 1.f;
	bounds.mins.y = 0.f - (spritePivot.y);
	bounds.maxs.y = bounds.mins.y + 1.f;
	
	Vector3 directionIdentifier = m_transform.m_position + Vector3((float)m_currentForwardInMapDirection.x * .5f, 1.f, (float)m_currentForwardInMapDirection.y * .5f);
	//test the flipping of the sprite
	//theRenderer->DrawOrientedTexturedAABB(m_transform.GetTransformMatrix(), bounds, texture, Rgba::WHITE,);	
	theRenderer->UseShaderProgram(theRenderer->CreateOrGetShaderProgram("Data/Shaders/discardAlpha"));
	theRenderer->DrawOrientedTexturedAABB(m_transform.GetTransformMatrix(), bounds, texture, Rgba::WHITE, sprite.GetNormalizedUV());		

	theRenderer->EnableDepth(LESS, true);

	theRenderer->SetTexture(*theRenderer->m_defaultTexture);
	theRenderer->UseShaderProgram(theRenderer->CreateOrGetShaderProgram("default"));

	theRenderer->DrawCube(directionIdentifier, Vector3(.25f, .25f, .25f), Rgba::PURPLE);

	//theRenderer->DrawOrientedTexturedAABB(m_transform.GetTransformMatrix(), bounds, texture, Rgba::WHITE, sprite.GetNormalizedUV());		

	theRenderer = nullptr;
}

void Actor::SetSpriteRenderDirection()
{
	OrbitCamera* gameCamera = g_theGame->m_gameCamera;
	Vector2 cameraForwardDirection = Vector2(gameCamera->m_cameraMatrix.GetKBasis().x, gameCamera->m_cameraMatrix.GetKBasis().z);
	//cameraForwardDirection = Vector2(GetMagnitude(cameraForwardDirection.x), GetMagnitude(cameraForwardDirection.y));

	//get actors forward to compare with camera's forward
	IntVector2 actorForward = m_currentForwardInMapDirection;

	IntVector2* actorLeft = new IntVector2();
	IntVector2* actorRight = new IntVector2();
	IntVector2* actorBack = new IntVector2();

	GetDirectionsFromActorForward(actorForward, *actorLeft, *actorRight, *actorBack);

	//calculate the largest dot between facing or turned away
	float dotValue = 0;
	float dot = DotProduct(cameraForwardDirection, Vector2(actorForward));
	dotValue = dot;
	IntVector2 largestDotBackAndForward = actorForward;

	dot = DotProduct(cameraForwardDirection, Vector2(*actorBack));
	if(dot > dotValue)
	{
		dotValue = dot;
		largestDotBackAndForward = *actorBack;
	}

	//calculate the largest dot between left or right
	dotValue = 0;
	dot = DotProduct(cameraForwardDirection, Vector2(*actorRight));
	dotValue = dot;
	IntVector2 largestDotLeftAndRight = *actorRight;

	dot = DotProduct(cameraForwardDirection, Vector2(*actorLeft));
	if(dot > dotValue)
	{
		dotValue = dot;
		largestDotLeftAndRight = *actorLeft;
	}

	//set the sprite direction for the isosprite to draw the correct sprite in animation
	if(largestDotBackAndForward == actorForward)
	{
		m_spriteDirection.y = 1;
	}
	else
	{
		m_spriteDirection.y = -1;
	}

	if(largestDotLeftAndRight == *actorRight)
	{
		m_spriteDirection.x = -1;
	}
	else
	{
		m_spriteDirection.x = 1;
	}
}


void Actor::GetDirectionsFromActorForward(IntVector2 actorForward, IntVector2& actorLeftOut, IntVector2& actorRightOut, IntVector2& actorBackOut)
{
	if(actorForward == MAP_NORTH)
	{
		actorLeftOut = MAP_WEST;
		actorRightOut = MAP_EAST;
		actorBackOut = MAP_SOUTH;
	}

	if(actorForward == MAP_EAST)
	{
		actorLeftOut = MAP_NORTH;
		actorRightOut = MAP_SOUTH;
		actorBackOut = MAP_WEST;
	}

	if(actorForward == MAP_WEST)
	{
		actorLeftOut = MAP_SOUTH;
		actorRightOut = MAP_NORTH;
		actorBackOut = MAP_EAST;
	}

	if(actorForward == MAP_SOUTH)
	{
		actorLeftOut = MAP_EAST;
		actorRightOut = MAP_WEST;
		actorBackOut = MAP_NORTH;
	}
}

float Actor::GetCombatBlockChance(Actor* attackingActor)
{
	IntVector2 attackingActorFacingDirection = attackingActor->m_currentForwardInMapDirection;
	
	IntVector2* actorLeft = new IntVector2();
	IntVector2* actorRight = new IntVector2();
	IntVector2* actorBack = new IntVector2();

	GetDirectionsFromActorForward(m_currentForwardInMapDirection, *actorLeft, *actorRight, *actorBack);
	
	float blockChance = 0.0f;

	float dotValue = 0.0f;
	float dot = DotProduct(Vector2(attackingActorFacingDirection), Vector2(m_currentForwardInMapDirection));
	dotValue = dot;
	blockChance = g_blockChanceBack;

	dot = DotProduct(Vector2(attackingActorFacingDirection), Vector2(*actorBack));
	if(dot > dotValue)
	{
		dotValue = dot;
		blockChance = g_blockChanceFront;
	}

	dot = DotProduct(Vector2(attackingActorFacingDirection), Vector2(*actorLeft));
	if(dot > dotValue)
	{
		dotValue = dot;
		blockChance = g_blockChanceSide;
	}

	dot = DotProduct(Vector2(attackingActorFacingDirection), Vector2(*actorRight));
	if(dot > dotValue)
	{
		dotValue = dot;
		blockChance = g_blockChanceSide;
	}

	return blockChance + m_blockChance;
}

float Actor::GetCombatCritChance(Actor* targetingActor)
{
	IntVector2 facingDirection = targetingActor->m_currentForwardInMapDirection;

	IntVector2* actorLeft = new IntVector2();
	IntVector2* actorRight = new IntVector2();
	IntVector2* actorBack = new IntVector2();

	GetDirectionsFromActorForward(m_currentForwardInMapDirection, *actorLeft, *actorRight, *actorBack);

	float critChance = 0.0f;

	float dotValue = 0.0f;
	float dot = DotProduct(Vector2(facingDirection), Vector2(m_currentForwardInMapDirection));
	dotValue = dot;
	critChance = g_critChanceBack;

	dot = DotProduct(Vector2(facingDirection), Vector2(*actorBack));
	if(dot > dotValue)
	{
		dotValue = dot;
		critChance = g_critChanceFront;
	}

	dot = DotProduct(Vector2(facingDirection), Vector2(*actorLeft));
	if(dot > dotValue)
	{
		dotValue = dot;
		critChance = g_critChanceSide;
	}

	dot = DotProduct(Vector2(facingDirection), Vector2(*actorRight));
	if(dot > dotValue)
	{
		dotValue = dot;
		critChance = g_critChanceSide;
	}

	return critChance;
}

void Actor::TurnTowardTargetLocation(IntVector2 position)
{
	Vector2 displacementToTarget =  Vector2(position.x, position.y) - Vector2(m_positionXYZ.x, m_positionXYZ.z)  ;
	IntVector2* actorLeft = new IntVector2();
	IntVector2* actorRight = new IntVector2();
	IntVector2* actorBack = new IntVector2();

	GetDirectionsFromActorForward(m_currentForwardInMapDirection, *actorLeft, *actorRight, *actorBack);

	float dotValue = 0.0f;
	float dot = DotProduct(Vector2(displacementToTarget), Vector2(m_currentForwardInMapDirection));
	dotValue = dot;

	dot = DotProduct(Vector2(displacementToTarget), Vector2(*actorBack));
	if(dot > dotValue)
	{
		dotValue = dot;
		m_currentForwardInMapDirection = *actorBack;
	}

	dot = DotProduct(Vector2(displacementToTarget), Vector2(*actorLeft));
	if(dot > dotValue)
	{
		dotValue = dot;
		m_currentForwardInMapDirection = *actorLeft;
	}

	dot = DotProduct(Vector2(displacementToTarget), Vector2(*actorRight));
	if(dot > dotValue)
	{
		dotValue = dot;
		m_currentForwardInMapDirection = *actorRight;
	}

	return;
}

IntVector2 Actor::GetPositionInXZ()
{
	return IntVector2(RoundToNearestInt(m_positionXYZ.x), RoundToNearestInt(m_positionXYZ.z));
}

void Actor::SetPositionXZ(IntVector2 positionXZ)
{
	m_positionXYZ.x = (float)positionXZ.x;
	m_positionXYZ.z = (float)positionXZ.y;

	return;
}
