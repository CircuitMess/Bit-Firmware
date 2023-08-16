#include "ProgressBar.h"

ProgressBar::ProgressBar(std::shared_ptr<Sprite> sprite) : sprite(sprite){
	fill(0.0f);
}

void ProgressBar::fill(float fillPercent){
	sprite->clear(TFT_TRANSPARENT);
	sprite->fillRoundRect(0, 0, sprite->width(), sprite->height(), 3, EmptyColor);
	sprite->fillRoundRect(1 + (1.0 - fillPercent) * (sprite->width() - 2), 1, fillPercent * (sprite->width() - 2), sprite->height() - 2, 2, FillColor);
}
