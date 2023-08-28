#include "Player.h"
#include "CapacitronGame.h"
#include "GameEngine/Collision/RectCC.h"

CapacitronGame::Player::Player(GameObjPtr playerObj, GameObjPtr playerLegsObj, CapacitronGame* game, File jumpFile, File deadFile, File invincFile) :
		gamePtr(game), obj(playerObj), legsObj(playerLegsObj), jumpFile(jumpFile), deadFile(deadFile), invincFile(invincFile){
	anim = std::static_pointer_cast<AnimRC>(obj->getRenderComponent());
	anim->setLoopMode(GIF::Single);
	anim->start();
}

void CapacitronGame::Player::btnPressed(Input::Button btn){
	if(btn != Input::Left && btn != Input::Right) return;

	if(state == State::Death) return;
	lastPressed = btn;

	if(btn == Input::Left){
		anim->setFlipY(false);
		horizontalDirection = -1.0f;
	}else if(btn == Input::Right){
		horizontalDirection = 1.0f;
		anim->setFlipY(true);
	}
}

void CapacitronGame::Player::btnReleased(Input::Button btn){
	if(btn != Input::Left && btn != Input::Right) return;

	if(state == State::Death) return;

	if(btn == lastPressed){
		horizontalDirection = 0.0f;
		lastPressed = Input::Menu;
	}
}

void CapacitronGame::Player::damage(){
	if(state == State::Death) return;

	state = State::Damaged;
}

void CapacitronGame::Player::fallDown(){
	if(state == State::Death) return;

	trampolineJump();
	state = State::Damaged;
}

void CapacitronGame::Player::death(){
	if(state == State::Death) return;

	state = State::Death;
	ySpeed = CapacitronGame::JumpSpeed;
	horizontalDirection = 0;
	anim->setAnim(deadFile);
	anim->setLoopMode(GIF::Single);
	anim->reset();
	anim->start();
}

void CapacitronGame::Player::invincible(){
	if(state == State::Death) return;

	if(state == State::Invincibility){
		invincibilityTime = 0;
		return;
	}

	state = State::Invincibility;
	anim->setAnim(invincFile);
	anim->setLoopMode(GIF::Single);
	anim->start();
}

void CapacitronGame::Player::jump(){
	if(state == State::Death) return;

	gamePtr->audio.play({ { 80, 80, 80 } });
	ySpeed = CapacitronGame::JumpSpeed;
	anim->reset();
	anim->start();
}

void CapacitronGame::Player::trampolineJump(){
	if(state == State::Death) return;

	ySpeed = CapacitronGame::TrampolineSpeed;
	anim->reset();
	anim->start();
}

float CapacitronGame::Player::getYSpeed() const{
	return ySpeed;
}

bool CapacitronGame::Player::isInvincible() const{
	return state == State::Damaged || state == State::Invincibility;
}

float CapacitronGame::Player::update(float delta){

	updateMovement(delta);
	updateState(delta);

	return delta * ySpeed;
}

void CapacitronGame::Player::updateMovement(float delta){
	float xPos = delta * HorizontalSpeed * horizontalDirection;
	xPos += obj->getPos().x;
	xPos = std::clamp(xPos, 8.f, 128.f - CapacitronGame::PlayerSize.x - 8);

	float yPos = obj->getPos().y;
	ySpeed += delta * CapacitronGame::CapacitronGame::Gravity;
	yPos += delta * ySpeed;

	obj->setPos(xPos, yPos);

	legsObj->setPos(xPos, yPos + CapacitronGame::PlayerSize.y - 3);
}

void CapacitronGame::Player::updateState(float delta){
	switch(state){
		case State::Jumping:
			if(!anim->isVisible()){
				anim->setVisible(true);
			}
			break;
		case State::Damaged:
			invincibilityTime += delta;
			if((int) (invincibilityTime / InvincibilityBlinkDuration) % 2 == 0){
				anim->setVisible(false);
			}else{
				anim->setVisible(true);
			}

			if(invincibilityTime >= InvincibilityDuration){
				invincibilityTime = 0;
				state = State::Jumping;
				anim->setVisible(true);
			}
			break;
		case State::Death:
			if(!anim->isVisible()){
				anim->setVisible(true);
			}
			break;
		case State::Invincibility:
			if(!anim->isVisible()){
				anim->setVisible(true);
			}
			invincibilityTime += delta;

			if(invincibilityTime >= PotionDuration){
				invincibilityTime = 0;
				state = State::Jumping;
				anim->setAnim(jumpFile);
				anim->start();
				anim->stop();
			}
			break;
	}
}

bool CapacitronGame::Player::isDead() const{
	return state == State::Death;
}
