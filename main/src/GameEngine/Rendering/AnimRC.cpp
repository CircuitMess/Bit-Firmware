#include "AnimRC.h"
#include <utility>

//parentSprite as nullptr is safe as long as you pass the correct parent Sprite* in the push() function
AnimRC::AnimRC(File file) : gif(std::make_unique<GIFSprite>(file)){
	gif->setLoopMode(loopMode);
}

void AnimRC::setAnim(File file){
	gif->stop();
//	gif.reset();
	gif = std::make_unique<GIFSprite>(file);
	gif->setLoopMode(loopMode);

	if(playing){
		gif->start();
	}
}

void AnimRC::start(){
	if(playing) return;

	gif->start();
	playing = true;
}

void AnimRC::stop(){
	if(!playing) return;

	gif->stop();
	playing = false;
}

void AnimRC::reset(){
	gif->reset();
}

void AnimRC::push(Sprite& parent, PixelDim pos, float rot, bool flipX, bool flipY) const{
	auto sprite = gif->getSprite();
	sprite.pushRotateZoom(&parent, std::round(pos.x + (float) sprite.width() / 2.0), std::round(pos.y + (float) sprite.height() / 2.0), rot, flipY ? -1 : 1, flipX ? -1 : 1, TFT_TRANSPARENT);
}

void AnimRC::setLoopDoneCallback(std::function<void(uint32_t)> cb){
	gif->setLoopDoneCallback(std::move(cb));
}

void AnimRC::setLoopMode(GIF::LoopMode loopMode){
	this->loopMode = loopMode;
	gif->setLoopMode(loopMode);
}

void AnimRC::update(uint32_t deltaMicros){
	if(!playing) return;

	gif->loop(deltaMicros);
}
