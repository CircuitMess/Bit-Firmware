#include "Pong.h"
#include "GameEngine/Rendering/SpriteRC.h"
#include "GameEngine/Rendering/StaticRC.h"
#include "GameEngine/Collision/RectCC.h"
#include "GameEngine/Collision/CircleCC.h"
#include <esp_random.h>

Pong::Pong(Sprite& base) : Game(base, Games::Pong, "/Games/Pong", {
		{ "/bg.raw", {}, true }
}){

}

void Pong::onLoad(){
	buildElements();
	setCollision();
	changeState(State::PressToStart);
}

void Pong::onLoop(float deltaTime){
	switch(state){
		case State::PressToStart:
			player->setPos(player->getPos().x, std::clamp(player->getPos().y + playerSpeed * deltaTime, 0.f, 128.f - PlayerDim.y));
			break;
		case State::Game:
			moveEnemy();
			player->setPos(player->getPos().x, std::clamp(player->getPos().y + playerSpeed * deltaTime, 0.f, 128.f - PlayerDim.y));
			enemy->setPos(enemy->getPos().x, std::clamp(enemy->getPos().y + enemySpeed * deltaTime, 0.f, 128.f - EnemyDim.y));
			ball->setPos(ball->getPos() + ballSpeed * deltaTime);
			break;
		case State::End:
			endCounter += deltaTime;
			if(endCounter >= EndTime){
				exit();
			}
			break;
	}
}

void Pong::handleInput(const Input::Data& data){
	switch(state){
		case State::PressToStart:
			if(data.action == Input::Data::Press && data.btn == Input::A){
				changeState(State::Game);
			}
		case State::Game:
			if(data.action == Input::Data::Press && data.btn == Input::Up){
				playerSpeed = -PlayerBaseSpeed;
			}else if(data.action == Input::Data::Release && data.btn == Input::Up && playerSpeed == -PlayerBaseSpeed){
				playerSpeed = 0;
			}else if(data.action == Input::Data::Press && data.btn == Input::Down){
				playerSpeed = PlayerBaseSpeed;
			}else if(data.action == Input::Data::Release && data.btn == Input::Down && playerSpeed == PlayerBaseSpeed){
				playerSpeed = 0;
			}
			break;

		case State::End:
			if(data.action == Input::Data::Press){
				exit();
			}
			break;
	}
}

void Pong::onStop(){
	handleInput({ Input::Button::Up, Input::Data::Release });
	handleInput({ Input::Button::Down, Input::Data::Release });
}

uint32_t Pong::getXP() const{
	if(state != State::End) return 0;

	return (ScoreLimit - enemyScore) * 50;
}

void Pong::buildElements(){
	auto staticRC = std::make_unique<StaticRC>(getFile("/bg.raw"), PixelDim{ 128, 128 });
	staticRC->setLayer(-2);
	bg = std::make_shared<GameObject>(
			std::move(staticRC),
			nullptr
	);
	addObject(bg);


	auto spriteRC = std::make_unique<SpriteRC>(PlayerDim);
	spriteRC->getSprite()->clear(TFT_RED);
	player = std::make_shared<GameObject>(
			std::move(spriteRC),
			std::make_unique<RectCC>(PlayerDim)
	);
	player->setPos(PaddleEdgeGap, (128 - PlayerDim.y) / 2);
	addObject(player);

	spriteRC = std::make_unique<SpriteRC>(EnemyDim);
	spriteRC->getSprite()->clear(TFT_BLUE);
	enemy = std::make_shared<GameObject>(
			std::move(spriteRC),
			std::make_unique<RectCC>(EnemyDim)
	);
	enemy->setPos(128 - PaddleEdgeGap - EnemyDim.x, (128 - EnemyDim.y) / 2);
	addObject(enemy);

	spriteRC = std::make_unique<SpriteRC>(BallDim);
	spriteRC->getSprite()->clear(TFT_TRANSPARENT);
	spriteRC->getSprite()->fillCircle(5, 5, 5, TFT_WHITE);
	ball = std::make_shared<GameObject>(
			std::move(spriteRC),
			std::make_unique<CircleCC>(6, PixelDim{ 5, 5 })
	);
	ball->setPos(128 - BallDim.x - EnemyDim.x - 6, (128 - BallDim.y) / 2);
	addObject(ball);

	auto labelRC = std::make_unique<TextRC>("0 : 0", TextStyle{ &lgfx::fonts::Font0, TFT_WHITE, 2, top_center });
	auto score = std::make_shared<GameObject>(
			std::move(labelRC),
			nullptr
	);
	scoreLabel = std::static_pointer_cast<TextRC>(score->getRenderComponent());
	scoreLabel->setLayer(-1);
	score->setPos(64, 5);
	addObject(score);

	labelRC = std::make_unique<TextRC>("initial", TextStyle{ &lgfx::fonts::Font0, TFT_WHITE, 1, top_center });
	auto status = std::make_shared<GameObject>(
			std::move(labelRC),
			nullptr
	);
	statusLabel = std::static_pointer_cast<TextRC>(status->getRenderComponent());
	statusLabel->setLayer(-1);
	status->setPos(64, 25);
	addObject(status);


	ballSpeed.x = -BallBaseSpeed;
	ballSpeed.y = 0;
}

void Pong::setCollision(){
	collision.addPair(*player, *ball, [this](){
		auto p = ball->getPos();

		auto relativeIntersectY = (player->getPos().y + (PlayerDim.y / 2)) - (ball->getPos().y + BallDim.y / 2);
		auto normalizedRelativeIntersectionY = std::clamp((relativeIntersectY / (PlayerDim.y / 2)), -1.f, 1.f);
		auto bounceAngle = normalizedRelativeIntersectionY * MaxBounceAngleRad;

		ballSpeed.x = BallBaseSpeed * cos(bounceAngle);
		ballSpeed.y = BallBaseSpeed * -sin(bounceAngle);

		audio.play({ { 100, 400, 100 } });
	});

	collision.addPair(*enemy, *ball, [this](){
		auto p = ball->getPos();

		auto relativeIntersectY = (player->getPos().y + (EnemyDim.y / 2)) - (ball->getPos().y + BallDim.y / 2);
		auto normalizedRelativeIntersectionY = std::clamp((relativeIntersectY / (PlayerDim.y / 2)), -1.f, 1.f);
		auto bounceAngle = normalizedRelativeIntersectionY * MaxBounceAngleRad;
		ballSpeed.x = BallBaseSpeed * cos(bounceAngle);
		ballSpeed.y = BallBaseSpeed * -sin(bounceAngle);

		ballSpeed.x = -abs(ballSpeed.x);
		audio.play({ { 100, 300, 50 },
					 { 300, 100, 50 } });
	});

	collision.wallsHorizontal(*ball, [this](){
		auto p = ball->getPos();
		ball->setPos(p.x, std::clamp(p.y, 0.f, 128.f));
		ballSpeed.y *= -1;

		audio.play({ { 50, 150, 100 } });
	});

	collision.wallLeft(*ball, [this](){
		enemyScore++;

		if(enemyScore == ScoreLimit-1 && playerSpeed == 0){
			possibleComeback = true;
		}

		scoreLabel->setText(std::to_string(playerScore) + " : " + std::to_string(enemyScore));
		ballSpeed.x = abs(ballSpeed.x);
		ballSpeed.y = (esp_random() % 2 == 1) ? BallBaseSpeed : -BallBaseSpeed;

		if(enemyScore >= ScoreLimit || playerScore >= ScoreLimit){
			changeState(State::End);
		}else{
			int16_t posY = esp_random() % 100;
			posY += 14;
			ball->setPos(player->getPos().x + PlayerDim.x + 2, posY);
			ballSpeed.x = BallBaseSpeed;
			ballSpeed.y = 0;
			changeState(State::PressToStart);
		}

		audio.play({ { 400, 100, 200 },
					 { 0,   0,   80 },
					 { 150, 100, 300 } });
	});

	collision.wallRight(*ball, [this](){
		playerScore++;
		scoreLabel->setText(std::to_string(playerScore) + " : " + std::to_string(enemyScore));
		ballSpeed.x = -abs(ballSpeed.x);
		ballSpeed.y = (esp_random() % 2 == 1) ? BallBaseSpeed : -BallBaseSpeed;

		if(enemyScore >= ScoreLimit || playerScore >= ScoreLimit){
			changeState(State::End);
		}else{
			int16_t posY = esp_random() % 100;
			posY += 14;
			ball->setPos(enemy->getPos().x - BallDim.x - 2, posY);
			ballSpeed.x = -BallBaseSpeed;
			ballSpeed.y = 0;
			changeState(State::PressToStart);
		}

		audio.play({ { 200, 400, 100 },
					 { 0,   0,   80 },
					 { 200, 400, 100 },
					 { 0,   0,   80 },
					 { 200, 800, 200 } });
	});
}

void Pong::moveEnemy(){
	auto enemyY = enemy->getPos().y;
	const float enemyCenter = enemyY + (EnemyDim.y / 2);
	const float ballCenter = (ball->getPos().y + (BallDim.y / 2));

	if(abs(enemyCenter - ballCenter) < 0.9){
		enemySpeed = 0;
	}else{
		if(enemyCenter < ballCenter){ //if the ball is below the oponent
			enemySpeed = EnemyBaseSpeed;
		}else{
			enemySpeed = -EnemyBaseSpeed;
		}
	}
}

void Pong::changeState(Pong::State newState){
	state = newState;

	switch(newState){
		case State::PressToStart:
			statusLabel->setText("Press A to start!");
			break;
		case State::Game:
			statusLabel->setText("");

			break;
		case State::End:
			statusLabel->setText(std::string(playerScore > enemyScore ? "Player" : "Bit") + " wins!");
			endCounter = 0;

			if(playerScore > enemyScore){
				addAchi(Achievement::Bonk_5, 1);

				if(possibleComeback){
					addAchi(Achievement::Bonk_comeback, 1);
				}
			}else{
				resetAchi(Achievement::Bonk_5);
			}

			break;
	}
}
