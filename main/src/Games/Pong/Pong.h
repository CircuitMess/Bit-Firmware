#ifndef BIT_FIRMWARE_PONG_H
#define BIT_FIRMWARE_PONG_H

#include "GameEngine/Game.h"
#include "GameEngine/Rendering/TextRC.h"

class Pong : public Game {
public:
	Pong(Sprite& base);
protected:
	void onLoad() override;
	void onLoop(float deltaTime) override;
	void handleInput(const Input::Data& data) override;
	void onStop() override;
	uint32_t getXP() const override;

private:
	enum class State {
		PressToStart, Game, End
	} state = State::PressToStart;

	static constexpr uint8_t EndTime = 5;
	float endCounter = 0;

	GameObjPtr player;
	GameObjPtr enemy;
	GameObjPtr ball;
	GameObjPtr bg;

	std::shared_ptr<TextRC> scoreLabel;
	std::shared_ptr<TextRC> statusLabel;

	static constexpr PixelDim PlayerDim{ 6, 32 };
	static constexpr PixelDim EnemyDim{ 6, 32 };
	static constexpr PixelDim BallDim{ 11, 11 };
	static constexpr float MaxBounceAngleDeg = 65;
	static constexpr float MaxBounceAngleRad = MaxBounceAngleDeg * M_PI / 180.0;
	static constexpr uint8_t PaddleEdgeGap = 5; //gap from paddle to respective screen border
	static constexpr uint8_t ScoreLimit = 3; //First to 5 points
	uint8_t playerScore = 0, enemyScore = 0;
	float playerSpeed = 0, enemySpeed = 0;

	static constexpr float BallBaseSpeed = 120.0;
	static constexpr float PlayerBaseSpeed = 60.0;
	static constexpr float EnemyBaseSpeed = 60.0;
	glm::vec2 ballSpeed = {};

	void buildElements();
	void setCollision();
	void moveEnemy();

	void changeState(State newState);

	bool possibleComeback = false;
};


#endif //BIT_FIRMWARE_PONG_H
