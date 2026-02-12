#include "Game.h"

#include <iostream>

#include "Actor.h"
#include "Paddle.h"
#include <functional>
#include <string>
Game gGame;

Game::Game()
{
	// comments in class declaration explain these members

	mSdlWindow = nullptr;
	mSdlRenderer = nullptr;

	mContinueRunning = true;

	// actors start null
	mPaddle = nullptr;

	mPreviousTime = 0;

	mActorPositions.reserve(static_cast<unsigned>(WINDOW_HEIGHT / PLOT_POINT_SIZE));
	mActorVelocities.reserve(static_cast<unsigned>(WINDOW_HEIGHT / PLOT_POINT_SIZE));
}

bool Game::Initialize()
{
	SDL_SetHint("SDL_MAIN_CALLBACK_RATE", "60");
	bool sdlInit = SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);
	// fail if SDL init doesn't work
	if (!sdlInit)
	{
		return false;
	}
	// use window constants
	mSdlWindow = SDL_CreateWindow("PONG", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	if (mSdlWindow == nullptr)
	{
		return false;
	}

	mSdlRenderer = SDL_CreateRenderer(mSdlWindow, nullptr);
	if (mSdlRenderer == nullptr)
	{
		return false;
	}

	// init actors!
	LoadData();
	return true;
}

bool Game::RunIteration()
{
	// game loop!

	// get input
	ProcessInput();
	// update game objects
	UpdateGame();
	// render
	GenerateOutput();
	return mContinueRunning;
}

void Game::Shutdown()
{
	// call proper unloaders/destroyers
	UnloadData();
	SDL_DestroyRenderer(mSdlRenderer);
	SDL_DestroyWindow(mSdlWindow);
	SDL_Quit();
}

void Game::HandleEvent(const SDL_Event* event)
{
	// x button
	if (event->type == SDL_EVENT_QUIT)
	{
		mContinueRunning = false;
	}
}

void Game::ProcessInput()
{
	const bool* keyboardState = SDL_GetKeyboardState(nullptr);
	// esc key can also end game
	if (keyboardState[SDL_SCANCODE_ESCAPE])
	{
		mContinueRunning = false;
	}

	// mouse
	float x = 0.0f;
	float y = 0.0f;
	SDL_MouseButtonFlags mouseButtons = SDL_GetMouseState(&x, &y);
	mMousePos = {x, y};

	for (auto a : mActors) {
		a->HandleInput(keyboardState, mouseButtons, mMousePos);
	}
}

void Game::TransformPoints(const std::vector<float>& src, std::vector<float>& dest, const std::function<float(float)>& transformFunc, bool time) {
	for (size_t i = 0; i + mFrameAgg <= src.size(); i += mFrameAgg) {
		float sum = 0.0f;
		for (size_t x = 0; x < mFrameAgg; ++x) {
			sum += src[i + x];
		}
		// now these are transformed for rendering
		dest.push_back((transformFunc(sum/(mFrameAgg * 1.0f)) + time * WINDOW_WIDTH/2.0f));
	}
}

void Game::DrawText(float x, float y, const char* fmt, float value, float scale) {
	SDL_SetRenderDrawColor(mSdlRenderer, MAX_COLOR,MAX_COLOR,MAX_COLOR,MAX_COLOR);
	if (scale != 1.0f) { SDL_SetRenderScale(mSdlRenderer, scale, scale); }
	SDL_RenderDebugTextFormat(mSdlRenderer, x / scale, y / scale, fmt, value);
	if (scale != 1.0f) { SDL_SetRenderScale(mSdlRenderer, 1.0f, 1.0f); }
}

void Game::DrawText(float x, float y, const char* fmt, int value, float scale) {
	SDL_SetRenderDrawColor(mSdlRenderer, MAX_COLOR,MAX_COLOR,MAX_COLOR,MAX_COLOR);
	if (scale != 1.0f) { SDL_SetRenderScale(mSdlRenderer, scale, scale); }
	SDL_RenderDebugTextFormat(mSdlRenderer, x / scale, y / scale, fmt, value);
	if (scale != 1.0f) { SDL_SetRenderScale(mSdlRenderer, 1.0f, 1.0f); }
}

void Game::DrawPaddleText() {
	if (mPaddle == nullptr){return;}
	// now we type in the score onto the paddle
	Vector2 paddleRect = mPaddle->GetTransform().GetPosition();
	SDL_SetRenderDrawColor(mSdlRenderer, MAX_COLOR, MAX_COLOR, MAX_COLOR, MAX_COLOR);
	
	// time remaining
	DrawText(paddleRect.x - HALF_CHAR_PIXELS, paddleRect.y - 10.0f / HALF_CHAR_PIXELS, "%i", static_cast<int>(DURATION_SECONDS - mDT), 1.35f);		  

	// pos + velo scaled 2x
	float posY = paddleRect.y - 10.0f / HALF_CHAR_PIXELS - mPaddle->GetTransform().GetSize().y * 2;
	float veloY = paddleRect.y - 10.0f / HALF_CHAR_PIXELS - mPaddle->GetTransform().GetSize().y;
	DrawText(paddleRect.x - CHAR_PIXELS*2, posY, "%.0f", mPaddle->GetTransform().GetPosition().x, 2.0f);
	DrawText(paddleRect.x - CHAR_PIXELS*2, veloY, "%.0f", mPaddle->GetVelocity(), 2.0f);
}

void Game::DrawSpacetime() {
	float midX = WINDOW_WIDTH / 2.0f;
	// plot as many positions fit in the window (we'll accept overlap)
	float yStep = mActorPositions.empty() ? PLOT_POINT_SIZE
	              : WINDOW_HEIGHT / static_cast<float>(mActorPositions.size());
	float y = WINDOW_HEIGHT;

	// Y (time) axis
	SDL_SetRenderDrawColor(mSdlRenderer, MAX_COLOR,MAX_COLOR,MAX_COLOR,MAX_COLOR);
	for (size_t i = 0; i < WINDOW_HEIGHT; ++i) {
		SDL_FRect rect(midX - 1.0f, i - 1.0f, 2.0f, 2.0f);
		SDL_RenderFillRect(mSdlRenderer, &rect);
	}

	// how many time dilation prints do we want
	auto numTimePrints = 18.0f;
	auto timePrintInterval = static_cast<unsigned>(mActorPositions.size() / numTimePrints);

	for (size_t i = 0; i < mActorPositions.size(); ++i) {
		// pos, velo, timeDil for plotting
		float mSpacetimePos = mActorPositions[i];
		float mSpacetimeVelo = mActorVelocities[i];
		float veloPct = Math::Abs(mSpacetimeVelo - midX) / (midX * 0.5f);

		// for printing dots and text
		bool neg = mSpacetimeVelo < midX;
		unsigned r = neg * 255;
		unsigned g = !neg * 255;
		unsigned alpha = static_cast<unsigned>(255 * veloPct);

		// pos - white
		SDL_SetRenderDrawColor(mSdlRenderer, MAX_COLOR, MAX_COLOR, MAX_COLOR, MAX_COLOR);
		SDL_FRect rect(mSpacetimePos - PLOT_POINT_SIZE/2.0f, y - PLOT_POINT_SIZE/2.0f, PLOT_POINT_SIZE, PLOT_POINT_SIZE);
		SDL_RenderFillRect(mSdlRenderer, &rect);

		if (i % timePrintInterval == 0 && i != 0){
			bool posNeg = mSpacetimePos < midX;
			int shift = ((!posNeg) -posNeg) * (30 * PLOT_POINT_SIZE);
			DrawText(mSpacetimePos - PLOT_POINT_SIZE/2.0f - HALF_CHAR_PIXELS - shift,
							  y - PLOT_POINT_SIZE/2.0f, "%.2f%%", mActorTimeVelocities[i], 1.35f);
		}

		// velo - color is velo gauged
		SDL_SetRenderDrawBlendMode(mSdlRenderer, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(mSdlRenderer, r, g, 0, alpha);
		SDL_FRect rect2(mSpacetimeVelo - PLOT_POINT_SIZE/2.0f, y - PLOT_POINT_SIZE/2.0f, PLOT_POINT_SIZE, PLOT_POINT_SIZE);
		SDL_RenderFillRect(mSdlRenderer, &rect2);
		SDL_SetRenderDrawBlendMode(mSdlRenderer, SDL_BLENDMODE_NONE);

		y -= yStep;
	}

	// mouse tooltip — index from mouse Y, always stack downward with consistent order
	auto index = static_cast<size_t>(std::clamp((WINDOW_HEIGHT - mMousePos.y) / yStep, 0.0f, static_cast<float>(mActorPositions.size() - 1)));
	float mPos = mActorPositions[index];
	float mVelo = mActorVelocities[index];
	float mTime = mActorTimeVelocities[index];

	// flip text to whichever side has more room
	float textW = CHAR_PIXELS * 10;
	float textH = CHAR_PIXELS * 3 + 4.0f;
	float tx = mMousePos.x < midX ? mMousePos.x + CHAR_PIXELS : mMousePos.x - textW - CHAR_PIXELS;
	float ty = mMousePos.y < WINDOW_HEIGHT / 2.0f ? mMousePos.y + CHAR_PIXELS : mMousePos.y - textH - CHAR_PIXELS;

	// outlined box
	float pad = 3.0f;
	SDL_SetRenderDrawColor(mSdlRenderer, MAX_COLOR, MAX_COLOR, MAX_COLOR, MAX_COLOR);
	SDL_FRect box(tx - pad, ty - pad, textW + pad * 2, textH + pad * 2);
	SDL_RenderRect(mSdlRenderer, &box);

	DrawText(tx, ty, "%.2f%%", mTime);
	DrawText(tx, ty + CHAR_PIXELS + 2.0f, "%.0f", mVelo);
	DrawText(tx, ty + (CHAR_PIXELS + 2.0f) * 2, "%.0f", mPos);
}

void Game::EndGame() {
	unsigned numSamples = mPaddle->GetPositions().size();
	unsigned maxPlotPoints = static_cast<unsigned>(WINDOW_HEIGHT / PLOT_POINT_SIZE);
	mFrameAgg = std::max(1u, (numSamples + maxPlotPoints - 1) / maxPlotPoints);

	TransformPoints(mPaddle->GetPositions(), mActorPositions, mTransformPosition);
	TransformPoints(mPaddle->GetVelocities(), mActorVelocities, mTransformVelocity);
	TransformPoints(mPaddle->GetTimeVelocities(), mActorTimeVelocities, mTransformTimeVelocity, false);

	UnloadData();
	mPaddle = nullptr;
}

void Game::UpdateGame()
{
	// calculate deltatime
	Uint64 currTimeMs = SDL_GetTicks();
	Uint64 uIntDiff = currTimeMs - mPreviousTime;
	mPreviousTime = currTimeMs;
	float deltaTime = uIntDiff / MS_PER_SEC;
	deltaTime = Math::Min(MAX_DELTA_TIME, deltaTime);
	mDT += deltaTime;

	if (mDT >= DURATION_SECONDS && !mGameDone) {
		mGameDone = true;
		return EndGame();
	}

	for (auto a : mActors) {
		a->HandleUpdate(deltaTime);
	}
}

void Game::GenerateOutput()
{
	SDL_SetRenderDrawColor(mSdlRenderer, 0, 0, 0, MAX_COLOR);
	SDL_RenderClear(mSdlRenderer);

	float hh = WINDOW_HEIGHT/2.0f;
	SDL_SetRenderDrawColor(mSdlRenderer, MAX_COLOR,MAX_COLOR,MAX_COLOR,MAX_COLOR);
	for (size_t i = 0; i < WINDOW_WIDTH; ++i) {
		SDL_FRect rect(i - 1.0f, hh - 1.0f, 2.0f, 2.0f);
		SDL_RenderFillRect(mSdlRenderer, &rect);
	}

	// now set to blue
	SDL_SetRenderDrawColor(mSdlRenderer, 0, 0, MAX_COLOR, MAX_COLOR);
	// render each actor
	for (auto& actor : mActors)
	{
		SDL_FRect rect = actor->GetTransform().GetRect();
		SDL_RenderFillRect(mSdlRenderer, &rect);
	}

	if (mGameDone) {
		DrawSpacetime();
	}
	else	{

		DrawPaddleText();
	}

	SDL_RenderPresent(mSdlRenderer);
}

void Game::LoadData()
{
	mPaddle = CreateActor<Paddle>();
	mPaddle->GetTransform().SetPosition(Vector2(WINDOW_WIDTH / HALF_DIVISOR, WINDOW_HEIGHT / HALF_DIVISOR ));
}

void Game::UnloadData()
{
	// delete objects and clear vector
	for (auto& actor : mActors)
	{
		delete actor;
	}
	mActors.clear();
}

