#include "GrayscaleImageElement.h"

GrayscaleImageElement::GrayscaleImageElement(lv_obj_t* parent, const std::string& path, uint16_t width, uint16_t height, bool grayscale) : LVObject(parent), path(path), grayscale(grayscale){
	canvasData.resize(LV_CANVAS_BUF_SIZE_TRUE_COLOR(width, height), 0);

	lv_obj_set_size(*this, width, height);

	canvas = lv_canvas_create(*this);
	lv_obj_set_size(canvas, width, height);

	lv_canvas_set_buffer(canvas, canvasData.data(), width, height, LV_IMG_CF_TRUE_COLOR);

	setGrayscale(grayscale);
}

bool GrayscaleImageElement::isGrayscale() const{
	return grayscale;
}

void GrayscaleImageElement::setGrayscale(bool value){
	grayscale = value;

	lv_canvas_fill_bg(canvas, lv_color_black(), LV_OPA_COVER);

	lv_draw_img_dsc_t drawImgDsc;
	lv_draw_img_dsc_init(&drawImgDsc);

	lv_canvas_draw_img(canvas, 0, 0, path.c_str(), &drawImgDsc);

	if(grayscale){
		uint16_t* colorData = (uint16_t*) canvasData.data();
		for(size_t i = 0; i < canvasData.size() / (LV_COLOR_SIZE / 8); ++i){
			const uint16_t color = colorData[i];
			const uint16_t r = ((0xF800 & color) >> 11);
			const uint16_t g = ((0x7E0 & color) >> 5);
			const uint16_t b = 0x1F & color;
			const float grayscaleFactor = (0.2126f * r / 0x1F + 0.7152f * g / 0x3F + 0.0722f * b / 0x1F);

			colorData[i] = lv_color_to16(lv_color_make(grayscaleFactor * 0xFF, grayscaleFactor * 0xFF, grayscaleFactor * 0xFF));
		}
	}

	lv_obj_invalidate(canvas);
}
