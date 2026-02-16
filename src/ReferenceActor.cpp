//
// Created by Ben Meyers on 2/14/26.
//

#include "ReferenceActor.h"
#include "Game.h"
#include "ImageComponent.h"

void ReferenceActor::HandleInput(const bool keys[], SDL_MouseButtonFlags mouseButtons,
                                 const Vector2& posMouse) {
    if (mSpacetimeDisplay) {
        GetTransform().SetPosition({Game::HALF_WIDTH, posMouse.y});
    }


    // mImage->Draw(); // is called in handleupdate below
    // update components
    Actor::HandleInput(keys, mouseButtons, posMouse);
}

void ReferenceActor::Setup() {
    mImage = CreateComponent<ImageComponent>();
    mImage->SetTexture(gGame.GetTexture(Game::BUDDHA_FILE));
}

