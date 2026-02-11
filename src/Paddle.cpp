//
// Created by Ben Meyers on 2/11/26.
//
#include "Paddle.h"
#include "Game.h"

Paddle::Paddle( unsigned frameLife) : Actor(frameLife) {
    GetTransform().SetSize(Vector2(PADDLE_WIDTH, PADDLE_HEIGHT)); // not quite bottom
}
