#include "Player.h"
#include "GameEngine/Collision/CircleCC.h"
#include "BobGame.h"

BobGame::Player::Player(File eat, File eatBad, File win, File dead) : eat(eat), eatBad(eatBad), win(win), dead(dead){
	go = std::make_shared<GameObject>(
			std::make_unique<AnimRC>(this->eat),
			std::make_unique<CircleCC>(collisionRadius, collisionOffset)
	);
	go->setPos({ (128 - width) / 2, 128 - height });
	anim = std::static_pointer_cast<AnimRC>(go->getRenderComponent());
	anim->start();
	anim->stop();
}

BobGame::Player::~Player(){
	anim->stop();
}

void BobGame::Player::loop(float deltaTime){
	if(eatDone){
		finishEating();
		eatDone = false;
	}

//	if(eating) return;
	float moveX = deltaTime * velocity * speed;
	moveX += go->getPos().x;
	if(moveX < 8 - width / 4 || moveX + width - 10 >= 128) return;
	go->setPos({ moveX, go->getPos().y });
}

void BobGame::Player::startEating(int value){
//	eating = true;
	if(value > 0){
		anim->setAnim(eat);
	}else{
		anim->setAnim(eatBad);
		eatingBad = true;
	}
	anim->start();

	anim->setLoopMode(GIF::Single);
	anim->setLoopDoneCallback([this](uint32_t t){
		eatDone = true;
	});
}

void BobGame::Player::finishEating(){
	anim->setAnim(eat);
	anim->start();
	anim->stop();
	anim->setLoopDoneCallback({});
//	eating = false;
	eatingBad = false;
}

std::shared_ptr<GameObject> BobGame::Player::getGameObject(){
	return go;
}

void BobGame::Player::filled(BobGame* game){
	anim->setAnim(win);
	anim->setLoopMode(GIF::Single);
	eatingBad = true;
	anim->setLoopDoneCallback([game](uint32_t){
		game->state = BobGame::WinPause;
	});
}

bool BobGame::Player::isEatingBad() const{
	return eatingBad;
}

void BobGame::Player::killed(BobGame* game){
	anim->setAnim(dead);
	anim->setLoopMode(GIF::Single);
	anim->setLoopDoneCallback([game](uint32_t){
		game->state = BobGame::DeadPause;
	});
}

void BobGame::Player::btnPressed(Input::Button btn){
	if(btn != Input::Left && btn != Input::Right) return;

	lastPressed = btn;

	if(btn == Input::Left){
		velocity = -1.0f;
	}else if(btn == Input::Right){
		velocity = 1.0f;
	}
}

void BobGame::Player::btnReleased(Input::Button btn){
	if(btn != Input::Left && btn != Input::Right) return;

	if(btn == lastPressed){
		velocity = 0.0f;
		lastPressed = Input::Menu;
	}
}
