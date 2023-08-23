#include "Player.h"
#include "CapacitronGame.h"
#include "GameEngine/Collision/RectCC.h"

CapacitronGame::Player::Player(GameObjPtr playerObj, GameObjPtr playerLegsObj, CapacitronGame* game) : gamePtr(game), obj(playerObj), legsObj(playerLegsObj){
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

}

void CapacitronGame::Player::fallDown(){

}

void CapacitronGame::Player::death(){

}

void CapacitronGame::Player::invincible(){

}

void CapacitronGame::Player::jump(){
	gamePtr->audio.play({ { 80, 80, 80 } });
	ySpeed = CapacitronGame::JumpSpeed;
	anim->reset();
}

void CapacitronGame::Player::trampolineJump(){

}

float CapacitronGame::Player::update(float delta){
	float xPos = delta * HorizontalSpeed * horizontalDirection;
	xPos += obj->getPos().x;
	xPos = std::clamp(xPos, 8.f, 128.f - CapacitronGame::PlayerSize.x - 8);

	float yPos = obj->getPos().y;
	ySpeed += delta * CapacitronGame::CapacitronGame::Gravity;
	yPos += delta * ySpeed;

	obj->setPos(xPos, yPos);

	legsObj->setPos(xPos, yPos + CapacitronGame::PlayerSize.y);

	return delta * ySpeed;
}

float CapacitronGame::Player::getYSpeed() const{
	return ySpeed;
}
