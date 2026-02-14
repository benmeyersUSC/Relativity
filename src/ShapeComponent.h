//
// Created by Ben Meyers on 2/13/26.
//

#ifndef RELATIVITY_SHAPECOMPONENT_H
#define RELATIVITY_SHAPECOMPONENT_H
#include "Component.h"


class ShapeComponent : public Component{
protected:
    explicit ShapeComponent(class Actor *owner);
    void HandleUpdate(float deltaTime) override;

    friend class Actor;
};


#endif //RELATIVITY_SHAPECOMPONENT_H