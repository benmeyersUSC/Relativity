//
// Created by Ben Meyers on 2/11/26.
//

#ifndef RELATIVITY_ACTOR_H
#define RELATIVITY_ACTOR_H
#include <vector>
#include "Transform.h"

class Actor {
public:
    Transform& GetTransform() { return mTransform; }
    [[nodiscard]] const Transform& GetTransform() const { return mTransform; }

    explicit Actor(const unsigned frameLife){mVelos.reserve(frameLife);}
    void HandleUpdate(float deltaTime);
    void HandleInput(const bool* keyboardInput);
protected:
    Transform mTransform;
    float mVelo = 0.0f;
    std::vector<float> mVelos;


};


#endif //RELATIVITY_ACTOR_H