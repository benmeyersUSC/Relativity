//
// Created by Ben Meyers on 2/13/26.
//
#include "SpacetimeComponent.h"
#include "Actor.h"
#include "Game.h"

float SpacetimeComponent::GetProperOverCoordinateTime() const {
    return mProperTimes.back()/Game::MAX_VELO;
}

void SpacetimeComponent::Setup(size_t frameLife) {
    mSpatialPositions.reserve(frameLife);
    mSpatialVelocities.reserve(frameLife);
    mProperTimes.reserve(frameLife);
}

float SpacetimeComponent::ElapsedPct() const {
    return gGame.GetDT() / Game::DURATION_SECONDS;
}

SpacetimeComponent::SpacetimeComponent(class Actor *owner)
    : Component(owner) {
}

void SpacetimeComponent::HandleUpdate(float deltaTime) {
    Component::HandleUpdate(deltaTime);

    mSpatialVelocities.push_back(mSpatialVelo);
    mSpatialPositions.push_back(GetOwner()->GetTransform().GetPosition().x);

    // LORENTZ
    // mVelo^2 + timeVelo^2 = MAX_VELO^2
    // timeVelo = SQRT(
    //      MAX_VELO^2 - mVelo^2
    // )
    mProperTimes.push_back(
        Math::Sqrt(
            Game::MAX_VELO * Game::MAX_VELO - mSpatialVelo * mSpatialVelo
        )
    );
}
