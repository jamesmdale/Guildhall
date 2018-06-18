#pragma once
#include "Engine\Core\Widget.hpp"
#include "Engine\Camera\Camera.hpp"
#include "Engine\Math\AABB2.hpp"

class Board : public Widget
{
public:
	Board();
	Board(const std::string& name) : Widget(name)
	{
		//board creation
	}

	virtual ~Board() override;
	virtual void Initialize() override;

	void CreateBoardMeshesForRenderable(Renderable2D* renderable);
	void CreateBoardTextMeshesForRenderable(Renderable2D* renderable);
	void CreateBoardTexturedMeshesForRenderable(Renderable2D* renderable);
	
	void UpdateRenderables();

public:
	
	// enemy board quads =========================================================================================
	AABB2 m_enemyHandQuad;
	AABB2 m_enemyBattlfieldQuad;
	AABB2 m_enemyHeroPortraitQuad;
	AABB2 m_enemyHeroAbilityQuad;
	AABB2 m_enemyHeroWeaponQuad;
	AABB2 m_enemyManaQuad;
	AABB2 m_enemyDeckQuad;

	// player board quads =========================================================================================
	AABB2 m_playerHandQuad;
	AABB2 m_playerBattlfieldQuad;
	AABB2 m_playerHeroPortraitQuad;
	AABB2 m_playerHeroAbilityQuad;
	AABB2 m_playerHeroWeaponQuad;
	AABB2 m_playerManaQuad;
	AABB2 m_playerDeckQuad;

	// shared board quads =========================================================================================
	AABB2 m_endTurnQuad;
	
};


