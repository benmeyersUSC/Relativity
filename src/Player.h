//
// Created by Ben Meyers on 2/11/26.
//
#pragma once
#include "Actor.h"
class SpacetimeComponent;
class DrawComponent;

class Player : public Actor
{
	static constexpr float BRAKE_FACTOR = 0.945f;
	int mVeloSign = 0;
	SpacetimeComponent* mSpacetime;
	DrawComponent* mDraw;
	void FixPosition();
public:
	static constexpr float PLAYER_HEIGHT = 54.0f;
	static constexpr float PLAYER_WIDTH = 54.0f;
	static constexpr float PLAYER_ACCEL = 810.0f;

	void HandleRender() override;
	void HandleInput(const bool keys[], SDL_MouseButtonFlags mouseButtons,
							 const Vector2& posMouse) override;
	void HandleUpdate(float deltaTime) override;
	[[nodiscard]] SpacetimeComponent* GetSpacetime() const {return mSpacetime;}
protected:
	explicit Player();
	friend class Game;
};
   