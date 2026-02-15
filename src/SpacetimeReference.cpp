//
// Created by Ben Meyers on 2/15/26.
//

#include "SpacetimeReference.h"
#include "ImageComponent.h"
#include "Game.h"
SpacetimeReference::SpacetimeReference() {
    mImage = CreateComponent<ImageComponent>();
    mImage->SetTexture(gGame.GetTexture(Game::BUDDHA_FILE));
    GetTransform().SetScale(0.1f);
}

void SpacetimeReference::HandleInput(const bool keys[], SDL_MouseButtonFlags mouseButtons, const Vector2 &posMouse) {
    Actor::HandleInput(keys, mouseButtons, posMouse);
    GetTransform().SetPosition({Game::HALF_WIDTH, posMouse.y});
}
