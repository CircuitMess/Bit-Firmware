#include "Player.h"

void Asteroids::Player::leftTurn(float delta){
	angle -= delta * rotSpeed;
	angle = fmod(angle, 360.0f);

	obj->setRot(angle);
}

void Asteroids::Player::rightTurn(float delta){
	angle += delta * rotSpeed;
	angle = fmod(angle, 360.0f);

	obj->setRot(angle);
}

float Asteroids::Player::getAngle() const{
	return angle;
}

void Asteroids::Player::setObj(Player::ObjPtr obj){
	Player::obj = obj;

}

const Asteroids::Player::ObjPtr& Asteroids::Player::getObj() const{
	return obj;
}

