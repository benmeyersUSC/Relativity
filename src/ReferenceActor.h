//
// Created by Ben Meyers on 2/14/26.
//

#ifndef RELATIVITY_REFERENCEACTOR_H
#define RELATIVITY_REFERENCEACTOR_H
#include "Actor.h"

class ImageComponent;

class ReferenceActor : public Actor{
    ImageComponent* mImage = nullptr;
    bool mSpacetimeDisplay = false;

protected:
     void HandleInput(const bool keys[], SDL_MouseButtonFlags mouseButtons,
                             const Vector2& posMouse) override;

public:
    void Setup();
};



#endif //RELATIVITY_REFERENCEACTOR_H
