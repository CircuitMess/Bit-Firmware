#include "Planck.h"

Planck::Planck::Planck(Sprite& canvas): Game(canvas, Games::Planck, "/Games/Planck", {}){

}

void Planck::Planck::onLoad(){
	Game::onLoad();
}

void Planck::Planck::onLoop(float deltaTime){
	Game::onLoop(deltaTime);
}

void Planck::Planck::handleInput(const Input::Data& data){
	Game::handleInput(data);
}

void Planck::Planck::onStop(){
	Game::onStop();
}
