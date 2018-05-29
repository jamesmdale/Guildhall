#include "Game.hpp"
#include <stdlib.h>
#include "Asteroid.hpp"
#include "Bullet.hpp"
#include "Engine\Math\MathUtils.hpp"
#include "Ship.hpp"

Game* g_Game = nullptr;
int numSpawnedAstroids = 0;
int waveLevel = 0;

Game::Game(){

	player = SpawnShip();
	
	for(int i = 0; i < g_maxAsteroidCount; i++)
	{
		asteroids[i] = nullptr;
	}
	for(int i = 0; i < g_maxBulletCount; i++)
	{
		bullets[i] = nullptr;
	}

	for(int i = 0; i < g_initialAsteroidCount; i++)  //spawn initial asteroids
	{
			asteroids[i] = SpawnRandomLargeAsteroid();
			asteroids[i]->Render();
	}
}

Game::~Game()
{

}

void Game::Initialize(){
	
}

void Game::Update(float timeDelta){		

	if(numSpawnedAstroids == 0)
	{  
 	 	waveLevel++;

		int numAsteroidsToSpawn = g_initialAsteroidCount + waveLevel;
		for(int i = 0; i < numAsteroidsToSpawn; i++)
		{
			asteroids[i] = SpawnRandomLargeAsteroid();
		}
		
	}

	float modifiedtimeDelta = UpdateFlagsFromInput(timeDelta); //edit ship values and flip flags based on inputs. Update time delta

	//done processiong new keys now update asteroids
	for(int i=0; i < g_maxAsteroidCount; i++)
	{
		if(asteroids[i] != nullptr)
		{			
			asteroids[i]->Update(modifiedtimeDelta);
			if(asteroids[i]->m_isDead) //if asteriod was updated to being dead last frame, we need to remove the asteroid
			{
				RemoveAsteroid(i);
			}
		}		
	}

	for(int i=0; i < g_maxBulletCount; i++)
	{
		if(bullets[i] != nullptr)
		{	
			
			bullets[i]->Update(modifiedtimeDelta);
			
						
			if(bullets[i]->m_age > g_maxBulletAge || bullets[i]->m_isDead) //if the bullet is too old or was updated to being dead last frame, we need to remove the bullet
			{
				RemoveBullet(i);				
			}
		}		
	}	

	if(player->m_isDead) //if the player was updated last frame to be dead, we need to reset the ship
	{
		ResetShip();
	}
	else
	{
		player->Update(modifiedtimeDelta);
	}	

	for(int i = 0; i < g_maxAsteroidCount; i++)
	{
		bool hasCollided = false;
		if(asteroids[i] != nullptr && !asteroids[i]->m_isExploding) //if the asteroid is exploding don't do this calculation.
		{	
			for(int j = 0; j < g_maxBulletCount; j++)
			{
				if(bullets[j] != nullptr && !bullets[j]->m_isExploding)
				{
					hasCollided = DoDiscsOverlap(asteroids[i]->m_innerCollider, bullets[j]->m_innerCollider); 
					if(hasCollided)
					{
						if(asteroids[i]->m_cosmeticRadius == g_largeAsteroidSize)
						{
							SpawnSmallerAsteroids(*asteroids[i], g_mediumAsteroidSize);
						}
						if(asteroids[i]->m_cosmeticRadius == g_mediumAsteroidSize)
						{
							SpawnSmallerAsteroids(*asteroids[i], g_smallAsteroidSize);
						}
						asteroids[i]->m_isExploding = true;
						asteroids[i]->m_explodeTime = asteroids[i]->m_age;
						bullets[j]->m_isExploding = true;
						bullets[j]->m_explodeTime = bullets[j]->m_age;
						break;
					}
				}
			}
			if(hasCollided)
			{
				hasCollided = false;
			}
			if(!player->m_isDead && !player->m_isExploding)
			{
				hasCollided = DoDiscsOverlap(asteroids[i]->m_innerCollider, player->m_innerCollider);
				if(hasCollided)
				{
					asteroids[i]->m_isExploding = true;	
					asteroids[i]->m_explodeTime = asteroids[i]->m_age;
					player->m_isExploding = true;
					player->m_explodeTime = player->m_age;					
				}
			}
		}
	}	
}

void Game::Render(){

	for(int i=0; i < g_maxAsteroidCount; i++)
	{
		if(asteroids[i] != nullptr)
		{
			asteroids[i]->Render();
		}		
	}

	for(int i=0; i < g_maxBulletCount; i++)
	{
		if(bullets[i] != nullptr)
		{
			bullets[i]->Render();
		}		
	}
	player->Render();	
}

void Game::RemoveAsteroid(int asteroidIndex)
{
	if(asteroidIndex < 0)
	{
		for(int i = 0; i < g_maxAsteroidCount; i++)
		{
			if(asteroids[i] != nullptr)
			{
				delete asteroids[i];
				asteroids[i] = nullptr;
				break;
			}			
		}
	}
	else
	{
		delete asteroids[asteroidIndex];
		asteroids[asteroidIndex] = nullptr;
	}

	numSpawnedAstroids--;
}

void Game::RemoveBullet(int bulletIndex)
{	
	delete bullets[bulletIndex];
	bullets[bulletIndex] = nullptr;	
}

void Game::AddNewAsteroid()
{
	for(int i = 0; i < g_maxAsteroidCount; i++)
	{
		if(asteroids[i] == nullptr)
		{
			asteroids[i] = SpawnRandomLargeAsteroid();
			numSpawnedAstroids++;
			break;
		}
	}
}

void Game::AddNewBullet()
{
	if(!player->m_isDead)
	{	
		for(int i = 0; i < g_maxBulletCount; i++)
		{
			if(bullets[i] == nullptr)
			{
				if(!player->m_isDead)
				bullets[i] = SpawnBullet();
				break;
			}
		}
	}
}

void Game:: SpawnSmallerAsteroids(const Asteroid& asteroidToCopy, float spawnAsteroidSize)
{
	Asteroid* spawnAsteroid1 = new Asteroid(asteroidToCopy);
	Asteroid* spawnAsteroid2 = new Asteroid(asteroidToCopy);
	float asteroidDegrees = asteroidToCopy.m_movementVector.GetOrientationDegrees();

	//spawn asteroids outside of each other's outter collider
	spawnAsteroid1->m_centerX += spawnAsteroidSize;
	spawnAsteroid1->m_centerY += spawnAsteroidSize;
	spawnAsteroid2->m_centerX -= spawnAsteroidSize;
	spawnAsteroid2->m_centerY -= spawnAsteroidSize;

	//shoot the asteroids off in direction relatively close to the previous direction of the copied asteroid. Make it smaller
	spawnAsteroid1->m_movementVector.x = cosf(asteroidDegrees + 10);
	spawnAsteroid1->m_movementVector.y = sinf(asteroidDegrees + 10);
	spawnAsteroid1->m_cosmeticRadius = spawnAsteroidSize;

	//shoot the asteroids off in direction relatively close to the previous direction of the copied asteroid. Make it smaller
	spawnAsteroid2->m_movementVector.x = cosf(asteroidDegrees - 10);
	spawnAsteroid2->m_movementVector.y = sinf(asteroidDegrees - 10);
	spawnAsteroid2->m_cosmeticRadius = spawnAsteroidSize;

	float degrees = 360.f/g_MaxVerticesPerShape;

	for(int i = 0; i < g_MaxVerticesPerShape; i++)
	{
		float radians = ConvertDegreesToRadians((float)i * degrees);
		float randomizedRadius = GenerateRandomFloatInRangeLessThanOne(0.8f);
		float startX = CalculateXPoint(0, randomizedRadius, radians);
		float startY = CalculateYPoint(0, randomizedRadius, radians);

		spawnAsteroid1->m_shapeVertices[i] = Vector2(startX, startY);
		spawnAsteroid2->m_shapeVertices[i] = Vector2(startX, startY);
	}

	spawnAsteroid1->m_age = 0.f;
	spawnAsteroid1->m_explodeTime = 0.f;
	spawnAsteroid1->m_isDead = false;
	spawnAsteroid1->m_isExploding = false;
	spawnAsteroid1->CloneVerticesToExplodeVertices();

	spawnAsteroid2->m_age = 0.f;
	spawnAsteroid1->m_explodeTime = 0.f;
	spawnAsteroid2->m_isDead = false;
	spawnAsteroid2->m_isExploding = false;
	spawnAsteroid2->CloneVerticesToExplodeVertices();

	spawnAsteroid1->m_outerCollider = Disc2(0.f, 0.f, 1.f * spawnAsteroid1->m_outerCollider.radius);
	spawnAsteroid1->m_innerCollider = Disc2(0.f, 0.f, 0.8f * spawnAsteroid1->m_innerCollider.radius);
	spawnAsteroid2->m_outerCollider = Disc2(0.f, 0.f, 1.f * spawnAsteroid1->m_outerCollider.radius); 
	spawnAsteroid2->m_innerCollider = Disc2(0.f, 0.f, 0.8f * spawnAsteroid1->m_innerCollider.radius);

	for(int i = 0; i < g_maxAsteroidCount; i++)
	{
		if(g_Game->asteroids[i] == nullptr)
		{
			g_Game->asteroids[i] = spawnAsteroid1;
			break;
		}
	}
	for(int i = 0; i < g_maxAsteroidCount; i++)
	{
		if(g_Game->asteroids[i] == nullptr)
		{
			g_Game->asteroids[i] = spawnAsteroid2;
			break;
		}
	}

	numSpawnedAstroids += 2;
}

Asteroid* Game::SpawnRandomLargeAsteroid()
{	
	Asteroid* randomAsteroid = new Asteroid();	
	randomAsteroid->m_Type = 'a';	
	
	randomAsteroid->m_cosmeticRadius = g_largeAsteroidSize;	

	randomAsteroid->m_rotationDegrees = GetRandomFloatInRange(5, 40);
	randomAsteroid->m_rotationSpeed = GetRandomFloatInRange(10, 40);
	randomAsteroid->m_movementSpeed = g_movementSpeed;
	float movementDegree = GetRandomFloatInRange(10, 350);
	if(movementDegree == 90 || movementDegree == 180)
	{
		movementDegree += 10; //avoid them being off screen
	}	
	randomAsteroid->m_movementVector.x = cosf(movementDegree);
	randomAsteroid->m_movementVector.y = sinf(movementDegree);		
	randomAsteroid->m_centerX = static_cast<float>(GetRandomFloatInRange(0.f, 1000.0f + randomAsteroid->m_cosmeticRadius)); //sets an asteroid that spawns off screen
	randomAsteroid->m_centerY = static_cast<float>(GetRandomFloatInRange(0.f, 1000.0f + randomAsteroid->m_cosmeticRadius));

	if(randomAsteroid->m_centerX < 1000.0f + randomAsteroid->m_cosmeticRadius || randomAsteroid->m_centerY < 1000.0f + randomAsteroid->m_cosmeticRadius)
	{
		float randomXorY = GetRandomFloatZeroToOne();

		if(randomXorY > 0.5f) //make sure that the asteroid is off screen
		{
			randomAsteroid->m_centerX = 1000.0f + randomAsteroid->m_cosmeticRadius;
		}
		else
		{
			randomAsteroid->m_centerY = 1000.0f + randomAsteroid->m_cosmeticRadius;
		}
	}

	float degrees = 360.f/g_MaxVerticesPerShape;

	for(int i = 0; i < g_MaxVerticesPerShape; i++)
	{
		float radians = ConvertDegreesToRadians((float)i * degrees);
		float randomizedRadius = GenerateRandomFloatInRangeLessThanOne(0.8f);
		float startX = CalculateXPoint(0, randomizedRadius, radians);
		float startY = CalculateYPoint(0, randomizedRadius, radians);

		randomAsteroid->m_shapeVertices[i] = Vector2(startX, startY);
	}

	randomAsteroid->m_age = 0.f;
	randomAsteroid->m_explodeTime = 0.f;
	randomAsteroid->m_isDead = false;
	randomAsteroid->m_isExploding = false;
	randomAsteroid->CloneVerticesToExplodeVertices();
	randomAsteroid->m_outerCollider = Disc2(0.f, 0.f, 1.f * randomAsteroid->m_outerCollider.radius); //does this use the local coordinates or not?
	randomAsteroid->m_innerCollider = Disc2(0.f, 0.f, 0.8f * randomAsteroid->m_innerCollider.radius);

	numSpawnedAstroids++;

	return randomAsteroid;
}

Ship* Game::SpawnShip()
{
	Ship* newPlayer = new Ship();
	newPlayer->m_isAccelerating = false;
	
	newPlayer->m_Type = 's';
	newPlayer->m_radius = 1.f;
	newPlayer->m_centerX = 500.f;
	newPlayer->m_centerY = 500.f;
	newPlayer->m_cosmeticRadius = 25.f;
	newPlayer->m_rotationDegrees = 90.f; //start ship facing up
	newPlayer->m_rotationSpeed = 0.0f; //set to 0. this will change when the player turns the ship
	newPlayer->m_movementSpeed = 0.0f;
	newPlayer->m_movementVector = Vector2(0,1);
	newPlayer->m_color = g_shipColor;
	newPlayer->m_orientationVector = Vector2(CosDegrees(newPlayer->m_rotationDegrees), SinDegrees(newPlayer->m_rotationDegrees)).GetNormalized();

	newPlayer->m_shapeVertices[0] = Vector2(1.f, 0.f); //this point is always the nose
	newPlayer->m_shapeVertices[1] = Vector2(-0.6f, 0.8f);
	newPlayer->m_shapeVertices[2] = Vector2(-0.2f, 0.2f);
	newPlayer->m_shapeVertices[3] = Vector2(-0.2f, 0.f);
	newPlayer->m_shapeVertices[4] = Vector2(-0.2f, -0.2f);
	newPlayer->m_shapeVertices[5] = Vector2(-0.6f, -0.8f);	

	newPlayer->m_age = 0.f;
	newPlayer->m_explodeTime = 0.f;
	newPlayer->m_isDead = false;
	newPlayer->m_isExploding = false;
	newPlayer->CloneVerticesToExplodeVertices();

	newPlayer->m_outerCollider = Disc2(0.f, 0.f, 1.f); //does this use the local coordinates or not?
	newPlayer->m_innerCollider = Disc2(0.f, 0.f, 0.8f);

	return newPlayer;
}

Bullet* Game::SpawnBullet()
{	
	Bullet* newBullet = new Bullet();	
	newBullet->m_Type = 'b';
	newBullet->m_radius = 1.f;
	Vector2 nose = player->GetShipNose();

	newBullet->m_centerX = nose.x;
	newBullet->m_centerY = nose.y;
	newBullet->m_cosmeticRadius = 5.f;
	newBullet->m_movementSpeed = 400.0f;
	newBullet->m_movementVector = (nose - Vector2(player->m_centerX, player->m_centerY)).GetNormalized();
	newBullet->m_rotationDegrees = 0.0f;
	newBullet->m_rotationSpeed = 0.0f;

	newBullet->m_shapeVertices[0] = Vector2(1.f, 0.f); //this point is always the nose
	newBullet->m_shapeVertices[1] = Vector2(0.f, 1.f);
	newBullet->m_shapeVertices[2] = Vector2(-1.f, 0.f);
	newBullet->m_shapeVertices[3] = Vector2(0.f, -1.f);	

	newBullet->m_age = 0.f;
	newBullet->m_explodeTime = 0.f;
	newBullet->m_isDead = false;
	newBullet->m_isExploding = false;
	newBullet->CloneVerticesToExplodeVertices();

	newBullet->m_outerCollider = Disc2(0.f, 0.f, 1.f * newBullet->m_innerCollider.radius); 
	newBullet->m_innerCollider = Disc2(0.f, 0.f, 0.8f * newBullet->m_innerCollider.radius);

	return newBullet;
}

void Game::ResetShip()
{
	player->ResetShipToOrigin();
}


float Game::UpdateFlagsFromInput(float timeDelta)
{
	if(player->m_isDead || player->m_isExploding)
	{
		player->m_rotationSpeed = 0;
	}
	//if they press 'd' key or right arrow AND the 'a' key or the left arrow, don't move
	else if( (g_theInput->IsKeyPressed(g_theInput->KEYBOARD_D) || g_theInput->IsKeyPressed(g_theInput->KEYBOARD_RIGHT_ARROW)) && (g_theInput->IsKeyPressed(g_theInput->KEYBOARD_A) || g_theInput->IsKeyPressed(g_theInput->KEYBOARD_LEFT_ARROW)) )
	{
		player->m_rotationSpeed = 0;
	}
	//if they press 'd' key or right arrow, rotate right
	else if(g_theInput->IsKeyPressed(g_theInput->KEYBOARD_D) || g_theInput->IsKeyPressed(g_theInput->KEYBOARD_RIGHT_ARROW))
	{
		player->m_rotationSpeed = -250.f;
	}

	//if they press 'a' key or left arrow, rotate left
	else if(g_theInput->IsKeyPressed(g_theInput->KEYBOARD_A) || g_theInput->IsKeyPressed(g_theInput->KEYBOARD_LEFT_ARROW))
	{
		player->m_rotationSpeed = 250.f;		
	}
	else
	{
		player->m_rotationSpeed = 0;
	}


	if(player->m_isDead || player->m_isExploding)
	{
		player->m_isAccelerating = false;
	}
	//if they press 'w' key or up arrow, accelerate
	else if(g_theInput->IsKeyPressed(g_theInput->KEYBOARD_W) || g_theInput->IsKeyPressed(g_theInput->KEYBOARD_UP_ARROW))
	{
		player->m_isAccelerating = true;
	}
	else
	{
		player->m_isAccelerating = false;
	}

	//if they press 'n' respawn the ship
	if(g_theInput->WasKeyJustPressed(g_theInput->KEYBOARD_N))
	{
		g_Game->isResetShipKeyPressed = true;
		player->m_isDead = false;	
		g_Game->ResetShip();
	}

	//if they press the space key, "shooooooooooootttt ttheeeeemmmmm, mmmmorrrrrtttyyyy"
	if(g_theInput->WasKeyJustPressed(g_theInput->KEYBOARD_SPACE) && (!player->m_isDead && !player->m_isExploding))
	{		
		g_Game->isShootKeyPressed = true;		
		g_Game->AddNewBullet();				
	}

	//if they press 'i' add a polygon
	if(g_theInput->WasKeyJustPressed(g_theInput->KEYBOARD_I)) //i
	{
		g_Game->isCreateAsteroidPressed = true;	
		g_Game->AddNewAsteroid();
	}

	//if they press '0' delete a polygon
	if(g_theInput->WasKeyJustPressed(g_theInput->KEYBOARD_O))
	{
		g_Game->isRemoveAsteroidPressed = true;	
		g_Game->RemoveAsteroid(-1); //remove a random asteroid
	}

	if(g_theInput->IsKeyPressed(g_theInput->KEYBOARD_T))
	{
		g_Game->isSlowed = true;

		if(isSlowed)
		{
			timeDelta *= 0.1f;		
		}
	}

	if(g_theInput->WasKeyJustPressed(g_theInput->KEYBOARD_P))
	{
		if(g_Game->isPaused == true)
		{
			g_Game->isPaused = false;	
		}
		else
		{
			g_Game->isPaused = true;	
		}		
	}
	if(isPaused)
	{
		timeDelta = 0.0f;
	}

	if(g_theInput->WasKeyJustPressed(g_theInput->KEYBOARD_F1))
	{
		if(g_isDebug == true)
		{
			g_isDebug = false;	
		}
		else
		{
			g_isDebug = true;	
		}
	}

	//now do controller
	XboxController inputController = g_theInput->GetController(0);

	if(inputController.WasKeyJustPressed(XBOX_A))
	{
		g_Game->isShootKeyPressed = true;		
		g_Game->AddNewBullet();
	}
	if(inputController.WasKeyJustPressed(XBOX_START))
	{
		g_Game->isResetShipKeyPressed = true;
		player->m_isDead = false;	
		g_Game->ResetShip();
	}
	if(inputController.m_joyStickLeft.m_correctedPositionX  != 0 || inputController.m_joyStickLeft.m_correctedPositionY != 0)
	{
		player->m_isAccelerating = true;
		player->m_movementVector += Vector2(inputController.m_joyStickLeft.m_correctedPositionX, inputController.m_joyStickLeft.m_correctedPositionY) * timeDelta *  inputController.m_joyStickLeft.m_magnitude * 100;
		player->m_orientationVector = Vector2(CosDegrees(inputController.m_joyStickLeft.m_angleDegrees), SinDegrees(inputController.m_joyStickLeft.m_angleDegrees)).GetNormalized();
		player->m_rotationDegrees = inputController.m_joyStickLeft.m_angleDegrees;
	}

	return timeDelta;
}

