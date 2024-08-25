#ifndef BIT_FIRMWARE_HERTZGAME_H
#define BIT_FIRMWARE_HERTZGAME_H

#include "../../GameEngine/Game.h"
#include "../../GameEngine/Rendering/AnimRC.h"
#include "Indicator.h"
#include "ProgressBar.h"
#include "Bar.h"
#include "RoboCtrl/Hertz.h"

class HertzGame : public Game {
public:
	HertzGame(Sprite& canvas);

protected:
	void onLoad() override;
	void onLoop(float deltaTime) override;
	void onStart() override;
	void onStop() override;
	void handleInput(const Input::Data& data) override;
	uint32_t getXP() const override;

private:
	void resetAnim();
	void addPoints(int difference);

	std::shared_ptr<RoboCtrl::Hertz> robo;

	std::unique_ptr<Indicator> indicator;
	std::unique_ptr<ProgressBar> progressBar;
	std::unique_ptr<Bar> bar;

	float fillPercent = 0.0f;
	int tries = 0;
	const float length = 120.0f;
	const float maxPoints = 0.35f;
	const uint8_t MinimumAttempts = std::round(1.0f / maxPoints);

	float multiplier;
	bool done = false;
	float winCounter = 0;
	static constexpr float WinPause = 1.0f;

	std::shared_ptr<AnimRC> duckAnim;
	std::shared_ptr<GameObject> indicatorGO;
	std::shared_ptr<GameObject> progressGO;
	std::shared_ptr<GameObject> barGO;
	std::shared_ptr<GameObject> mainGO;
	std::shared_ptr<GameObject> bg;

	bool animResetQueued = false;

};

#endif //BIT_FIRMWARE_HERTZGAME_H
