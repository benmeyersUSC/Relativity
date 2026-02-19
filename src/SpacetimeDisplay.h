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
    static constexpr float NUM_TIME_PRINTS = 9.0f;

    DrawComponent* mDraw = nullptr;
    ImageComponent* mVeloArrow = nullptr;
    Line* yLevel = nullptr;
    std::vector<float> mActorPositions;
    std::vector<float> mActorVelocities;
    std::vector<float> mActorProperTimes;
    std::vector<float> mCumulativeProperTime;

    float mDt = 0.0f;
    float mMousePos = 0.0f;
    float mMouseVelo = 0.0f;
    float mMouseTime = 0.0f;

    unsigned mTimePrintInterval{};
    float mYStep{};
    void DrawPoints();
    void DrawMouse(size_t mouseHeightIndex, float coordinateTimeAtMouse);
    void DrawVelocityMeter();
    void DrawAgingBars(size_t mouseHeightIndex, float coordinateTimeAtMouse);

protected:
    void HandleRender() override;

public:
    [[nodiscard]] float GetMouseBasedPos()const {return mMousePos;}
    [[nodiscard]] float GetMouseBasedVelo()const {return mMouseVelo;}
    [[nodiscard]] float GetMouseBasedTime()const {return mMouseTime;}

    std::vector<float>& GetPositions(){return mActorPositions;}
    std::vector<float>& GetVelocities(){return mActorVelocities;}
    std::vector<float>& GetProperTimes(){return mActorProperTimes;}
    SpacetimeDisplay();
    [[nodiscard]] size_t GetTimestepIndex() const;
    void Setup();
    [[nodiscard]] float GetCumulativeAgeDiff() const;
    [[nodiscard]] float GetCumulativeAgeDiff(size_t i) const;
};


#endif //RELATIVITY_SPACETIMEDISPLAY_H