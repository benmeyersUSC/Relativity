//
// Created by Ben Meyers on 2/13/26.
//

#include "ShapeComponent.h"

ShapeComponent::ShapeComponent(class Actor *owner) : Component(owner) {
}

void ShapeComponent::HandleUpdate(float deltaTime) {
    Component::HandleUpdate(deltaTime);
}
