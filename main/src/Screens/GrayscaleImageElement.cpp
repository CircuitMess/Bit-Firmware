#include "GrayscaleImageElement.h"
#include <LV_Interface/FSLVGL.h>

GrayscaleImageElement::GrayscaleImageElement(lv_obj_t* parent, const std::string& path, const std::string& pathGrayscale, uint16_t width, uint16_t height,
											 bool grayscale) : LVObject(parent), path(path), pathGrayscale(pathGrayscale), grayscale(grayscale){
	lv_obj_set_size(*this, width, height);
	img = lv_img_create(*this);
	setGrayscale(grayscale);
}

bool GrayscaleImageElement::isGrayscale() const{
	return grayscale;
}

void GrayscaleImageElement::setGrayscale(bool value){
	grayscale = value;

	if(grayscale){
		FSLVGL::removeFromCache(path.c_str());
		FSLVGL::addToCache(pathGrayscale.c_str());
		lv_img_set_src(img, pathGrayscale.c_str());
	}else{
		FSLVGL::removeFromCache(pathGrayscale.c_str());
		FSLVGL::addToCache(path.c_str());
		lv_img_set_src(img, path.c_str());
	}
}
