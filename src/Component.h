//
// Created by Ben Meyers on 2/13/26.
//

#ifndef RELATIVITY_COMPONENT_H
#define RELATIVITY_COMPONENT_H


#pragma once
#include "SDL3/SDL_mouse.h"
#include "Math.h"
class Actor;

class Component
{
    friend class Actor;

public:
    Actor* GetOwner() const;

    void Update(float deltaTime);

    void Input(const bool keys[], SDL_MouseButtonFlags mouseButtons, const Math::Vector2& posMouse);

protected:
    explicit Component(Actor* owner);
    virtual ~Component();

    virtual void HandleUpdate(float deltaTime);

    virtual void HandleInput(const bool keys[], SDL_MouseButtonFlags mouseButtons,
                             const Math::Vector2& posMouse);

private:
    Actor* mOwner;
};

#endif //RELATIVITY_COMPONENT_H