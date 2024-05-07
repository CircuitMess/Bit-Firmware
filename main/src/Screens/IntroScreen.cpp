#include "IntroScreen.h"
#include "MainMenu/MainMenu.h"

IntroScreen::IntroScreen(){
	lv_obj_set_size(*this, 128, 128);

	gif = new LVGIF(*this, "S:/Intro");
	lv_obj_set_pos(*gif, 0, 0);

	gif->setDoneCallback([this](){
		gif->stop();
		transition([](){ return std::make_unique<MainMenu>(); });
	});
}

void IntroScreen::onStart(){
	gif->start();
}

void IntroScreen::onStop(){
	gif->stop();
}
