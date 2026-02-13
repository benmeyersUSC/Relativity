//
// Created by Ben Meyers on 2/12/26.
//

#ifndef RELATIVITY_SPACETIMEPLOT_H
#define RELATIVITY_SPACETIMEPLOT_H
#include <vector>
class Image;
class SpacetimePlot {
    static constexpr const char* ARROW_FILE = "../src/Assets/Arrow.png";
    static constexpr const char* BUDDHA_FILE = "../src/Assets/Buddha.webp";
    static constexpr float NUM_TIME_PRINTS = 9.0f;
    std::vector<float> mActorPositions;
    std::vector<float> mActorVelocities;
    std::vector<float> mActorTimeVelocities;
    std::vector<float> mCumulativeProperTime;
    float mDt = 0.0f;

    static void DrawYAxis();
    void DrawPoints(float yStep, unsigned timePrintInterval);
    void DrawMouse(float yStep);

    std::unique_ptr<Image> mArrow;
    std::unique_ptr<Image> mBuddha;
public:
    std::vector<float>& GetPositions(){return mActorPositions;}
    std::vector<float>& GetVelocities(){return mActorVelocities;}
    std::vector<float>& GetTimeVelocities(){return mActorTimeVelocities;}
    void Draw();
    SpacetimePlot() ;
    void Setup();
    float GetCumulativeAgeDiff();
    float GetCumulativeAgeDiff(size_t i);
};


/*

proper time elapsed by mover:
    coordinate (rest) DURATION * average_of(√(1 - (v_i / MAX_VELO)²))
this makes sense. coordinate duration * the average of (Z)...what is Z?

Z = √(1 - (v_i / MAX_VELO)²)
let's derive Z...first, recognize:
    - v_i (velocity at period i) is dx_i/dt_i
    - MAX_VELO is our 'speed of light' (c)
...

fundamental invariant in spacetime;
ds² = c²dt² - dx²
    and
ds² = c²dτ²

s = spacetime interval; the 'distance' between two events in spacetime
t = coordinate time (measured by resting object)
x = spatial coordinate of moving object
c = speed of light or MAX_VELO
τ = proper time experienced by mover

so ds² (the squared change in spacetime) is made up of two competing terms:
    - c²dt²
    -- squared change in time, scaled by c for like units
    -- c * dt = how far light travelled in dt! (if t were a year, we'd have lightyears)
    - dx²
    -- scaled spatial change
so the spacetime interval, s, is proportional to the difference between
how much light moves in some unit of time and how much the object moves

the ds² term is invariant; let's equate two equations:

ds² = c²dt² - dx²
    and
ds² = c²dτ²

become:

c²dτ² = c²dt² - dx²
...
c²dτ² + dx² = c²dt²

dt is just the resting viewer's clock speed. it is unchanging and finite. 
we have dx (mover's change in space) and c * dτ (mover's 'light-distance' in
their own experienced time). 
as the mover moves more (higher velocity = dx/dt), the lower must be dτ 
(the rate at which they experience time)


c²dτ² = c²dt² - dx²          // divide both sides by c²dt²
(dτ/dt)² = 1 - (dx/dt)²/c²   // recognize dx/dt = v
(dτ/dt)² = 1 - v²/c²
dτ/dt   = √(1 - v²/c²)       // that's Z



*/


#endif //RELATIVITY_SPACETIMEPLOT_H