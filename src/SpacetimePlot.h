//
// Created by Ben Meyers on 2/12/26.
//

#ifndef RELATIVITY_SPACETIMEPLOT_H
#define RELATIVITY_SPACETIMEPLOT_H
#include <vector>
class Image;
class SpacetimePlot {
    static constexpr const char* ARROW_FILE = "Assets/Arrow.png";
    static constexpr float NUM_TIME_PRINTS = 18.0f;
    std::vector<float> mActorPositions;
    std::vector<float> mActorVelocities;
    std::vector<float> mActorTimeVelocities;

    static void DrawYAxis();
    void DrawPoints(float yStep, unsigned timePrintInterval);
    void DrawMouse(float yStep);

    std::unique_ptr<Image> mArrow;
public:
    std::vector<float>& GetPositions(){return mActorPositions;}
    std::vector<float>& GetVelocities(){return mActorVelocities;}
    std::vector<float>& GetTimeVelocities(){return mActorTimeVelocities;}
    void DrawSpacetime();
    SpacetimePlot() ;
};


#endif //RELATIVITY_SPACETIMEPLOT_H