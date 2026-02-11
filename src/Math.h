//
// Created by Ben Meyers on 2/11/26.
//

#ifndef RELATIVITY_MATH_H
#define RELATIVITY_MATH_H
namespace Math {
    class Vector2 {
        float mX;
        float mY;
    public:
        Vector2():mX(0.0f), mY(0.0f){}
        Vector2(float x, float y): mX(x), mY(y){}
        [[nodiscard]] float X() const{return mX;}
        [[nodiscard]] float Y() const{return mY;}
    };

    inline float Max(float a, float b) {
        return a >= b ? a : b;
    }

    inline float Min(float a, float b) {
        return a <= b ? a : b;
    }
}
#endif //RELATIVITY_MATH_H