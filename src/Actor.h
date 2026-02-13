//
// Created by Ben Meyers on 2/11/26.
//

#ifndef RELATIVITY_ACTOR_H
#define RELATIVITY_ACTOR_H
#include <vector>

#include "Transform.h"
#include "SDL3/SDL_mouse.h"

class Actor {
public:
    virtual ~Actor() = default;

    Transform& GetTransform() { return mTransform; }
    [[nodiscard]] const Transform& GetTransform() const { return mTransform; }

    explicit Actor( unsigned frameLife);
    virtual void HandleUpdate(float deltaTime);
    virtual void HandleInput(const bool keys[], SDL_MouseButtonFlags mouseButtons,
                             const Vector2& posMouse);

    [[nodiscard]] std::vector<float>& GetPositions() {return mPositions;}
    [[nodiscard]] std::vector<float>& GetVelocities() {return mVelocities;}
    [[nodiscard]] std::vector<float>& GetTimeVelocities() {return mTimeVelocities;}
    [[nodiscard]] float GetVelocity()const{return mVelo;}

    [[nodiscard]] float GetTimeFactor()const;
protected:
    Transform mTransform;
    float mVelo = 0.0f;
    std::vector<float> mPositions;
    std::vector<float> mVelocities;
    std::vector<float> mTimeVelocities;
};


#endif //RELATIVITY_ACTOR_H