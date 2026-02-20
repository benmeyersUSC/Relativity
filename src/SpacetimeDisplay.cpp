//
// Created by Ben Meyers on 2/15/26.
//

#include "SpacetimeDisplay.h"

#include <algorithm>
#include "DrawComponent.h"
#include "Game.h"
#include "Line.h"
#include "ImageComponent.h"

void SpacetimeDisplay::DrawPoints() const {
    float y = Game::WINDOW_HEIGHT;

    // plot the points
    for (size_t i = 0; i < mActorPositions.size(); ++i) {
        // pos, velo, timeDil for plotting
        const float position = mActorPositions[i];
        const float velocity = mActorVelocities[i];

    	// the reason we adjust by HALF-WIDTH is because
    	// this is already transformed velocity
        const float veloPct = Math::Abs(velocity - Game::HALF_WIDTH) / Game::HALF_WIDTH;

        // red for neg velo, green for pos
        const bool neg = velocity < Game::HALF_WIDTH;
        const unsigned r = neg * Game::MAX_COLOR;
        const unsigned g = !neg * Game::MAX_COLOR;
    	// magnitude determines brightness
        const auto alpha = static_cast<unsigned>(Game::MAX_COLOR * veloPct);

        // pos - white dots
        mDraw->AddRect(
	        position - Game::HALF_PLOT_POINT_SIZE,
	        y - Game::HALF_PLOT_POINT_SIZE,

	        Game::PLOT_POINT_SIZE,Game::PLOT_POINT_SIZE,
        Game::MAX_COLOR, Game::MAX_COLOR, Game::MAX_COLOR, Game::MAX_COLOR
        );

        // print the time dilation at various times
        if (i % mTimePrintInterval == 0 && i != 0){
            const bool negativePosition = position < Game::HALF_WIDTH;
        	constexpr float textScale = 1.35f;
        	//																			6 characters in 'XX.XX%'
            const float shift = static_cast<float>(negativePosition - !negativePosition) * 6.0f * Game::CHAR_PIXELS * textScale;
            mDraw->AddText(
            position - Game::HALF_PLOT_POINT_SIZE - Game::HALF_CHAR_PIXELS + shift,
                          y - Game::HALF_PLOT_POINT_SIZE, DrawComponent::FormatString("%.2f%%", mActorProperTimes[i] * 100.0f),
                          textScale
            );
        }

        // velocity-gauged velo plotting
        // - color is direction-gauged
        // - transparency is magnitude-gauged
        mDraw->AddRect(
	        velocity - Game::HALF_PLOT_POINT_SIZE, y - Game::HALF_PLOT_POINT_SIZE,
	        Game::PLOT_POINT_SIZE, Game::PLOT_POINT_SIZE,
	        r, g, 0, alpha
        );

        y -= mYStep;
    }
}

void SpacetimeDisplay::DrawMouseGaugedTextBox(const size_t mouseHeightIndex, const float coordinateTimeAtMouse) const {
	// text box in sidebar, near the bottom
	constexpr float scale = 1.5f;
	constexpr float lineStep = (Game::CHAR_PIXELS + 4.0f) * scale;
	constexpr float textH = lineStep * 4;
	constexpr float pad = 16.0f;
	constexpr float mouseTextX = Game::PLOT_WIDTH + pad * 2;
	constexpr float mouseTextY = Game::WINDOW_HEIGHT - textH - pad * 2;

	// outlined box
	constexpr float boxW = Game::WINDOW_WIDTH - mouseTextX;
	mDraw->AddOutlineRect(
		mouseTextX - pad, mouseTextY - pad,
		boxW, textH + pad * 2,
		Game::MAX_COLOR, Game::MAX_COLOR, Game::MAX_COLOR, Game::MAX_COLOR);

	// draw text
	mDraw->AddText(
		mouseTextX, mouseTextY, DrawComponent::FormatString("Space Position: %.1f px", mMousePos), scale
	);
	mDraw->AddText(
		mouseTextX, mouseTextY + lineStep, DrawComponent::FormatString("Space Velocity: %.1f px/s", mMouseVelo), scale
	);
	mDraw->AddText(
		mouseTextX, mouseTextY + lineStep * 2, DrawComponent::FormatString("Aging at %.1f%% speed", mMouseTime * 100.0f), scale
	);
	mDraw->AddText(
		mouseTextX, mouseTextY + lineStep * 3, DrawComponent::FormatString("Age Difference: %.2fs", Math::Abs(coordinateTimeAtMouse - mCumulativeProperTime[mouseHeightIndex])), scale
	);
}

void SpacetimeDisplay::DrawMeterCircle(const float centerX, const float centerY, const float radius) const {
	constexpr int NUM_SEGMENTS = 50;

	// half circle
	// we are building a half circle out of little segments. for each segment we need to calculate
	// its start coordinates (based on one angle) and its end coordinates (based on another slightly larger angle)
	for (int i = 0; i < NUM_SEGMENTS; ++i) {
		// divide pi into NUM_SEGMENTS segments
		const float theta1 = Math::Pi * static_cast<float>(i) / static_cast<float>(NUM_SEGMENTS);
		const float theta2 = Math::Pi * static_cast<float>(i + 1) / static_cast<float>(NUM_SEGMENTS);
		mDraw->AddLine(
			// [// unit circle: center - cos(theta)] * actual radius!
			// and for y, we subtract (sdl) by the vertical components, sin
			centerX - radius * Math::Cos(theta1), centerY - radius * Math::Sin(theta1),
			centerX - radius * Math::Cos(theta2), centerY - radius * Math::Sin(theta2),
			Game::MAX_COLOR, Game::MAX_COLOR, Game::MAX_COLOR, 128
		);
	}
	// base line
	// SDL_RenderLine(gGame.GetRenderer(), centerX - radius, centerY, centerX + radius, centerY);
	mDraw->AddLine(
		centerX - radius, centerY, centerX + radius, centerY,
		Game::MAX_COLOR, Game::MAX_COLOR, Game::MAX_COLOR, 128
	);
}

void SpacetimeDisplay::DrawMeterArrow(const float centerX, const float centerY) const {
	// arrow itself
	mVeloArrow->GetTransform().SetPosition({centerX, centerY});

	const auto rotationSign = mMouseVelo >= 0.0f ? 1.0f : -1.0f;
	// mMouseTime is the vertical component of the spacetime vector
	// since we're rotating the arrow off the y axis, that vertical segment is 'adjacent'
	// inverse cosine of mMousetime/Hypotenuse(1) = the angle by which we need to rotate the arrow
	const float angle = rotationSign * Math::ToDegrees(Math::Acos(mMouseTime));
	mVeloArrow->GetTransform().SetRotation(angle);

	// set arrow position to center of circle and rotate about that
	float scaledW = 0.0f, scaledH = 0.0f;
	SDL_GetTextureSize(gGame.GetTexture(Game::ARROW_FILE), &scaledW, &scaledH);
	const float meterScale = mVeloArrow->GetTransform().GetScale();
	scaledW *= meterScale;
	scaledH *= meterScale;
	// from top left origin, pivot is half-width over, height down
	mVeloArrow->SetPivot({scaledW / 2.0f, scaledH});
}

void SpacetimeDisplay::DrawMeterComponentProjections(const float centerX, const float centerY, const float radius)const {
	constexpr float barThickness = 6.0f;

	// component projections
	// spatial velo projection
	const float spatialFrac = mMouseVelo / Game::HALF_WIDTH; // by halfwidth because velo is already transformed
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
	mDraw->AddScaledWidthRect(
		centerX, centerY - barThickness / 2.0f, radius, barThickness,
		Math::Abs(spatialFrac),
		veloR, veloG, veloB, veloA, "v", 2.0f, 2.0f, spatialFrac < 0.0f
	);

	// time component
	mDraw->AddScaledHeightRect(centerX - barThickness / 2.0f, centerY - radius, barThickness, radius, mMouseTime,
		255, 255, 32, Game::MAX_COLOR, "t", 3.0f, 2.0f);
}

void SpacetimeDisplay::DrawVelocityMeter() const {
	constexpr float radius = 100.0f;
	// center in sidebar, above the text box
	constexpr float centerX = Game::PLOT_WIDTH + Game::SIDEBAR_WIDTH / 2.0f;
	// text box occupies roughly bottom 120px of sidebar
	constexpr float textBoxTop = Game::WINDOW_HEIGHT - 120.0f;
	constexpr float centerY = textBoxTop - 16.0f - radius;

	DrawMeterCircle(centerX, centerY, radius);
	DrawMeterComponentProjections(centerX, centerY, radius);
	DrawMeterArrow(centerX, centerY);
}

void SpacetimeDisplay::DrawAgingBars(const size_t mouseHeightIndex, const float coordinateTimeAtMouse) const {
	constexpr float pad = 16.0f;
	constexpr float barMaxW = Game::SIDEBAR_WIDTH - pad * 4;
	constexpr float barH = 36.0f;
	constexpr float barGap = 18.0f;
	constexpr float barBaseX = Game::PLOT_WIDTH + pad * 2;
	constexpr float barTopY = pad * 3;

	// buddha = index/size
	const float buddhaFrac = static_cast<float>(mouseHeightIndex + 1) / static_cast<float>(mActorPositions.size());
	// player = cumulative time / total dur :)
	const float playerFrac = mCumulativeProperTime[mouseHeightIndex] / Game::DURATION_SECONDS;

	// textbox label
	constexpr std::string txt("Cumulative aging");
	constexpr auto mul = 1.35f;
	const float txtW = static_cast<float>(txt.size()) * Game::CHAR_PIXELS * mul;
	constexpr float sidebarCenterX = Game::PLOT_WIDTH + Game::SIDEBAR_WIDTH / 2.0f;
	mDraw->AddText(sidebarCenterX - txtW / 2.0f, barTopY, txt, mul);

	// player bar (top)
	constexpr float playerY = barTopY + Game::CHAR_PIXELS * mul + pad;
	mDraw->AddScaledWidthRect(
		barBaseX, playerY, barMaxW, barH, playerFrac, 0, 0, Game::MAX_COLOR, Game::MAX_COLOR,
		DrawComponent::FormatString("%.2fs", mCumulativeProperTime[mouseHeightIndex]), 2.0f, 1.35f
	);

	// buddha bar (below player)
	constexpr float buddhaY = playerY + barH + barGap;
	mDraw->AddScaledWidthRect(
		barBaseX, buddhaY, barMaxW, barH, buddhaFrac, 135, 108, 32, Game::MAX_COLOR,
		DrawComponent::FormatString("%.2fs", coordinateTimeAtMouse), 2.0f, 1.35f
	);

	// outline around bar area
	constexpr float outlineY = playerY - pad / 2;
	mDraw->AddOutlineRect(barBaseX - pad / 2, outlineY, barMaxW + pad, (barH * 2 + barGap) + pad,
		Game::MAX_COLOR, Game::MAX_COLOR, Game::MAX_COLOR, Game::MAX_COLOR);
}

void SpacetimeDisplay::HandleRender() {
    Actor::HandleRender();
    yLevel->SetOrigin({0.0f, gGame.GetMousePos().y});

	// mouse-height(time)-gauged information
	// index is inverse height (sdl convention), quantized by datapoints, clamped at 0 and max-index
	const auto mouseHeightQuantized = static_cast<size_t>(Math::Clamp(
		(Game::WINDOW_HEIGHT - gGame.GetMousePos().y) / mYStep,
		0.0f, static_cast<float>(mActorPositions.size() - 1))
	);
	mMouseTime = mActorProperTimes[mouseHeightQuantized];
	// de-transforming values so centered in middle of plot
	mMousePos = mActorPositions[mouseHeightQuantized] - Game::HALF_WIDTH;
	mMouseVelo = mActorVelocities[mouseHeightQuantized] - Game::HALF_WIDTH;
	const float coordinateTimeAtMouse = static_cast<float>(mouseHeightQuantized + 1) * Game::DURATION_SECONDS / static_cast<float>(mActorPositions.size());

    DrawPoints();

	// sidebar widgets
    DrawMouseGaugedTextBox(mouseHeightQuantized, coordinateTimeAtMouse);
	DrawVelocityMeter();
	DrawAgingBars(mouseHeightQuantized, coordinateTimeAtMouse);
}

SpacetimeDisplay::SpacetimeDisplay() {
    constexpr auto samples = static_cast<unsigned>(Game::WINDOW_HEIGHT / Game::PLOT_POINT_SIZE);
    mActorPositions.reserve(samples);
    mActorVelocities.reserve(samples);
    mActorProperTimes.reserve(samples);

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
	mVeloArrow->SetTexture(gGame.GetTexture(Game::ARROW_FILE));
}

size_t SpacetimeDisplay::GetTimestepIndex() const {
   return static_cast<size_t>(Math::Clamp(
   		(Game::WINDOW_HEIGHT - gGame.GetMousePos().y) / mYStep,
   		0.0f, static_cast<float>(mActorPositions.size() - 1))
   	);
}

void SpacetimeDisplay::Setup(){
    mTimePrintInterval = std::max(1u, static_cast<unsigned>(static_cast<float>(mActorPositions.size()) / NUM_TIME_PRINTS));
    mYStep = Game::WINDOW_HEIGHT / static_cast<float>(mActorPositions.size());

    mDt = Game::DURATION_SECONDS / static_cast<float>(mActorProperTimes.size());
    mCumulativeProperTime.resize(mActorProperTimes.size());

	// discretely integrating over time to compute net cumulative proper time aged by mover
	// we average out differences in frame durations by simply distributing net duration.
	// net duration (coordinate time) is divided evenly among the transformed-relative-aging-rates
	// mActorTimeVelocities contains the dTau/dT for each slice/observation.

	// thus, each successive cumulative time aged is:
	//		cumulativeProperTime[i-1] + dT * dTau/dT
    mCumulativeProperTime[0] = mDt * mActorProperTimes[0];
    for (size_t i = 1; i < mCumulativeProperTime.size(); ++i) {
        mCumulativeProperTime[i] = mCumulativeProperTime[i-1] + mDt * mActorProperTimes[i];
    }


}

float SpacetimeDisplay::GetCumulativeAgeDiff(size_t i) const {
    return mCumulativeProperTime[i];
}

float SpacetimeDisplay::GetCumulativeAgeDiff() const {
    return mCumulativeProperTime.back();
}
