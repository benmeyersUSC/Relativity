//
// Created by Ben Meyers on 2/11/26.
//

#include "Actor.h"

Actor::Actor(unsigned frameLife) {
    mVelos.reserve(frameLife);
}

void Actor::HandleUpdate(float deltaTime) {
}

void Actor::HandleInput(const bool *keyboardInput) {
}

