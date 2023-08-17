#include "Pong.h"
#include "GameEngine/Rendering/SpriteRC.h"
#include "GameEngine/Collision/RectCC.h"
#include "GameEngine/Collision/CircleCC.h"

Pong::Pong(Sprite& base) : Game(base, "/Games/Pong", {/* TODO - dodat bg */}){

}

void Pong::onLoad(){
	buildElements();
	setCollision();
	changeState(State::PressToStart);
}

void Pong::onLoop(float deltaTime){
	switch(state){
		case State::PressToStart:
			player->setPos(player->getPos() + glm::vec2{ 0, playerSpeed } * deltaTime);
			break;
		case State::Game:
			moveEnemy();
			player->setPos(player->getPos() + glm::vec2{ 0, playerSpeed } * deltaTime);
			if(player->getPos().y > 128 - playerDim.y || player->getPos().y < 0){
				auto pos = player->getPos();
				pos.y = std::clamp(pos.y, 0.f, 128.f - playerDim.y);
				player->setPos(pos);
			}
			enemy->setPos(enemy->getPos() + glm::vec2{ 0, enemySpeed } * deltaTime);
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

void Pong::buildElements(){
	auto spriteRC = std::make_unique<SpriteRC>(PixelDim{ 128, 128 });
	spriteRC->getSprite()->clear(TFT_BLACK);
	spriteRC->setLayer(-2);
	bg = std::make_shared<GameObject>(
			std::move(spriteRC),
			nullptr
	);
	addObject(bg);


	spriteRC = std::make_unique<SpriteRC>(playerDim);
	spriteRC->getSprite()->clear(TFT_RED);
	player = std::make_shared<GameObject>(
			std::move(spriteRC),
			std::make_unique<RectCC>(playerDim)
	);
	player->setPos(PaddleEdgeGap, (128 - playerDim.y) / 2);
	addObject(player);

	spriteRC = std::make_unique<SpriteRC>(enemyDim);
	spriteRC->getSprite()->clear(TFT_BLUE);
	enemy = std::make_shared<GameObject>(
			std::move(spriteRC),
			std::make_unique<RectCC>(enemyDim)
	);
	enemy->setPos(128 - PaddleEdgeGap - enemyDim.x, (128 - enemyDim.y) / 2);
	addObject(enemy);

	spriteRC = std::make_unique<SpriteRC>(ballDim);
	spriteRC->getSprite()->clear(TFT_TRANSPARENT);
	spriteRC->getSprite()->fillCircle(5, 5, 5, TFT_WHITE);
	ball = std::make_shared<GameObject>(
			std::move(spriteRC),
			std::make_unique<CircleCC>(6, PixelDim{ 5, 5 })
	);
	ball->setPos(128 - ballDim.x - enemyDim.x - 2, (128 - ballDim.y) / 2);
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


	ballSpeed.x = BallBaseSpeed;
	ballSpeed.y = (rand() % 2 == 1) ? BallBaseSpeed : -BallBaseSpeed;
}

void Pong::setCollision(){
	collision.addPair(*player, *ball, [this](){
		auto p = ball->getPos();
		ball->setPos(player->getPos().x + playerDim.x, p.y);
		ballSpeed.x *= -1;
		audio.play({ { 100, 100, 50 } });
	});

	collision.addPair(*enemy, *ball, [this](){
		auto p = ball->getPos();
		ball->setPos(enemy->getPos().x - ballDim.x, p.y);
		ballSpeed.x *= -1;
		audio.play({ { 150, 150, 50 } });
	});

	collision.wallsHorizontal(*ball, [this](){
		auto p = ball->getPos();
		ball->setPos(p.x, std::clamp(p.y, 0.f, 128.f));
		ballSpeed.y *= -1;
	});

	collision.wallLeft(*ball, [this](){
		enemyScore++;
		scoreLabel->setText(std::to_string(playerScore) + " : " + std::to_string(enemyScore));
		ballSpeed.x = abs(ballSpeed.x);
		ballSpeed.y = (rand() % 2 == 1) ? BallBaseSpeed : -BallBaseSpeed;

		if(enemyScore >= ScoreLimit || playerScore >= ScoreLimit){
			changeState(State::End);
		}else{
			ball->setPos(player->getPos().x + playerDim.x + 2, rand() % (128 - ballDim.y + 1));
			changeState(State::PressToStart);
		}
	});

	collision.wallRight(*ball, [this](){
		playerScore++;
		scoreLabel->setText(std::to_string(playerScore) + " : " + std::to_string(enemyScore));
		ballSpeed.x = -abs(ballSpeed.x);
		ballSpeed.y = (rand() % 2 == 1) ? BallBaseSpeed : -BallBaseSpeed;

		if(enemyScore >= ScoreLimit || playerScore >= ScoreLimit){
			changeState(State::End);
		}else{
			ball->setPos(enemy->getPos().x - ballDim.x - 2, rand() % (128 - ballDim.y + 1));
			changeState(State::PressToStart);
		}
	});
}

void Pong::moveEnemy(){
	auto enemyY = enemy->getPos().y;
	if((enemyY + (enemyDim.y / 2)) < (ball->getPos().y + (ballDim.y / 2))){ //if the ball is below the oponent
		enemySpeed = EnemyBaseSpeed;
	}else{
		enemySpeed = -EnemyBaseSpeed;
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
			statusLabel->setText(std::string(playerScore > enemyScore ? "Player" : "CPU") + " wins!");
			endCounter = 0;
			break;
	}
}
