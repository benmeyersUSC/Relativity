//
// Created by Ben Meyers on 2/13/26.
//

#include "Component.h"

Component::Component(Actor* owner)
{
    mOwner = owner;
}
Component::~Component() = default;
void Component::HandleUpdate(float deltaTime)
{
}
void Component::HandleInput(const bool keys[], SDL_MouseButtonFlags mouseButtons,
                            const Math::Vector2& relativeMouse)
{
}
Actor* Component::GetOwner() const
{
    return mOwner;
}
void Component::Update(float deltaTime)
{
    HandleUpdate(deltaTime);
}
void Component::Input(const bool keys[], SDL_MouseButtonFlags mouseButtons,
                      const Math::Vector2& relativeMouse)
{
    HandleInput(keys, mouseButtons, relativeMouse);
}