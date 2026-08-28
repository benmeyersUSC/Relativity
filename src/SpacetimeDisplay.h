//
// Created by Ben Meyers on 2/15/26.
//

#ifndef RELATIVITY_SPACETIMEDISPLAY_H
#define RELATIVITY_SPACETIMEDISPLAY_H
#include "Actor.h"
#include "Game.h"

class Line;
class ImageComponent;
class DrawComponent;

class SpacetimeDisplay : public  Actor{
    static constexpr float NUM_TIME_PRINTS = 9.0f;

    static constexpr float BTN_W = 140.0f;
    static constexpr float BTN_H = 34.0f;
    static constexpr float BTN_X = Game::PLOT_WIDTH + (Game::SIDEBAR_WIDTH - BTN_W) / 2.0f;
    static constexpr float BTN_Y = 205.0f;

    bool mLastClick = false;
    std::function<void()> mOnRestartClick = []{ gGame.RequestRestart(); };

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
    void DrawPoints() const;
    void DrawMouseGaugedTextBox(size_t mouseHeightIndex, float coordinateTimeAtMouse) const;
    void DrawVelocityMeter() const;
    void DrawAgingBars(size_t mouseHeightIndex, float coordinateTimeAtMouse) const;
    void DrawMeterCircle( float centerX,  float centerY,  float radius) const;
    void DrawMeterArrow(float centerX, float centerY) const;
    void DrawMeterComponentProjections( float centerX,  float centerY,  float radius) const;
    void DrawMeterReferenceArrows(float centerX, float centerY, float radius) const;
    void DrawRestartButton() const;

protected:
    void HandleRender() override;
    void HandleInput(const bool keys[], SDL_MouseButtonFlags mouseButtons, const Vector2& posMouse) override;

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