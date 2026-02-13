//
// Created by Ben Meyers on 2/11/26.
//
#pragma once
#include "Actor.h"
   
class Paddle : public Actor
{
	static constexpr float BRAKE_FACTOR = 0.9f;
	short mVeloSign = 0.0f;
public:
	static constexpr float PADDLE_HEIGHT = 54.0f;
	static constexpr float PADDLE_WIDTH = 54.0f;
	static constexpr float PADDLE_ACCEL = 2000.0f;

	void HandleInput(const bool keys[], SDL_MouseButtonFlags mouseButtons,
							 const Vector2& posMouse) override;
	void HandleUpdate(float deltaTime) override;
protected:
	explicit Paddle(unsigned frameLife);
	friend class Game;
};
   