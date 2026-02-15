//
// Created by Ben Meyers on 2/15/26.
//

#ifndef RELATIVITY_SPACETIMEREFERENCE_H
#define RELATIVITY_SPACETIMEREFERENCE_H

#include "Actor.h"
class ImageComponent;
class SpacetimeReference : public Actor {
    ImageComponent* mImage = nullptr;
public:
    SpacetimeReference();
protected:
    void HandleInput(const bool keys[], SDL_MouseButtonFlags mouseButtons, const Vector2 &posMouse) override;
};


#endif //RELATIVITY_SPACETIMEREFERENCE_H