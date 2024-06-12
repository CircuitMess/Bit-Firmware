#include "Planck.h"

Planck::Planck::Planck(Sprite& canvas): Game(canvas, Games::Planck, "/Games/Planck", {
		{"/battery.raw", {}, true},
		{"/battery-bar.raw", {}, true},
		{"/battery-bar-line.raw", {}, true},
		{"/boost.raw", {}, true},
		{"/car.raw", {}, true},
		{"/cones.raw", {}, true},
		{"/life.raw", {}, true},
		{"/lr.raw", {}, true},
		{"/lw.raw", {}, true},
		{"/oil.raw", {}, true},
		{"/pickup1.raw", {}, true},
		{"/pickup2.raw", {}, true},
		{"/rail.raw", {}, true},
		{"/ramp.raw", {}, true},
		{"/road.raw", {}, true},
		{"/rr.raw", {}, true},
		{"/rw.raw", {}, true},
		{"/trash.raw", {}, true}
}){

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
