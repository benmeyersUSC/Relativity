//
// Created by Ben Meyers on 2/11/26.
//
#pragma once
#include "Actor.h"
   
class Paddle : public Actor
{
	short mVeloSign = 0.0f;
public:
	static constexpr float PADDLE_HEIGHT = 10.0f;
	static constexpr float PADDLE_WIDTH = 10.0f;
	static constexpr float PADDLE_ACCEL = 10.0f;

	void HandleInput(const bool *keyboardInput) override;
	void HandleUpdate(float deltaTime) override;
protected:
	explicit Paddle(unsigned frameLife);
	friend class Game;
};
   