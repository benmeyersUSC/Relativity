//
// Created by Ben Meyers on 2/11/26.
//
#pragma once
#include "Actor.h"
   
class Paddle : public Actor
{
public:
	static constexpr float PADDLE_HEIGHT = 10.0f;
	static constexpr float PADDLE_WIDTH = 10.0f;
	static constexpr float PADDLE_ACCEL = 10.0f;

protected:

	Paddle(unsigned frameLife);
	friend class Game;
};
   