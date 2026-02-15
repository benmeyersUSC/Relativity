//
// Created by Ben Meyers on 2/15/26.
//

#ifndef RELATIVITY_SPACETIMEPLAYER_H
#define RELATIVITY_SPACETIMEPLAYER_H
#include "Actor.h"
class ImageComponent;
class DrawComponent;
class SpacetimePlayer : public Actor{
    ImageComponent* mImage = nullptr;
    DrawComponent* mDraw = nullptr;
public:
    SpacetimePlayer();
protected:
    void HandleInput(const bool keys[], SDL_MouseButtonFlags mouseButtons, const Vector2 &posMouse) override;
    void HandleRender() override;
};


#endif //RELATIVITY_SPACETIMEPLAYER_H