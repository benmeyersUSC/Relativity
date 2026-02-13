//
// Created by Ben Meyers on 2/12/26.
//

#include "SpacetimePlot.h"


#include "Game.h"
#include "Image.h"
void SpacetimePlot::DrawYAxis() {
	// Y (time) axis
	SDL_SetRenderDrawColor(gGame.GetRenderer(), Game::MAX_COLOR,Game::MAX_COLOR,Game::MAX_COLOR,Game::MAX_COLOR);
	for (size_t i = 0; i < Game::WINDOW_HEIGHT; ++i) {
		SDL_FRect rect(Game::HALF_WIDTH - 1.0f, i - 1.0f, 2.0f, 2.0f);
		SDL_RenderFillRect(gGame.GetRenderer(), &rect);
	}
}

void SpacetimePlot::DrawPoints(float yStep, unsigned timePrintInterval) {
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
		SDL_SetRenderDrawColor(gGame.GetRenderer(), Game::MAX_COLOR, Game::MAX_COLOR, Game::MAX_COLOR, Game::MAX_COLOR);
		SDL_FRect rect(mSpacetimePos - Game::HALF_PLOT_POINT, y - Game::HALF_PLOT_POINT, Game::PLOT_POINT_SIZE, Game::PLOT_POINT_SIZE);
		SDL_RenderFillRect(gGame.GetRenderer(), &rect);

		// print the time dilation at various times
		if (i % timePrintInterval == 0 && i != 0){
			bool negativePosition = mSpacetimePos < Game::HALF_WIDTH;
			float shiftMag = 30.0f;
			float shift = static_cast<float>(negativePosition - !negativePosition) * (shiftMag * Game::PLOT_POINT_SIZE);
			gGame.DrawFloat(mSpacetimePos - Game::HALF_PLOT_POINT - Game::HALF_CHAR_PIXELS + shift,
							  y - Game::HALF_PLOT_POINT, "%.2f%%", mActorTimeVelocities[i], 1.35f);
		}

		// velocity-gauged velo plotting
		// - color is direction-gauged
		// - transparency is magnitude-gauged
		SDL_SetRenderDrawBlendMode(gGame.GetRenderer(), SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(gGame.GetRenderer(), r, g, 0, alpha);
		SDL_FRect rect2(mSpacetimeVelo - Game::HALF_PLOT_POINT, y - Game::HALF_PLOT_POINT, Game::PLOT_POINT_SIZE, Game::PLOT_POINT_SIZE);
		SDL_RenderFillRect(gGame.GetRenderer(), &rect2);
		SDL_SetRenderDrawBlendMode(gGame.GetRenderer(), SDL_BLENDMODE_NONE);

		y -= yStep;
	}
}

void SpacetimePlot::DrawMouse(float yStep) {
	// mouse-height(time)-gauged information
	// index is inverse height (sdl convention), quantized by datapoints, clamped at 0 and max-index
	auto mouseHeightIndex = static_cast<size_t>(std::clamp((Game::WINDOW_HEIGHT - gGame.GetMousePos().y) / yStep, 0.0f, static_cast<float>(mActorPositions.size() - 1)));
	float mouseTime = mActorTimeVelocities[mouseHeightIndex];
	// de-transforming values so centered in middle of plot
	float mousePos = mActorPositions[mouseHeightIndex] - Game::HALF_WIDTH;
	float mouseVelo = mActorVelocities[mouseHeightIndex] - Game::HALF_WIDTH;

	// total magic here--fix
	float textW = Game::CHAR_PIXELS * 27;
	float textH = Game::CHAR_PIXELS * 3 + 4.0f;
	// always put mouse 'inward' (x and y)
	float mouseTextX = gGame.GetMousePos().x < Game::HALF_WIDTH ? gGame.GetMousePos().x + Game::CHAR_PIXELS : gGame.GetMousePos().x - textW - Game::CHAR_PIXELS;
	float mouseTextY = gGame.GetMousePos().y < Game::HALF_HEIGHT ? gGame.GetMousePos().y + Game::CHAR_PIXELS : gGame.GetMousePos().y - textH - Game::CHAR_PIXELS;

	// outlined box
	float pad = 12.0f;
	SDL_SetRenderDrawColor(gGame.GetRenderer(), Game::MAX_COLOR, Game::MAX_COLOR, Game::MAX_COLOR, Game::MAX_COLOR);
	SDL_FRect box(mouseTextX - pad, mouseTextY - pad, textW + pad * 3, textH + pad * 2);
	SDL_RenderRect(gGame.GetRenderer(), &box);

	// draw text
	float lineStep = Game::CHAR_PIXELS + 4.0f;
	gGame.DrawFloat(mouseTextX, mouseTextY, "Space Position: %.1f", mousePos);
	gGame.DrawFloat(mouseTextX, mouseTextY + lineStep, "Space Velocity: %.1f", mouseVelo);
	gGame.DrawFloat(mouseTextX, mouseTextY + lineStep * 2, "Aging Factor: %.1f%%", mouseTime);

	auto rotationSign = mouseVelo >= 0.0f ? 1.0f : -1.0f;
	mArrow->GetTransform().SetRotation(rotationSign * Math::ToDegrees(Math::Acos(mouseTime/100.0f)));
	mArrow->GetTransform().SetPosition({mouseTextX + textW, mouseTextY + lineStep});
	mArrow->Draw(gGame.GetRenderer());
}

void SpacetimePlot::Draw() {

	// how many time dilation prints do we want
	auto timePrintInterval = static_cast<unsigned>(static_cast<float>(mActorPositions.size()) / NUM_TIME_PRINTS);
	// plot as many positions fit in the window (we'll accept overlap)
	float yStep = Game::WINDOW_HEIGHT / static_cast<float>(mActorPositions.size());

	// Y (time) axis
	DrawYAxis();
	DrawPoints(yStep, timePrintInterval);
	DrawMouse(yStep);
}

SpacetimePlot::SpacetimePlot() {
	auto samples = static_cast<unsigned>(Game::WINDOW_HEIGHT / Game::PLOT_POINT_SIZE);
	mActorPositions.reserve(samples);
	mActorVelocities.reserve(samples);
	mActorTimeVelocities.reserve(samples);

	mArrow = std::make_unique<Image>();
	mArrow->GetTransform().SetScale(0.1f);
	mArrow->SetTexture(gGame.GetTexture(ARROW_FILE));
}
