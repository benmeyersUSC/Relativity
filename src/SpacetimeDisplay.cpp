//
// Created by Ben Meyers on 2/15/26.
//

#include "SpacetimeDisplay.h"

#include <algorithm>
#include "DrawComponent.h"
#include "Game.h"
#include "Line.h"
#include "ImageComponent.h"

void SpacetimeDisplay::DrawPoints() {
    float y = Game::WINDOW_HEIGHT;

    // plot the points
    for (size_t i = 0; i < mActorPositions.size(); ++i) {
        // pos, velo, timeDil for plotting
        float mSpacetimePos = mActorPositions[i];
        float mSpacetimeVelo = mActorVelocities[i];
        // mul by two to make it brighter...magic
        float veloPct = Math::Abs(2.0f * (mSpacetimeVelo - Game::HALF_WIDTH)) / Game::HALF_WIDTH;

        // for printing dots and text
        bool neg = mSpacetimeVelo < Game::HALF_WIDTH;
        unsigned r = neg * Game::MAX_COLOR;
        unsigned g = !neg * Game::MAX_COLOR;
        auto alpha = static_cast<unsigned>(Game::MAX_COLOR * veloPct);

        // pos - white dots
        mDraw->AddRect(
        mSpacetimePos - Game::HALF_PLOT_POINT, y - Game::HALF_PLOT_POINT, Game::PLOT_POINT_SIZE, Game::PLOT_POINT_SIZE,
        Game::MAX_COLOR, Game::MAX_COLOR, Game::MAX_COLOR, Game::MAX_COLOR
        );

        // print the time dilation at various times
        if (i % mTimePrintInterval == 0 && i != 0){
            bool negativePosition = mSpacetimePos < Game::HALF_WIDTH;
            float shiftMag = 30.0f;
            float shift = static_cast<float>(negativePosition - !negativePosition) * (shiftMag * Game::PLOT_POINT_SIZE);
            mDraw->AddText(
            mSpacetimePos - Game::HALF_PLOT_POINT - Game::HALF_CHAR_PIXELS + shift,
                          y - Game::HALF_PLOT_POINT, DrawComponent::FormatString("%.2f%%", mActorTimeVelocities[i]),
                          1.35f
            );
        }

        // velocity-gauged velo plotting
        // - color is direction-gauged
        // - transparency is magnitude-gauged
        mDraw->AddRect(
        mSpacetimeVelo - Game::HALF_PLOT_POINT, y - Game::HALF_PLOT_POINT, Game::PLOT_POINT_SIZE, Game::PLOT_POINT_SIZE,
        r, g, 0, alpha
        );

        y -= mYStep;
    }
}

void SpacetimeDisplay::DrawMouse(size_t mouseHeightIndex, float coordinateTimeAtMouse) {
	// text box in bottom-left corner, scaled 1.5x
	float scale = 1.5f;
	float lineStep = (Game::CHAR_PIXELS + 4.0f) * scale;
	float textW = Game::CHAR_PIXELS * 27 * scale;
	float textH = lineStep * 4;
	float pad = 12.0f;
	float mouseTextX = pad * 2;
	float mouseTextY = Game::WINDOW_HEIGHT - textH - pad * 2;

	// outlined box
	mDraw->AddOutlineRect(mouseTextX - pad, mouseTextY - pad, textW + pad * 3, textH + pad * 2,
		Game::MAX_COLOR, Game::MAX_COLOR, Game::MAX_COLOR, Game::MAX_COLOR);

	// draw text
	mDraw->AddText(
		mouseTextX, mouseTextY, DrawComponent::FormatString("Space Position: %.1f px", mMousePos), scale
	);
	mDraw->AddText(
	mouseTextX, mouseTextY + lineStep, DrawComponent::FormatString("Space Velocity: %.1f px/s", mMouseVelo), scale
	);
	mDraw->AddText(
	mouseTextX, mouseTextY + lineStep * 2,DrawComponent::FormatString ("Aging at %.1f%% speed", mMouseTime), scale
	);
	mDraw->AddText(
	mouseTextX, mouseTextY + lineStep * 3, DrawComponent::FormatString("Age Difference: %.2fs", Math::Abs(coordinateTimeAtMouse - mCumulativeProperTime[mouseHeightIndex])), scale
	);


}

void SpacetimeDisplay::DrawVelocityMeter( ) {
	// lets make this all relative!
	constexpr float radius = 100.0f;
	constexpr int NUM_SEGMENTS = 50;
	constexpr float barThickness = 6.0f;

	// center of the half-circle base, to the left of the life bars
	float centerX = 1050.0f;
	float centerY = Game::WINDOW_HEIGHT - 55.0f;

	// half circle
	SDL_SetRenderDrawColor(gGame.GetRenderer(), Game::MAX_COLOR, Game::MAX_COLOR, Game::MAX_COLOR, 128);
	// turn on blend
	SDL_SetRenderDrawBlendMode(gGame.GetRenderer(), SDL_BLENDMODE_BLEND);
	// we are building a half circle out of little segments. for each segment we need to calculate
	// its start coordinates (based on one angle) and its end coordinates (based on another slightly larger angle)
	for (int i = 0; i < NUM_SEGMENTS; ++i) {
		// divide pi into NUM_SEGMENTS segments
		float theta1 = Math::Pi * static_cast<float>(i) / static_cast<float>(NUM_SEGMENTS);
		float theta2 = Math::Pi * static_cast<float>(i + 1) / static_cast<float>(NUM_SEGMENTS);
		SDL_RenderLine(gGame.GetRenderer(),
			// [// unit circle: center - cos(theta)] * actual radius!
			// and for y, we subtract (sdl) by the vertical components, sin
			centerX - radius * Math::Cos(theta1), centerY - radius * Math::Sin(theta1),
			centerX - radius * Math::Cos(theta2), centerY - radius * Math::Sin(theta2));
	}
	// base line
	SDL_RenderLine(gGame.GetRenderer(), centerX - radius, centerY, centerX + radius, centerY);
	// turn off blend
	SDL_SetRenderDrawBlendMode(gGame.GetRenderer(), SDL_BLENDMODE_NONE);

	// component projections

	// spatial velo projection
	float spatialFrac = mMouseVelo / Game::HALF_WIDTH; // by halfwidth because velo is already transformed
	// red for neg, green for pos
	Uint8 veloR, veloG, veloB, veloA;
	if (spatialFrac >= 0.0f) {
		veloR = 0;
		veloG = Game::MAX_COLOR;
		veloB = 0;
		veloA = Game::MAX_COLOR;
	} else {
		veloR = Game::MAX_COLOR;
		veloG = 0;
		veloB = 0;
		veloA = Game::MAX_COLOR;
	}
	// spatial bar grows from center: right if positive, left if negative
	bool negVelo = spatialFrac < 0.0f;
	mDraw->AddScaledWidthRect(centerX, centerY - barThickness / 2.0f, radius, barThickness,
		Math::Abs(spatialFrac), veloR, veloG, veloB, veloA, "v", 2.0f, 2.0f, negVelo);

	// time component
	float timeFrac = mMouseTime / 100.0f; // mouseTime was a full percentage
	mDraw->AddScaledHeightRect(centerX - barThickness / 2.0f, centerY - radius, barThickness, radius, timeFrac,
		255, 255, 32, Game::MAX_COLOR, "t", 3.0f, 2.0f);

	// arrow itself
	auto rotationSign = mMouseVelo >= 0.0f ? 1.0f : -1.0f;
	float angle = rotationSign * Math::ToDegrees(Math::Acos(mMouseTime / 100.0f));
	mVeloArrow->GetTransform().SetPosition({centerX, centerY});
	mVeloArrow->GetTransform().SetRotation(angle);

	// set arrow position to center of circle and rotate about that
	float scaledW = 0.0f, scaledH = 0.0f;
	SDL_GetTextureSize(gGame.GetTexture(ARROW_FILE), &scaledW, &scaledH);
	float meterScale = mVeloArrow->GetTransform().GetScale();
	scaledW *= meterScale;
	scaledH *= meterScale;
	// y is already set, now go to center (x-wise) of arrow for true pivot
	mVeloArrow->SetPivot({scaledW / 2.0f, scaledH});
}

void SpacetimeDisplay::DrawAgingBars(size_t mouseHeightIndex, float coordinateTimeAtMouse) {
	// --- cumulative time bars (bottom-right) ---
	float barMaxH = 270.0f;
	float barW = 36.0f;
	float barGap = 18.0f;
	float barPad = 21.0f;
	float barBaseY = Game::WINDOW_HEIGHT - barPad;
	float barRightEdge = Game::WINDOW_WIDTH - barPad;

	// buddha = index/size
	float buddhaFrac = static_cast<float>(mouseHeightIndex + 1) / static_cast<float>(mActorPositions.size());
	// player = cumulative time / total dur
	float playerFrac = mCumulativeProperTime[mouseHeightIndex] / Game::DURATION_SECONDS;

	// buddha bar rightmost
	float buddhaX = barRightEdge - barW;
	mDraw->AddScaledHeightRect(
		buddhaX, barBaseY - barMaxH, barW, barMaxH, buddhaFrac, 135, 108, 32, Game::MAX_COLOR,
		DrawComponent::FormatString("%.2fs", coordinateTimeAtMouse), 2.0f
	);

	// player bar left of buddha
	float playerX = buddhaX - barGap - barW;
	mDraw->AddScaledHeightRect(
		playerX, barBaseY - barMaxH, barW, barMaxH, playerFrac, 0, 0, Game::MAX_COLOR, Game::MAX_COLOR,
		DrawComponent::FormatString("%.2fs", mCumulativeProperTime[mouseHeightIndex]), 2.0f
	);

	// outline around bar area
	mDraw->AddOutlineRect(playerX - barPad / 2, barBaseY - barMaxH - barPad / 2, (barW * 2 + barGap) + barPad, barMaxH + barPad,
		Game::MAX_COLOR, Game::MAX_COLOR, Game::MAX_COLOR, Game::MAX_COLOR);
}

void SpacetimeDisplay::HandleRender() {
    Actor::HandleRender();
    yLevel->SetOrigin({0.0f, gGame.GetMousePos().y});

	// mouse-height(time)-gauged information
	// index is inverse height (sdl convention), quantized by datapoints, clamped at 0 and max-index
	auto mouseHeightIndex = static_cast<size_t>(std::clamp((Game::WINDOW_HEIGHT - gGame.GetMousePos().y) / mYStep, 0.0f, static_cast<float>(mActorPositions.size() - 1)));
	mMouseTime = mActorTimeVelocities[mouseHeightIndex];
	// de-transforming values so centered in middle of plot
	mMousePos = mActorPositions[mouseHeightIndex] - Game::HALF_WIDTH;
	mMouseVelo = mActorVelocities[mouseHeightIndex] - Game::HALF_WIDTH;
	float coordinateTimeAtMouse = (mouseHeightIndex + 1) * Game::DURATION_SECONDS / mActorPositions.size();


    DrawPoints();
    DrawMouse(mouseHeightIndex,  coordinateTimeAtMouse);
	// spacetime velocity meter
	DrawVelocityMeter( );
	DrawAgingBars(mouseHeightIndex, coordinateTimeAtMouse);
}

SpacetimeDisplay::SpacetimeDisplay() {
    constexpr auto samples = static_cast<unsigned>(Game::WINDOW_HEIGHT / Game::PLOT_POINT_SIZE);
    mActorPositions.reserve(samples);
    mActorVelocities.reserve(samples);
    mActorTimeVelocities.reserve(samples);

    auto xAxis = gGame.CreateActor<Line>();
    xAxis->SetOrigin(Line::X_AXIS_ORIGIN);
    xAxis->SetSlope(Line::X_AXIS_SLOPE);

    auto yAxis = gGame.CreateActor<Line>();
    yAxis->SetOrigin(Line::Y_AXIS_ORIGIN);
    yAxis->SetSlope(Line::Y_AXIS_SLOPE);

    yLevel = gGame.CreateActor<Line>();
    yLevel->SetSlope(Line::X_AXIS_SLOPE);

    mDraw = CreateComponent<DrawComponent>();

	mVeloArrow = CreateComponent<ImageComponent>();
	mVeloArrow->SetUseLocalTransform(true);
	mVeloArrow->GetTransform().SetScale(0.24f);
	mVeloArrow->SetTexture(gGame.GetTexture(ARROW_FILE));
}

size_t SpacetimeDisplay::GetTimestepIndex() const {
   return static_cast<size_t>(std::clamp((Game::WINDOW_HEIGHT - gGame.GetMousePos().y) / mYStep, 0.0f, static_cast<float>(mActorPositions.size() - 1)));
}

void SpacetimeDisplay::Setup(){
    mTimePrintInterval = std::max(1u, static_cast<unsigned>(static_cast<float>(mActorPositions.size()) / NUM_TIME_PRINTS));
    mYStep = Game::WINDOW_HEIGHT / static_cast<float>(mActorPositions.size());

    mDt = Game::DURATION_SECONDS / static_cast<float>(mActorTimeVelocities.size());
    mCumulativeProperTime.resize(mActorTimeVelocities.size());

    mCumulativeProperTime[0] = mDt * mActorTimeVelocities[0] / 100.0f;
    for (size_t i = 1; i < mCumulativeProperTime.size(); ++i) {
        mCumulativeProperTime[i] = mCumulativeProperTime[i-1] + mDt * mActorTimeVelocities[i] / 100.0f;
    }
}

float SpacetimeDisplay::GetCumulativeAgeDiff(size_t i) const {
    return mCumulativeProperTime[i];
}

float SpacetimeDisplay::GetCumulativeAgeDiff() const {
    return mCumulativeProperTime.back();
}
