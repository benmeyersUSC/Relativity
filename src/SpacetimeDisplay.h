//
// Created by Ben Meyers on 2/15/26.
//

#ifndef RELATIVITY_SPACETIMEDISPLAY_H
#define RELATIVITY_SPACETIMEDISPLAY_H
#include "Actor.h"

class Line;
class ImageComponent;
class DrawComponent;

class SpacetimeDisplay : public  Actor{
    static constexpr const char* ARROW_FILE = "../src/Assets/Arrow.png";
    static constexpr float NUM_TIME_PRINTS = 9.0f;

    DrawComponent* mDraw = nullptr;
    ImageComponent* mVeloArrow = nullptr;
    Line* yLevel = nullptr;
    std::vector<float> mActorPositions;
    std::vector<float> mActorVelocities;
    std::vector<float> mActorTimeVelocities;
    std::vector<float> mCumulativeProperTime;

    float mDt = 0.0f;

    unsigned mTimePrintInterval;
    float mYStep;
    void DrawPoints();
    void DrawMouse();
    void DrawVelocityMeter(float mouseVelo, float mouseTime);
protected:
    void HandleRender() override;

public:
    std::vector<float>& GetPositions(){return mActorPositions;}
    std::vector<float>& GetVelocities(){return mActorVelocities;}
    std::vector<float>& GetTimeVelocities(){return mActorTimeVelocities;}
    SpacetimeDisplay();
    [[nodiscard]] size_t GetTimestepIndex() const;
    void Setup();
    [[nodiscard]] float GetCumulativeAgeDiff() const;
    [[nodiscard]] float GetCumulativeAgeDiff(size_t i) const;
};


#endif //RELATIVITY_SPACETIMEDISPLAY_H