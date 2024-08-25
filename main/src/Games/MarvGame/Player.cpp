#include "Player.h"
#include "MarvGame.h"

MarvGame::Player::Player(std::shared_ptr<GameObject> duckGoRc, std::shared_ptr<GameObject> duckGoCc, MarvGame* game4) : gamePtr(game4), gameObjectRc(duckGoRc),
																														gameObjectCc(duckGoCc){
	animRc = std::static_pointer_cast<AnimRC>(gameObjectRc->getRenderComponent());
	animRc->start();
}

MarvGame::Player::~Player(){
	animRc->stop();
}

void MarvGame::Player::update(float deltaTime){
	if(goToAnim){
		animRc->setAnim(goToAnim);
		animRc->setLoopMode(GIF::Infinite);
		goToAnim = File();
	}

	updateInvincibility(deltaTime);
	if(winFlag){
		float x = gameObjectRc->getPos().x;
		x += 30.0f * deltaTime;
		gameObjectRc->setPos({ x, gameObjectRc->getPos().y });
		if(x >= 63){
			walk();
			winFlag = false;
			gameObjectRc->setPos({ 63, startPosY - 23 });
			animRc->setAnim(winAnim);
			animRc->setLoopMode(GIF::Single);
			animRc->setLoopDoneCallback([this](uint32_t){
				gamePtr->endPauseState = MarvGame::Win;
			});
		}
	}

	if(isJumping){
		float y = gameObjectRc->getPos().y;
		y += velocity * deltaTime + 0.5f * gravity * pow(deltaTime, 2);
		velocity += gravity * deltaTime * multiplier;

		if(time < peakTime && !isDead){
			time += deltaTime;
		}else{
			multiplier = 5;
		}

		gameObjectRc->setPos({ 5, y });
		gameObjectCc->setPos({ 5, y });
		if(y > startPosY){
			gameObjectRc->setPos({ 5, startPosY });
			gameObjectCc->setPos({ 5, startPosY });

			multiplier = 1.0f;
			isJumping = false;
			time = 0.0f;
			if(isDead){
				if(goToAnim){
					animRc->setAnim(goToAnim);
					animRc->setLoopMode(GIF::Infinite);
					goToAnim = File();
				}
				return;
			}
			if(duckHold){
				duck();
			}else{

				animRc->setAnim(walking);
				animRc->setLoopMode(GIF::Infinite);
			}
		}
	}

	if(goToAnim){
		animRc->setAnim(goToAnim);
		animRc->setLoopMode(GIF::Infinite);
		goToAnim = File();
	}
}

void MarvGame::Player::walk(){
	if(!isDucked) return;
	isDucked = false;

	animRc->setAnim(unDucking);
	animRc->setLoopMode(GIF::Single);
	animRc->setLoopDoneCallback([this](uint32_t){
		goToAnim = walking;
	});
	gameObjectCc->setRot(0.0f);
}

void MarvGame::Player::jump(){
	if(isJumping) return;

	gamePtr->audio.play({ { 200, 200, 50 },
						  { 200, 800, 200 } });
	isJumping = true;
	velocity = 2 * maxHeight * gamePtr->getSpeed() / posXForMaxHeight;
	gravity = -2 * maxHeight * pow(gamePtr->getSpeed(), 2) / pow(posXForMaxHeight, 2);
	peakTime = posXForMaxHeight / gamePtr->getSpeed();
	walk();
	animRc->setAnim(jumpAnim);
	animRc->setLoopMode(GIF::Single);
}

void MarvGame::Player::duck(){
	if(isDucked) return;
	isDucked = true;

	animRc->setAnim(ducking);
	animRc->setLoopMode(GIF::Single);
	animRc->setLoopDoneCallback([this](uint32_t){
		goToAnim = ducked;
	});
	gameObjectCc->setRot(-90.0f);
}

void MarvGame::Player::death(){
	gamePtr->endPauseState = MarvGame::Lose;
	isDead = true;
	animRc->setAnim(down);
	animRc->setLoopMode(GIF::Single);
	animRc->setLoopDoneCallback([this](uint32_t){
		animRc->stop();
	});
}

std::shared_ptr<GameObject> MarvGame::Player::getGameObjectCc(){
	return gameObjectCc;
}

void MarvGame::Player::setFiles(File walk, File down, File jump, File ducking, File ducked, File unDucking, File winAnim){
	this->walking = walk;
	this->down = down;
	this->jumpAnim = jump;
	this->ducking = ducking;
	this->ducked = ducked;
	this->unDucking = unDucking;
	this->winAnim = winAnim;
}

void MarvGame::Player::win(){
	winFlag = true;
	if(isDucked){
		walk();
	}
}

void MarvGame::Player::duckPressed(){
	duckHold = true;
	gamePtr->audio.play({ { 400, 500, 80 },
						  { 500, 300, 150 } });
	if(isJumping){
		multiplier = 10.0f;
	}else{
		duck();
	}
}

void MarvGame::Player::jumpPressed(){
	jump();

}

void MarvGame::Player::duckReleased(){
	duckHold = false;
	if(isJumping) return;
	walk();
}

void MarvGame::Player::updateInvincibility(float delta){
	//TODO - play hit animation instead of blinking
	if(!invincible) return;

	invincibilityTime += delta;

	if((int) (invincibilityTime / invincibilityBlinkDuration) % 2 == 0){
		gameObjectRc->getRenderComponent()->setVisible(false);
	}else{
		gameObjectRc->getRenderComponent()->setVisible(true);
	}

	if(invincibilityTime >= invincibilityDuration){
		invincibilityTime = 0;
		invincible = false;
		gameObjectRc->getRenderComponent()->setVisible(true);
	}
}


