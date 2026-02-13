//
// Created by Ben Meyers on 2/11/26.
//

#include "Actor.h"
#include "Game.h"
Actor::Actor(unsigned frameLife) {
    mPositions.reserve(frameLife);
    mVelocities.reserve(frameLife);
    mTimeVelocities.reserve(frameLife);
}

void Actor::HandleUpdate(float deltaTime) {
    mVelocities.push_back(mVelo);
    mPositions.push_back(GetTransform().GetPosition().x);
    
    // LORENTZ
    // mVelo^2 + timeVelo^2 = MAX_VELO^2
    // timeVelo = SQRT(
    //      MAX_VELO^2 - mVelo^2
    // )
    mTimeVelocities.push_back(
        Math::Sqrt(
            Game::MAX_VELO * Game::MAX_VELO - mVelo * mVelo
        )
    );
}

void Actor::HandleInput(const bool keys[], SDL_MouseButtonFlags mouseButtons,
                             const Vector2& posMouse) {
}

float Actor::GetTimeFactor() const {return mTimeVelocities.back()/Game::MAX_VELO;}

