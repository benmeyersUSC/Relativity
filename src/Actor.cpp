//
// Created by Ben Meyers on 2/11/26.
//

#include "Actor.h"

#include "Component.h"
#include "Game.h"

Actor::~Actor() {
    for (auto& comp : mComponents)
    {
        delete comp;
    }
    mComponents.clear();
}

Actor::Actor(unsigned frameLife) {
    mPositions.reserve(frameLife);
    mVelocities.reserve(frameLife);
    mTimeVelocities.reserve(frameLife);
}

void Actor::Update(float deltaTime)
{
    for (auto& comp : mComponents)
    {
        comp->Update(deltaTime);
    }
    HandleUpdate(deltaTime);
}
void Actor::Input(const bool keys[], SDL_MouseButtonFlags mouseButtons,
                  const Vector2& posMouse)
{
    for (auto& c : mComponents)
    {
        c->Input(keys, mouseButtons, posMouse);
    }
    HandleInput(keys, mouseButtons, posMouse);
}

void Actor::Destroy()
{
    gGame.AddPendingDestroy(this);
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

