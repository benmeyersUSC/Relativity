//
// Created by Ben Meyers on 2/11/26.
//
#pragma once
#include "Actor.h"
class SpacetimeComponent;
class DrawComponent;

class Player : public Actor
{
	static constexpr float BRAKE_FACTOR = 0.9f;
	short mVeloSign = 0.0f;
	SpacetimeComponent* mSpacetime;
	DrawComponent* mDraw;
	void FixPosition();
public:
	static constexpr float PLAYER_HEIGHT = 54.0f;
	static constexpr float PLAYER_WIDTH = 54.0f;
	static constexpr float PLAYER_ACCEL = 1000.0f;

	void HandleRender() override;
	void HandleInput(const bool keys[], SDL_MouseButtonFlags mouseButtons,
							 const Vector2& posMouse) override;
	void HandleUpdate(float deltaTime) override;
	SpacetimeComponent* GetSpacetime(){return mSpacetime;}
protected:
	explicit Player();
	friend class Game;
};
   