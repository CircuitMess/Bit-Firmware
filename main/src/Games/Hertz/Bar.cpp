#include "Bar.h"

Bar::Bar(std::shared_ptr<Sprite> sprite) : sprite(sprite){
}

typedef struct {
	double r;       // a fraction between 0 and 1
	double g;       // a fraction between 0 and 1
	double b;       // a fraction between 0 and 1
} rgb;

typedef struct {
	double h;       // angle in degrees
	double s;       // a fraction between 0 and 1
	double v;       // a fraction between 0 and 1
} hsv;

static rgb hsv2rgb(hsv in);

rgb hsv2rgb(hsv in){
	double hh, p, q, t, ff;
	long i;
	rgb out;

	if(in.s <= 0.0){       // < is bogus, just shuts up warnings
		out.r = in.v;
		out.g = in.v;
		out.b = in.v;
		return out;
	}
	hh = in.h;
	if(hh >= 360.0) hh = 0.0;
	hh /= 60.0;
	i = (long) hh;
	ff = hh - i;
	p = in.v * (1.0 - in.s);
	q = in.v * (1.0 - (in.s * ff));
	t = in.v * (1.0 - (in.s * (1.0 - ff)));

	switch(i){
		case 0:
			out.r = in.v;
			out.g = t;
			out.b = p;
			break;
		case 1:
			out.r = q;
			out.g = in.v;
			out.b = p;
			break;
		case 2:
			out.r = p;
			out.g = in.v;
			out.b = t;
			break;

		case 3:
			out.r = p;
			out.g = q;
			out.b = in.v;
			break;
		case 4:
			out.r = t;
			out.g = p;
			out.b = in.v;
			break;
		case 5:
		default:
			out.r = in.v;
			out.g = p;
			out.b = q;
			break;
	}
	return out;
}

void Bar::resetGoal(){
	srand((unsigned) time(NULL));
	xGoal = rand() % (sprite->width() - 2);
	draw();
}

int Bar::getY(){
	return xGoal + 1; // + 1 to middle of the goal
}

void Bar::draw(){
	const auto width = sprite->width();
	const auto height = sprite->height();

	sprite->clear(TFT_TRANSPARENT);

	for(int x = 1; x < width - 1; x++){
		float difference = abs(xGoal - x);
		double hue = ((width - 2) - difference) / (float) (width - 2) * 60.0 / 255.0 * 360;
		rgb rgbColor0 = hsv2rgb({ hue, 1.0, 1.0 });
		uint16_t c0 = lgfx::color565(rgbColor0.r * 255.0, rgbColor0.g * 255.0, rgbColor0.b * 255.0);
		sprite->fillRect(x, 1, 1, height - 2, c0);
	}
	sprite->drawRoundRect(0, 0, width, height, 1, TFT_WHITE);
	sprite->fillRect(xGoal + 1, 1, 3, height - 2, TFT_DARKGREEN);
}

Pixel Bar::getColor(int diff){
	double hue = (100.0f - diff) / 100.0 * 60.0 / 255.0 * 360;
	rgb color = hsv2rgb({ hue, 1.0, 1.0 });

	return {
			(uint8_t) fmin(255.0, color.r * 255.0),
			(uint8_t) fmin(255.0, color.g * 255.0),
			(uint8_t) fmin(255.0, color.b * 255.0)
	};
}
