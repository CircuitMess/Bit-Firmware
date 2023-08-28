#include "Player.h"
#include "Invaders.h"

Invaders::Player::Player(GameObjPtr playerObj, Invaders* game, File hitFile, File deadFile) :
		gamePtr(game), obj(playerObj), hitFile(hitFile), deadFile(deadFile){
	anim = std::static_pointer_cast<AnimRC>(obj->getRenderComponent());
	anim->setLoopMode(GIF::Single);
	anim->reset();
	anim->start();
	anim->stop();
}

void Invaders::Player::update(float delta){
	updateMovement(delta);
	updateState(delta);
}

void Invaders::Player::btnPressed(Input::Button btn){

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

void Invaders::Player::btnReleased(Input::Button btn){

	if(btn != Input::Left && btn != Input::Right) return;

	if(state == State::Death) return;

	if(btn == lastPressed){
		horizontalDirection = 0.0f;
		lastPressed = Input::Menu;
	}

}

void Invaders::Player::damage(){
	if(state == State::Death) return;

	anim->reset();
	anim->start();
	anim->setLoopMode(GIF::Infinite);
	state = State::Damaged;
}

void Invaders::Player::death(){
	if(state == State::Death) return;

	state = State::Death;
	horizontalDirection = 0;
	anim->setAnim(deadFile);
	anim->setLoopMode(GIF::Single);
	anim->reset();
	anim->start();
}

bool Invaders::Player::isDead() const{
	return state == State::Death;
}

void Invaders::Player::updateMovement(float delta){
	float xPos = delta * HorizontalSpeed * horizontalDirection;
	xPos += obj->getPos().x;
	xPos = std::clamp(xPos, (float) -Invaders::PlayerObjOffset.x, 128.f - Invaders::PlayerSize.x - Invaders::PlayerObjOffset.x - 1);

	obj->setPos(xPos, obj->getPos().y);
}

void Invaders::Player::updateState(float delta){
	switch(state){
		case State::Idle:
			break;
		case State::Damaged:
			invincibilityTime += delta;
			if(invincibilityTime >= InvincibilityDuration){
				invincibilityTime = 0;
				state = State::Idle;

				anim->setAnim(hitFile);
				anim->reset();
				anim->start();
				anim->stop();
			}
			break;
		case State::Death:
			break;
	}
}
