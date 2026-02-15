//
// Created by Ben Meyers on 2/13/26.
//
#include "SpacetimeComponent.h"
#include "Actor.h"
#include "Game.h"

float SpacetimeComponent::GetTimeFactor() const {
    return mTimeVelocities.back()/Game::MAX_VELO;
}

void SpacetimeComponent::Setup(size_t frameLife) {
    mPositions.reserve(frameLife);
    mVelocities.reserve(frameLife);
    mTimeVelocities.reserve(frameLife);
}

float SpacetimeComponent::LifePct() const {
    return static_cast<float>(mPositions.size()) / static_cast<float>(mPositions.capacity());
}

SpacetimeComponent::SpacetimeComponent(class Actor *owner)
    : Component(owner) {
}

void SpacetimeComponent::HandleUpdate(float deltaTime) {
    Component::HandleUpdate(deltaTime);

    mVelocities.push_back(mVelo);
    mPositions.push_back(GetOwner()->GetTransform().GetPosition().x);

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
